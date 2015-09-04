
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
    
    int P = DotProductMP_P;
    int C = DotProductMP_C;
    LL N = 8192 * 8192;

    value_t *x = (value_t *) malloc(sizeof(value_t) * N);
    value_t *y = (value_t *) malloc(sizeof(value_t) * N);
    value_t *r = (value_t *) malloc(sizeof(value_t) * N);

    for (int i = 0; i < N; i++) {
        x[i] = (value_t) rand()/RAND_MAX;
        y[i] = (value_t) rand()/RAND_MAX;
    }
    printf("P:\t %d\n", P);
    printf("C:\t %d\n", C);
    printf("N:\t %lld\n", N);

    printf("Heating DFE ...\n");
    DotProductMP(N, x, y, r);

    printf("Running DFE ...\n");
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    DotProductMP(N, x, y, r);
    gettimeofday(&t1, 0);
    double duration = (double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_usec-t0.tv_usec)/1e6;
    printf("Total time %lf s per element time %lf us\n", duration, duration * 1e6 / N);

    for (int i = 0; i < N; i++) {
        value_t expect = x[i] * y[i];
        if (abs(expect-r[i])/r[i] > 1e-6) {
            printf("ERROR: [%5d]\t %.6f %.6f\n", i, expect, r);
            exit(1);
        }
    }
    printf("OK!\n");

    return 0;
}
