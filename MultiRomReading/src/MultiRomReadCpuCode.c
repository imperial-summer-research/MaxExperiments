
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Maxfiles.h"
#include <MaxSLiCInterface.h>

typedef uint32_t index_t;
typedef float    value_t;
typedef double	 value_rom_t;

const int num_pipes = MultiRomRead_numPipes;
const int num_ports = MultiRomRead_numPorts;

void run(int size, int rom_size, index_t *index, value_t *value, value_rom_t *rom) {

	printf("Num of Pipes: %d\n", num_pipes);
	printf("Num of Ports: %d\n", num_ports);
	printf("Num of ROMs:  %d\n", num_pipes/num_ports);

	MultiRomRead_actions_t actions;
	actions.param_length = size;
	actions.instream_index = index;
	actions.outstream_value = value;

	value_rom_t **roms = (value_rom_t **) &(actions.inmem_MultiRomReadKernel_ROM0000);
  for (int i = 0; i < num_pipes; i++) {
      roms[i] = (value_rom_t *) malloc(sizeof(value_rom_t) * rom_size);
      memcpy(roms[i], rom, sizeof(value_rom_t) * rom_size);
  }
	max_file_t *mf =  MultiRomRead_init();
	max_engine_t *me = max_load(mf, "*");

	printf("Running DFE ...\n");

	struct timeval t0, t1;
  gettimeofday(&t0, 0);
	MultiRomRead_run(me, &actions);
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
	run(size, rom_size, index, value, rom);
    return 0;
}
