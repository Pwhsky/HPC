to run valgrind memory check: 

valgrind -s ./myprogram






Commenting out initialization of as gives segmentation fault "Access not within mapped region at address 0x0".

commenting out free will return "Definetly lost", and means the reference was not freed prior to program exit, and is occupied. (modern OS will take care of this leak though).

adding more free's will give segmentationfault and try to free memory at an adress which has no memory allocated.

