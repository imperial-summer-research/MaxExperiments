
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


int main(int argc, char *argv[]) {
    
    int N = 6144;
    uint32_t *inA_stream   = (uint32_t*) malloc(sizeof(uint32_t) * N);
    uint32_t *inB_stream   = (uint32_t*) malloc(sizeof(uint32_t) * N);
    uint32_t *oData_stream = (uint32_t*) malloc(sizeof(uint32_t) * N);

    for (int i = 0; i < N; i++) {
        inA_stream[i] = i;
        inB_stream[i] = i;
    }

    LMemExample_writeLMem(N, 0, inA_stream);
    LMemExample_writeLMem(N, N, inB_stream);
    LMemExample(N);
    LMemExample_readLMem(N, 2*N, oData_stream);

    for (int i = 0; i < N; i++) {
        uint32_t expect = inA_stream[i] + inB_stream[i];
        uint32_t result = oData_stream[i];
        if (expect != result) {
            fprintf(stderr, "[%4d] %u != %u\n", i, expect, result);
            exit(1);
        } 
    }
    printf("OK\n");
    return 0;
}
