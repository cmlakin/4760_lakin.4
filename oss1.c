#include "config.h"

// shared memory
// 		/* own simulated time maintained by os - otime: only oss changes clock */
// 		clock: int seconds, int nano
//
// 		/* priority queues */
// 		queue1 - high priority
// 		queue2 - low priority
// 		queue3 - blocked
//
// 		/* process table - ptable: only 18 entries */
//
// 		/* process control block - pcb: max of 18 */
//		keep track of statistics:
//				- pid
//				- type of process (will determine priority)
//				- time started
//				- how long it ran for
//				- time finished
//				- what queue it was put into

// TODO randomly launch new process

// TODO send msg to uprocess and let it know how much time it has to run

// TODO random time between launching a new process
// int maxTimeBetweenNewProcsNS;
// int maxTimeBetweenNewProcsSecs;



int main(int argc, char ** argv){

	int option; // parse command line arguements
	char * filename; // filename pointer
	pid_t pid; 
	
	while((option = getopt(argc, argv, "hs:l:")) != -1){

		switch(option){
			case 'h':
					fprintf(stderr, "usage: %s -h\n", argv[0]);
					exit(-1);
			case 's':
					fprintf(stderr, "usage: %s -s\n", argv[0]);
					printf("Enter maximum number of seconds\n");
					break;
			case 'l':
					fprintf(stderr, "usage: %s -l\n", argv[0]);
					printf("Specify a particular name for the log file\n");
					break;
			default:
					fprintf(stderr, "errno: %i\n", errno);
		}

		if(optind < argc){
			while(optind < argc){
				filename = argv[optind];
				optind++;
			}
		}
		else{
			filename = "oss.log";
		}
	}



}
