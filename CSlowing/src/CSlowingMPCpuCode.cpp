
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>

// -- C Library
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <cstdint>

#include "Maxfiles.h"
#include <MaxSLiCInterface.h>

using namespace std;

typedef uint32_t index_t;
typedef float    value_t;
typedef double	 value_rom_t;
typedef long long LL;

int main(int argc, char *argv[]) {
    
    int C = CSlowingMP_C;
    int N = 8192;

    value_t *input  = (value_t *) malloc(sizeof(value_t) * N);
    value_t *expect = (value_t *) malloc(sizeof(value_t) * N);
    value_t *output = (value_t *) malloc(sizeof(value_t) * N);

    for (int i = 0; i < N; i++) 
        input[i] = (value_t) rand() / RAND_MAX;
    for (int i = 0; i < N; i+=C) 
        for (int j = 0; j < C; j++)
            expect[i+j] = (i == 0) ? input[i+j] : input[i+j] + expect[i-C+j];

    printf("Heating DFE ...\n");
    CSlowingMP(N, input, output);

    printf("Running DFE ...\n");
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    CSlowingMP(N, input, output);
    gettimeofday(&t1, 0);
    double duration = (double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_usec-t0.tv_usec)/1e6;
    printf("Total time %lf s per element time %lf us\n", duration, duration * 1e6 / N);

    for (int i = 0; i < N; i++) {
        if (abs(expect[i]-output[i])/output[i] > 1e-6) {
            printf("ERROR: [%5d]\t %.6f %.6f\n", i, expect[i], output[i]);
            exit(1);
        }
    }
    printf("OK!\n");

    return 0;
}
