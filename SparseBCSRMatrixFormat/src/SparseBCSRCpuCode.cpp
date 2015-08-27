
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
    long long m, n, nnz, r, c, freq;
    // it's a dense matrix
    r = SparseBCSR_R;
    c = SparseBCSR_C;
    n = SparseBCSR_depth;
    
    for (int t = 1; t < 10; t++) {
        printf("Running test case %d\n", t);
        m = n * t;
        nnz = m * n;
        freq = SparseBCSR_freq;

        printf("%4d X %4d block for %4d X %4d matrix\n", r, c, m, n);
        printf("Number of Non zeros:\t%d\n", nnz);
        printf("Memory size for matrix:\t%.f GB\n", (double)sizeof(index_t) * nnz * 3 / (1024 * 1024 * 1024));

        // Original COO format
        index_t *row = (index_t *) malloc(sizeof(index_t) * nnz);
        index_t *col = (index_t *) malloc(sizeof(index_t) * nnz);
        value_t *val = (value_t *) malloc(sizeof(value_t) * nnz);

        for (int i = 0; i < nnz; i++) {
            row[i] = i / n;
            col[i] = i % n;
            val[i] = 0.01;
        }
        printf("Generated COO Matrix\n");
        
        // BCSR format: 
        // param{index}: matrix element's column index
        // param{value}: matrix element's value
        // param{input}: vector value input
        vector< vector<index_t> > index_queues(m, vector<index_t>(0));
        vector< vector<value_t> > value_queues(m, vector<value_t>(0));
        for (int i = 0; i < nnz; i++) {
            index_queues[row[i]].push_back(col[i]);
            value_queues[row[i]].push_back(val[i]);
        }
        printf("Transformed to intermediate queues\n");
        vector<index_t> index;
        vector<value_t> value;
        vector<index_t> start;
        // padding
        long long pad_num_row = ceil((double)m/r) * r;
        long long pad_num_col = ceil((double)n/c) * c;
        long long pad_nnz = 0;

        value_t *vec = (value_t *) malloc(sizeof(value_t) * pad_num_col);
        value_t *res = (value_t *) malloc(sizeof(value_t) * pad_num_row);
        for (int i = 0; i < pad_num_col; i++) 
            vec[i] = 0.01;
        for (int i = 0; i < pad_num_row; i++)
            res[i] = 0.0;

        // generalized iteration
        for (int i = 0; i < pad_num_row; i += r) {
            for (int j = 0; j < pad_num_col; j += c) {
                start.push_back((j == 0));
                bool is_empty = true;
                for (int _i = 0; _i < r; _i++) {
                    int _r = i + _i;
                    int _c = j;
                    if (_r < m && _c < index_queues[_r].size())
                        is_empty = false; 
                }
                if (is_empty) 
                    break;
                // for each block
                for (int _j = 0; _j < c; _j ++) {
                    for (int _i = 0; _i < r; _i ++) {
                        int _r = i + _i;
                        int _c = j + _j;
                        index_t _index = (_r >= m || _c >= index_queues[_r].size()) ? 0   : index_queues[_r][_c];
                        value_t _value = (_r >= m || _c >= value_queues[_r].size()) ? 0.0 : value_queues[_r][_c];
                        index.push_back(_index);
                        value.push_back(_value);
                        pad_nnz ++;
                    }
                }
            }
        }

        // SEND TRANSFORMED RESULT TO BOARD
        cout << "Ready to send data to Max3 Board ..." << endl;
        cout << "Number of non zeros(padded):\t"    << pad_nnz << endl;
        cout << "Number of rows(padded):\t\t"       << pad_num_row << endl;
        cout << "Number of cols(padded):\t\t"       << pad_num_col << endl;
        cout << "Number of blocks(r X c):\t"        << pad_nnz / (r * c) << endl;
        cout << "Frequency for one tick: \t"        << freq << " MHZ" << endl; 
        cout << "Estimated Time:\t\t\t"             << (1.0/(freq * 1e6) * pad_nnz / (r * c) * 15) << " s" << endl;
        cout << "Estimated Bandwidth:\t\t"          << (double) freq * r * c / 15 << " MHz" << endl;

        value_rom_t *rom = (value_rom_t*) malloc(sizeof(value_rom_t) * SparseBCSR_depth);
        for (int i = 0; i < SparseBCSR_depth; i++)
            rom[i] = (i >= pad_num_col) ? 0.0 : (value_rom_t) vec[i];

        value_t *value_stream = (value_t *) malloc(sizeof(value_t) * pad_nnz);
        index_t *index_stream = (index_t *) malloc(sizeof(index_t) * pad_nnz);
        index_t *start_stream = (index_t *) malloc(sizeof(index_t) * pad_nnz / (r * c));
        value_t *output_stream = (value_t *) malloc(sizeof(value_t) * pad_nnz / c);

        copy(value.begin(), value.end(), value_stream);
        copy(index.begin(), index.end(), index_stream);
        copy(start.begin(), start.end(), start_stream);

        cout << "Heating ..." << endl;
        SparseBCSR(pad_nnz, index_stream, start_stream, value_stream, output_stream, rom);

        cout << "Running DFE ..." << endl;
        struct timeval t0, t1;
        gettimeofday(&t0, 0);
        SparseBCSR(pad_nnz, index_stream, start_stream, value_stream, output_stream, rom);
        gettimeofday(&t1, 0);

        cout << "Finished" << endl; 
        double duration = (double)(t1.tv_sec-t0.tv_sec)+(double)(t1.tv_usec-t0.tv_usec)/1e6;
        printf("Total time %lf s per element time %lf us GFLOPS %.6f Bandwidth %.6f MHz\n", 
            duration, 
            duration * 1e6 / nnz,
            2.0 * nnz / duration / 1e9,
            1.0 / (duration / nnz) / 1e6);

        int curr_row = 0;
        for (int i = 0; i < pad_nnz/(r*c); i++) {
            if (i && start[i]) {
                for (int j = 0; j < r; j++)
                    res[curr_row+j] = output_stream[(i-1)*r+j];
                curr_row += r;
            }
            if (i == pad_nnz/(r*c)-1)
                for (int j = 0; j < r; j++)
                    res[curr_row+j] = output_stream[i*r+j];
        }
        
        value_t *expected = (value_t *) malloc(sizeof(value_t) * m);
        memset(expected, 0, sizeof(value_t) * m);
        for (int i = 0; i < nnz; i++)
            expected[row[i]] += val[i] * vec[col[i]];

        for (int i = 0; i < m; i++)
            if (abs(res[i]-expected[i])/expected[i] > 1e-4) {
                printf("ERROR: [%6d] %15.6f %15.6f\n", i, res[i], expected[i]);
                exit(1);
            }

        printf("OK!\n");
    }

    return 0;
}
