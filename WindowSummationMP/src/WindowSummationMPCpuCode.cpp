
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
    int C = WindowSummationMP_C;
    int depth = WindowSummationMP_depth;
    int iter = 100;
    LL N = 8192 * 8192;

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
    printf("C:\t %d\n", C);
    printf("depth:\t %d\n", depth);
    printf("N:\t %lld\n", N);

    max_file_t *mf = WindowSummationMP_init();
    max_engine_t *me = max_load(mf, "*");

    WindowSummationMP_Init_actions_t init_actions;
    value_rom_t **roms = (value_rom_t **) &init_actions.inmem_WindowSummationMPGatherKernel_ROM0000;
    for (int i = 0; i < W/C/2; i++) {
        roms[i] = (value_rom_t *) malloc(sizeof(value_rom_t) * depth);
        memcpy(roms[i], rom, sizeof(value_rom_t) * depth);
    }

    WindowSummationMP_Compute_actions_t actions;
    actions.param_length = N;
    actions.instream_index = index;
    actions.outstream_result = result;
    
    printf("Setting ROM ...\n");
    WindowSummationMP_Init_run(me, &init_actions);

    printf("Heating DFE ...\n");
    WindowSummationMP_Compute_run(me, &actions);

    printf("Running DFE ...\n");
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    for (int i = 0; i < iter; i++)
        WindowSummationMP_Compute_run(me, &actions);
    gettimeofday(&t1, 0);

    max_unload(me);

    double duration = (double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_usec-t0.tv_usec)/1e6;
    duration /= iter;
    printf("Total time %lf s per element time %lf us freq(elem) %.2f MHz freq(tick) %.2f MHz bandwidth %.2f MB/s\n", 
        duration, 
        duration * 1e6 / N,
        1.0 / (duration / N) / 1e6,
        1.0 / (duration / (N/W)) / 1e6,
        sizeof(value_t) / (duration / N) / 1e6);

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
