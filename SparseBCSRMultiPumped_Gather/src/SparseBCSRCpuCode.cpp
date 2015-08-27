
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
    long long m, n, nnz, r, c, freq, depth;
    // it's a dense matrix
    r = SparseBCSR_R;
    c = SparseBCSR_C;
    depth = SparseBCSR_depth;
    n = 8;
    m = n * 2;
    nnz = m * n;
    
    printf("This is a simple test example of Gather Kernel\n");
    printf("R:\t %4d\nC:\t %4d\n", r, c);
    printf("M:\t %4d\nN:\t %4d\n", m, n);

    index_t *index = (index_t*) malloc(sizeof(index_t) * nnz);
    value_t *value = (value_t*) malloc(sizeof(value_t) * nnz);
    value_rom_t *rom = (value_rom_t *) malloc(sizeof(value_rom_t) * depth);

    for (int i = 0; i < nnz; i++) {
        index[i] = (i % (n * r)) / c;
        value[i] = (value_t) rand()/RAND_MAX;
    }
    for (int i = 0; i < depth; i++) {
        rom[i] = (value_rom_t) rand()/RAND_MAX;
    }

    max_file_t *mf =  SparseBCSR_init();
    max_engine_t *me = max_load(mf, "*");
    SparseBCSR_actions_t actions;

    actions.param_length = nnz;
    actions.instream_index = index;
    actions.instream_value = value;
    actions.inmem_SparseBCSRGatherKernel_ROM = rom;
    value_t **input_vector = &actions.outstream_inputVector0000;
    value_t **value_vector = &actions.outstream_valueVector0000;
    for (int i = 0; i < r; i++) {
        input_vector[i] = (value_t *) malloc(sizeof(value_t) * nnz / r);
        value_vector[i] = (value_t *) malloc(sizeof(value_t) * nnz / r);
    }

    printf("Running DFE ...\n");
    SparseBCSR_run(me, &actions);

    for (int i = 0; i < r; i++) {
        printf("For pipe %04d: \n", i);
        for (int j = 0; j < nnz / r; j++) {
            value_t read    = input_vector[i][j];
            value_t expect  = rom[index[j * r + i]];
            printf("\t[%4d] %.6f %.6f", j, read, expect);
            if (abs(read-expect)/expect > 1e-5) 
                printf("\tERROR\n");
            else
                printf("\tOK\n");
        }
    }
    SparseBCSR_free();
    return 0;
}
