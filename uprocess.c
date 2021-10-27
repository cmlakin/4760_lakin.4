#include "config.h"



// TODO wait for message from oss



// TODO determine % of how many can be I/O bound and CPU processes



// TODO pick random number to determine if I/O bound (1) or CPU (0)



// TODO randomly pick operation choice:	
// 		0 - use all time allowed (higher chance for CPU)
// 		1 - terminate (chance of this option should be low)
// 		2 - use part of time (higher chance for I/O bound)
// 		



// TODO send msg back to oss:
// 		- what type of process it is
// 		- operation number it chose
// 		- how much time it ran for (add this to the otime clock)
//			- if terminate: send msg to oss then terminate itself
//
