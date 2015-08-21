
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Maxfiles.h"
#include <MaxSLiCInterface.h>
//#include <MaxCompilerRT.h>

float rand_number() { return (float)rand()/RAND_MAX; }

const int min_PCI_stream_length = SummationTree_minPCIStreamLength;
const int data_per_row = SummationTree_dataPerRow;

int main(int argc, char *argv[]) 
{
	srand(time(NULL));
	int num_row = min_PCI_stream_length * 4;
	int data_size = data_per_row * num_row;

	printf("Min PCI stream:\t %d\n", min_PCI_stream_length);
	printf("Num of Row:\t %d\n", num_row);
	printf("Data per row:\t %d\n", data_per_row);
	printf("Data size:\t %d\n", data_size);
	float *input 	= (float *) malloc(sizeof(float) * data_size);
	float *output 	= (float *) malloc(sizeof(float) * num_row);
	float *expected = (float *) malloc(sizeof(float) * num_row);

	for (int i = 0; i < data_size; i++) 
		input[i] = rand_number();
	for (int i = 0; i < num_row; i++)
		for (int j = 0; j < data_per_row; j++)
			expected[i] += input[i*data_per_row+j];

	printf("Running DFE ...\n");
	
	struct timeval t0, t1;
    gettimeofday(&t0, 0);
	SummationTree(data_size, input, output);
	gettimeofday(&t1, 0);

	printf("Finished.\n");

	double duration = (double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_usec-t0.tv_usec)/1e6;
    printf("Total time %lf s per element time %lf us\n", 
        duration, duration * 1e6 / data_size);

	for (int i = 0; i < num_row; i++)
		printf("output[%3d] = %10.6f\t %10.6f\t %s\n", 
			i, 
			output[i],
			expected[i],
			abs(expected[i]-output[i])/output[i] < 1e-6 
				? "OK" 
				: "ERROR");

    return 0;
}
