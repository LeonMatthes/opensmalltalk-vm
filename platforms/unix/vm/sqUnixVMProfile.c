/****************************************************************************
*   PROJECT: Unix (pthread/nanosleep/sigaction/SIGPROF) profiling logic for
*			 statistical profiling of the VM
*   FILE:    sqUnixVMProfile.c
*   CONTENT: 
*
*   AUTHOR:  Eliot Miranda
*   ADDRESS: 
*   EMAIL:   eliot@qwaq.com
*   RCSID:   $Id$
*
*   NOTES: 
*  August 5th, 2008, EEM wrote pc sampling code
*
*****************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h" /* this must happen before including std libraries */
#endif

#if NO_VM_PROFILE
#include "sq.h"

void 
ioNewProfileStatus(sqInt *running, long *buffersize)
{
	if (running)
		*running = 0;
	if (buffersize)
		*buffersize = 0;
}

long
ioNewProfileSamplesInto(void *sampleBuffer)
{
	return 0;
}

long
ioControlNewProfile(int on, unsigned long buffer_size)
{
	return 0;
}

void
ioClearProfile(void)
{
}
#else /* NO_VM_PROFILE */
#include <pthread.h>
#include "include_ucontext.h"
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#if __APPLE__ && __MACH__ /* Mac OS X */
# include <mach-o/getsect.h>
extern unsigned long _start;
#elif defined(__linux__)
extern unsigned long _start;
extern unsigned long _etext;
#endif

#include <errno.h>

#include "sq.h"

/*
 * The pc collection scheme is an event buffer into which are written pcs.  The
 * image then builds up the histogram from the samples.  8 meg of buffer is 23
 * minutes of 32-bit pc samples at 1.5KHz, plenty for practical profiling.
 */

#if LLP64
typedef unsigned long long pctype;
#else
typedef unsigned long pctype;
#endif

static void
bail_out(int err, char *err_string)
{
	errno = err;
	perror(err_string);
	exit(1);
}

typedef enum { dead, nascent, quiescent, active } machine_state;
machine_state profileState = dead,
			  nextState = dead;

#define PROFILE_USECS 666 /* chosen to be out of phase with the delay resolution of 1000 */
static volatile struct timespec proftime = { 0, PROFILE_USECS * 1000};

static int					profThreadPolicy;
static struct sched_param	profThreadPriority;

static pthread_t *threads = NULL;
static int threadCount = 0;

static void *
profileStateMachine(void *ignored)
{
	int er;
	/* if ((er = pthread_setschedparam(pthread_self(), */
	/*                                 profThreadPolicy, */
	/*                                 &profThreadPriority))) */
	/*     bail_out(er,"pthread_setschedparam failed"); */

	profileState = quiescent;
	while (profileState != dead) {
		struct timespec naptime;
		naptime = proftime;
		while (nanosleep(&naptime, &naptime) == -1) /* repeat */;
		if (profileState == active) {
			for (int i = 0; i < threadCount; i++) {
				union sigval signal_value;
				signal_value.sival_int = i;

				// We use sigqueue here instead of "kill" because we need to
				// send the thread-index along, so that the thread knows which
				// buffer to write its PC into.
				pthread_sigqueue(threads[i], SIGPROF, signal_value);
			}
		}
		profileState = nextState;
	}
	return 0;
}

static void
setState(machine_state desiredState)
{
	struct timespec halfAMo;

	if (profileState == desiredState)
		return;
	nextState = desiredState;
	halfAMo.tv_sec  = 0;
	halfAMo.tv_nsec = 1000 * 100;
	while (profileState != desiredState)
		nanosleep(&halfAMo, 0);
}

/*
 * Example of 2meg buffer (512k samples) gives nearly 6 minutes of profile
 * (nearly 3 minutes in 64-bits).
 *
 * 2 * 1024 * 1024  = 512 * 1024 pcs (each pc 4 bytes)
 * Sampling frequency is 1000000 usecs / 666 usecs = 1501 Hz (1501 pcs / sec)
 * 512 * 1024 / 1501 = 349 seconds = 5.8 minutes
 */
typedef struct pc_buffer {
	pctype *pc_buffer;
	long pc_buffer_index;
	long pc_buffer_wrapped;
} pc_buffer_t;

static pc_buffer_t *pc_buffers;
static long pc_buffer_size;
static long pc_buffers_count;

static void
pcbufferSIGPROFhandler(int sig, siginfo_t *info, void *context)
{
	ucontext_t *uap = (ucontext_t *) context;
	int thread_index = info->si_value.sival_int;

	pc_buffer_t *buffer = &pc_buffers[thread_index];
	buffer->pc_buffer[buffer->pc_buffer_index] = uap->_PC_IN_UCONTEXT;
	if (++(buffer->pc_buffer_index) >= pc_buffer_size) {
		buffer->pc_buffer_index = 0;
		buffer->pc_buffer_wrapped = 1;
	}
}

void allocThreadInfos() {
#if COGMTVM
	threadCount = ioGetThreadCount();
	threads = realloc(threads, threadCount * sizeof(pthread_t));
	for (long i = 0; i < threadCount; i++) {
		threads[i] = getVMOSThreadAt(i);
	}
#else
	threads = realloc(threads, sizeof(pthread_t));
	*threads = pthread_self();
	threadCount = 1;
#endif
}

static void
initProfileThread()
{
	int er;
	struct sigaction sigprof_handler_action;
	pthread_t careLess;

	sigprof_handler_action.sa_sigaction = pcbufferSIGPROFhandler;
	sigprof_handler_action.sa_flags = SA_NODEFER | SA_SIGINFO;
	sigemptyset(&sigprof_handler_action.sa_mask);
	(void)sigaction(SIGPROF, &sigprof_handler_action, 0);

	allocThreadInfos();

	if ((er = pthread_getschedparam(pthread_self(),
									&profThreadPolicy,
									&profThreadPriority)))
		bail_out(er,"pthread_getschedparam failed");
	/* add 2 to priority to be above poll heartbeat thread, which we
	 * want to profile.
	 */
	profThreadPriority.sched_priority += 2;
	/* If the priority isn't appropriate for the policy (typically SCHED_OTHER)
	 * then change policy.
	 */
	if (sched_get_priority_max(profThreadPolicy) < profThreadPriority.sched_priority)
		profThreadPolicy = SCHED_FIFO;
	profileState = nascent;
	if ((er= pthread_create(&careLess,
							(const pthread_attr_t *)0,
							profileStateMachine,
							(void *)0)))
		bail_out(er,"profile thread creation failed");
	setState(quiescent);
}

void free_pc_buffers() {
	if (pc_buffers) {
		for (long i = 0; i < pc_buffers_count; i++) {
			free(pc_buffers[i].pc_buffer);
		}
		free(pc_buffers);
	}
	pc_buffers_count = 0;
}

void alloc_pc_buffers(long count, unsigned long buffer_size) {
	pc_buffers = calloc(count, sizeof(pc_buffer_t));

	for (long i = 0; i < count; i++) {
		pc_buffer_t *buffer = &pc_buffers[i];
		buffer->pc_buffer = calloc(buffer_size, sizeof(pctype));
	}
	ioClearProfile();

	pc_buffers_count = count;
	pc_buffer_size = buffer_size;
}

long
ioControlNewProfile(int on, unsigned long buffer_size)
{
	if (buffer_size
	 && pc_buffer_size < buffer_size) {
		if (profileState == active)
			setState(quiescent);

		free_pc_buffers();
		
#if COGMTVM
		alloc_pc_buffers(ioGetThreadCount(), buffer_size);
#else
		alloc_pc_buffers(1, buffer_size);
#endif
	}
	if (profileState == dead)
		initProfileThread();
   	setState(on ? active : quiescent);

	// TODO: What should be the return value here?
	// We somehow need to tell the profiler how many samples we got.
	if (pc_buffers_count == 0) {
		return 0;
	}
	pc_buffer_t *buffer = &pc_buffers[0];
	return buffer->pc_buffer_wrapped ? pc_buffer_size : buffer->pc_buffer_index;
}

void 
ioNewProfileStatus(sqInt *running, long *buffersize)
{
	if (running)
		*running = profileState == active;
	if (buffersize)
		*buffersize = pc_buffer_size;
}

long
ioNewProfileThreadSamplesInto(long threadIndex, void *sampleBuffer)
{
	if (threadIndex < 0 || pc_buffers_count <= threadIndex) {
		return 0;
	}

	pc_buffer_t *buffer = &pc_buffers[threadIndex];
	if (!buffer->pc_buffer_wrapped) {
		memcpy(sampleBuffer,
				buffer->pc_buffer,
				buffer->pc_buffer_index * sizeof(pctype));
		return buffer->pc_buffer_index;
	}


	memcpy(sampleBuffer,
		   buffer->pc_buffer + buffer->pc_buffer_index,
		   (pc_buffer_size - buffer->pc_buffer_index) * sizeof(pctype));
	memcpy((pctype *)sampleBuffer + (pc_buffer_size - buffer->pc_buffer_index),
		   buffer->pc_buffer,
		   buffer->pc_buffer_index * sizeof(pctype));
	return pc_buffer_size;
}

void
ioClearProfile(void)
{
	for(long i = 0; i < pc_buffers_count; i++) {
		pc_buffer_t *buffer = &pc_buffers[i];
		buffer->pc_buffer_index = 0;
		buffer->pc_buffer_wrapped = 0;
	}
}
#endif /* NO_VM_PROFILE */
