
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
    
    int W = WindowSummationMP_W;
    int depth = WindowSummationMP_depth;
    LL N = 128;

    index_t *index  = (index_t *) malloc(sizeof(index_t) * N);
    value_t *result = (value_t *) malloc(sizeof(value_t) * N / W);
    value_rom_t *rom = (value_rom_t *) malloc(sizeof(value_rom_t) * depth);

    for (int i = 0; i < N; i++) {
        index[i] = i % depth;   
    }
    for (int i = 0; i < depth; i++) {
        rom[i] = (value_rom_t) rand()/RAND_MAX;
    }
    printf("W:\t %d\n", W);
    printf("depth:\t %d\n", depth);
    printf("N:\t %lld\n", N);

    printf("Heating DFE ...\n");
    WindowSummationMP(N, index, result, rom);

    printf("Running DFE ...\n");
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    WindowSummationMP(N, index, result, rom);
    gettimeofday(&t1, 0);
    double duration = (double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_usec-t0.tv_usec)/1e6;
    printf("Total time %lf s per element time %lf us\n", duration, duration * 1e6 / N);

    for (int i = 0; i < N/W; i++) {
        value_t expect = 0.0;
        for (int j = 0; j < W; j++)
            expect += rom[index[i*W+j]];
        if (abs(expect-result[i])/result[i] > 1e-6) {
            printf("ERROR: [%5d]\t %.6f %.6f\n", i, expect, result[i]);
            exit(1);
        }
    }
    printf("OK!\n");

    return 0;
}
