/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * main.c
 */

#include "system.h"
#include "scheduler.h"

static void
_thread_(void *arg)
{
	const char *name;
	int i;
	/* print name and count */
	name = (const char *)arg;
	for (i=0; i<100; ++i) {
		printf("%s %d\n", name, i);
		us_sleep(20000);
		/* implement this*/
		scheduler_yield();
	}
}

int
main(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	if (scheduler_create(_thread_, "hello") ||
	    scheduler_create(_thread_, "world") ||
	    scheduler_create(_thread_, "love") ||
	    scheduler_create(_thread_, "this") ||
	    scheduler_create(_thread_, "course!")) {
		TRACE(0);
		/* destroy(); */
		return -1;
	}
	scheduler_execute();
	return 0;
}

/*

#include <signal.h>
if(SIG_ERR = signal(SIG_ALRM, fnc)){
	error
}
...
...
// one shot  alarm
alarm(...);

void fnc(int i){
	assert(SIGALRM == i)
	...
	alarm(1);
}
*/
