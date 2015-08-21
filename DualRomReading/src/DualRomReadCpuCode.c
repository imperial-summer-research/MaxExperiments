
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Maxfiles.h"
#include <MaxSLiCInterface.h>

typedef uint32_t index_t;
typedef float    value_t;
typedef double	 value_rom_t;

const int num_ports = DualRomRead_numPorts;

#if DualRomRead_numPorts == 1 
	#define CALL_DFE(s, i, v, r, n) DualRomRead(s, i, v, r, r); 
#else 
	#define CALL_DFE(s, i, v, r, n) DualRomRead(s, i, v, r); 
#endif
	

void run(int size, index_t *index, value_t *value, value_rom_t *rom) {
	printf("Num Ports: %d\n", num_ports);
	CALL_DFE(size, index, value, rom, num_ports);

	printf("Running DFE ...\n");

	struct timeval t0, t1;
    gettimeofday(&t0, 0);
	CALL_DFE(size, index, value, rom, num_ports);
	gettimeofday(&t1, 0);

	printf("Finished.\n");

	double duration = (double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_usec-t0.tv_usec)/1e6;
    printf("Total time %lf s per element time %lf us\n", 
        duration, duration * 1e6 / size);

	// checking
	for (int i = 0; i < size; i++)
		if (value[i] != (value_t) rom[index[i]])
			printf("ERROR: [%6d] = %.6f %.6f\n", value[i], rom[index[i]] * 3);
	printf("OK\n");
}

int main(int argc, char *argv[]) 
{
	int size = 8192;
	int rom_size = 8192;
	index_t *index = (index_t*) malloc(sizeof(index_t) * size);
	value_t *value = (value_t*) malloc(sizeof(value_t) * size);
	value_rom_t *rom = (value_rom_t*) malloc(sizeof(value_rom_t) * rom_size);

	for (int i = 0; i < rom_size; i++)
		rom[i] = (double) rand()/RAND_MAX;
	for (int i = 0; i < size; i++) 
		index[i] = i % size;
	run(size, index, value, rom);
    return 0;
}
