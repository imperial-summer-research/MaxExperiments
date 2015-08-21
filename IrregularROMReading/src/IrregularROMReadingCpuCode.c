
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Maxfiles.h"
#include <MaxSLiCInterface.h>

typedef uint32_t index_t;
typedef float    value_t;
typedef double	 value_rom_t;

void run(int size, index_t *index, value_t *value, value_rom_t *rom) {
	
	IrregularROMReading(size, index, value, rom);

	printf("Running DFE ...\n");

	struct timeval t0, t1;
    gettimeofday(&t0, 0);
	IrregularROMReading(size, index, value, rom);
	gettimeofday(&t1, 0);

	printf("Finished.\n");

	double duration = (double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_usec-t0.tv_usec)/1e6;
    printf("Total time %lf s per element time %lf us\n", 
        duration, duration * 1e6 / size);

	// checking
	for (int i = 0; i < size; i++)
		if (value[i] != (value_t) rom[index[i]] * 3)
			printf("ERROR: [%6d] = %.6f %.6f\n", value[i], rom[index[i]] * 3);
	printf("OK\n");
}

int main(int argc, char *argv[]) 
{
	int size = 81920;
	int rom_size = 8192;
	index_t *index = (index_t*) malloc(sizeof(index_t) * size);
	value_t *value = (value_t*) malloc(sizeof(value_t) * size);
	value_rom_t *rom = (value_rom_t*) malloc(sizeof(value_rom_t) * rom_size);

	for (int i = 0; i < rom_size; i++)
		rom[i] = (double) rand()/RAND_MAX;

	printf("Testing regular reading ...\n");
	for (int i = 0; i < size; i++) 
		index[i] = i % rom_size;
	// heating up
	IrregularROMReading(size, index, value, rom);
	run(size, index, value, rom);

	int stride = 2;
	for (; stride < 8192; stride *= 2) {
		printf("Testing stride %4d reading ...\n", stride);
		int curr = 0;
		for (int i = 0; i < size; i++, curr+=stride) 
			index[i] = (curr+i) % rom_size;
		IrregularROMReading(size, index, value, rom);
		run(size, index, value, rom);
	}

	printf("Testing random reading ...\n");
	for (int i = 0; i < size; i++) {
		index[i] = (double) rand()/RAND_MAX * rom_size;
	}
	run(size, index, value, rom);
    return 0;
}
