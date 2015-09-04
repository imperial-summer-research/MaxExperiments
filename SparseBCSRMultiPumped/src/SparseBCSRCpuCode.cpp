
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

int loopLength = 15;

int main(int argc, char *argv[]) {
    long long m, n, nnz, r, c, freq, depth;
    // it's a dense matrix
    r = SparseBCSR_R;
    c = SparseBCSR_C;
    depth = SparseBCSR_depth;
    n = 8;
    m = n * loopLength;
    nnz = m * n;
    
    printf("This is a simple test example of BCSR Multipumped SpMV\n");
    printf("R:\t %4d\nC:\t %4d\n", r, c);
    printf("M:\t %4d\nN:\t %4d\n", m, n);
    printf("NNZ:\t %4d\n", nnz);

    index_t *index      = (index_t*) malloc(sizeof(index_t) * nnz);
    index_t *start      = (index_t*) malloc(sizeof(index_t) * nnz / (r * c));
    value_t *value      = (value_t*) malloc(sizeof(value_t) * nnz);
    value_rom_t *rom    = (value_rom_t *) malloc(sizeof(value_rom_t) * depth);

    int num_blk_nnz     = (r * c);
    int num_blks        = nnz / (r * c);
    int num_col_blks    = n / c;
    int num_row_blks    = m / r;
    int num_chunk_blks  = num_col_blks * loopLength;
    int num_chunks      = num_blks / num_chunk_blks;

    printf("Blocks:\t %4d\n", num_blks);
    printf("Chunks:\t %4d\n", num_chunks);
    for (int i = 0; i < num_chunks; i++) {
        for (int j = 0; j < num_col_blks; j++) {
            for (int k = 0; k < loopLength; k++) {
                for (int x = 0; x < c; x++) {
                    for (int t = 0; t < r; t++) {
                        int idx = i * num_chunk_blks * num_blk_nnz +
                                  j * num_row_blks * num_blk_nnz +
                                  k * num_blk_nnz +
                                  x * r +
                                  t;
                        index[idx] = x + j * c;
                        value[idx] = (value_t) rand()/RAND_MAX; 
                    }
                }
                int blk_id = i * num_chunk_blks + j * num_row_blks + k;
                start[blk_id] = (j == 0);
            }
        }
    }

    for (int i = 0; i < depth; i++) {
        rom[i] = (value_rom_t) rand()/RAND_MAX;
    }

    printf("Loading Maxfile to Max Board ...\n");
    max_file_t *mf =  SparseBCSR_init();
    max_engine_t *me = max_load(mf, "*");
    SparseBCSR_actions_t actions;

    actions.param_length = nnz;
    actions.instream_index = index;
    actions.instream_value = value;
    actions.instream_start = start;
    actions.inmem_SparseBCSRGatherKernel_ROM = rom;
    actions.outstream_result = (value_t *) malloc(sizeof(value_t) * nnz / c);

    value_t *result = actions.outstream_result;

    printf("Running DFE ...\n");
    SparseBCSR_run(me, &actions);
    printf("Finished\n");
    for (int i = 1; i <= m/r; i++) {
        for (int j = 0; j < r; j++) {
            value_t read = 0.0;
            int rid = i * (n/c) * r - (r-j);
            read = result[rid];
            //printf("%d\n", rid);
            value_t expect = 0.0;
            for (int k = 0; k < n/c; k++)
                for (int t = 0; t < c; t++) {
                    int idx = (i-1) * n * r + k * c * r + t * r + j;
                    //printf("%d\n", idx);
                    expect += (value_t) rom[index[idx]] * value[idx];
                }
            //if (abs(read-expect)/expect > 1e-6) {
                printf("[%04d] %.6f %.6f\t ERROR\n", (i-1)*r+j, read, expect);
            //    exit(1);
            //}
        }
    }
    printf("OK\n");

    SparseBCSR_free();
    return 0;
}
