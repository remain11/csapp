/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);



void move(int M, int N, int A[N][M], int B[M][N], int ax, int ay, int bx, int by) {
    for (int i = 0; i < 4; i++) {
        int t0 = A[ax + i][ay + 0];
        int t1 = A[ax + i][ay + 1];
        int t2 = A[ax + i][ay + 2];
        int t3 = A[ax + i][ay + 3];
        int t4 = A[ax + i][ay + 4];
        int t5 = A[ax + i][ay + 5];
        int t6 = A[ax + i][ay + 6];
        int t7 = A[ax + i][ay + 7];
        B[bx + i][by + 0] = t0;
        B[bx + i][by + 1] = t1;
        B[bx + i][by + 2] = t2;
        B[bx + i][by + 3] = t3;
        B[bx + i][by + 4] = t4;
        B[bx + i][by + 5] = t5;
        B[bx + i][by + 6] = t6;
        B[bx + i][by + 7] = t7;
    }
    for (int i = 0; i < 4; i++) {
        for (int j = i; j < 4; j++) {
            int t0 = B[bx + i][by + j];
            B[bx + i][by + j] = B[bx + j][by + i];
            B[bx + j][by + i] = t0;
            t0 = B[bx + i][by + j + 4];
            B[bx + i][by + j + 4] = B[bx + j][by + i + 4];
            B[bx + j][by + i + 4] = t0;
        }
    }
}

void swap1(int M, int N, int B[M][N], int x1, int y1, int x2, int y2) {
    for (int i = 0; i < 4; i++) {
        int t0 = B[x1 + i][y1 + 0];
        int t1 = B[x1 + i][y1 + 1];
        int t2 = B[x1 + i][y1 + 2];
        int t3 = B[x1 + i][y1 + 3];
        int t4 = B[x2 + i][y2 + 0];
        int t5 = B[x2 + i][y2 + 1];
        int t6 = B[x2 + i][y2 + 2];
        int t7 = B[x2 + i][y2 + 3];
        B[x2 + i][y2 + 0] = t0;
        B[x2 + i][y2 + 1] = t1;
        B[x2 + i][y2 + 2] = t2;
        B[x2 + i][y2 + 3] = t3;
        B[x1 + i][y1 + 0] = t4;
        B[x1 + i][y1 + 1] = t5;
        B[x1 + i][y1 + 2] = t6;
        B[x1 + i][y1 + 3] = t7;
        
    }
}

void swap2(int M, int N, int B[M][N], int x1, int y1, int x2, int y2) {
    for (int i = 0; i < 4; i++) {
        int t0 = B[x1 + i][y1 + 0];
        int t1 = B[x1 + i][y1 + 1];
        int t2 = B[x1 + i][y1 + 2];
        int t3 = B[x1 + i][y1 + 3];
        int t4 = B[x1 + i][y1 + 4];
        int t5 = B[x1 + i][y1 + 5];
        int t6 = B[x1 + i][y1 + 6];
        int t7 = B[x1 + i][y1 + 7];
        B[x1 + i][y1 + 0] = B[x2 + i][y2 + 0];
        B[x1 + i][y1 + 1] = B[x2 + i][y2 + 1];
        B[x1 + i][y1 + 2] = B[x2 + i][y2 + 2];
        B[x1 + i][y1 + 3] = B[x2 + i][y2 + 3];
        B[x1 + i][y1 + 4] = B[x2 + i][y2 + 4];
        B[x1 + i][y1 + 5] = B[x2 + i][y2 + 5];
        B[x1 + i][y1 + 6] = B[x2 + i][y2 + 6];
        B[x1 + i][y1 + 7] = B[x2 + i][y2 + 7];
        B[x2 + i][y2 + 0] = t0;
        B[x2 + i][y2 + 1] = t1;
        B[x2 + i][y2 + 2] = t2;
        B[x2 + i][y2 + 3] = t3;
        B[x2 + i][y2 + 4] = t4;
        B[x2 + i][y2 + 5] = t5;
        B[x2 + i][y2 + 6] = t6;
        B[x2 + i][y2 + 7] = t7;
    }
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char trans_f1_desc[] = "f1";
void trans_f1(int M, int N, int A[N][M], int B[M][N])
{
    if (N == 32) {
        for (int i = 0; i < N; i+=8) {
            for (int j = 0; j < M; j+=8) {
                for (int l = 0; l < 8; l++) {
                    int t0 = A[i + l][j + 0];
                    int t1 = A[i + l][j + 1];
                    int t2 = A[i + l][j + 2];
                    int t3 = A[i + l][j + 3];
                    int t4 = A[i + l][j + 4];
                    int t5 = A[i + l][j + 5];
                    int t6 = A[i + l][j + 6];
                    int t7 = A[i + l][j + 7];
                    B[j + 0][i + l] = t0;
                    B[j + 1][i + l] = t1;
                    B[j + 2][i + l] = t2;
                    B[j + 3][i + l] = t3;
                    B[j + 4][i + l] = t4;
                    B[j + 5][i + l] = t5;
                    B[j + 6][i + l] = t6;
                    B[j + 7][i + l] = t7;
                }
            }
        }    
    } else if (N == 64) {   
        for (int i = 0; i < N; i += 8) {
            for (int j = 0; j < M; j += 8) {
                // for (int l = 0; l < 4; l++) {
                //     int t0 = A[j + l][i];
                //     int t1 = A[j + l][i + 1];
                //     int t2 = A[j + l][i + 2];
                //     int t3 = A[j + l][i + 3];
                //     int t4 = A[j + l][i + 4];
                //     int t5 = A[j + l][i + 5];
                //     int t6 = A[j + l][i + 6];
                //     int t7 = A[j + l][i + 7];
                //     B[i + l][j] = t0;
                //     B[i + l][j + 1] = t1;
                //     B[i + l][j + 2] = t2;
                //     B[i + l][j + 3] = t3;
                //     B[i + l][j + 4] = t4;
                //     B[i + l][j + 5] = t5;
                //     B[i + l][j + 6] = t6;
                //     B[i + l][j + 7] = t7;
                // }
                // for (int l = 0; l < 4; l++) {
                //     for (int r = l; r < 4; r++) {
                //         int t1 = B[i + l][j + r];
                //         int t2 = B[i + r][j + l];
                //         B[i + l][j + r] = t2;
                //         B[i + r][j + l] = t1;
                //         t1 = B[i + l][j + r + 4];
                //         t2 = B[i + r][j + l + 4];
                //         B[i + l][j + r + 4] = t2;
                //         B[i + r][j + l + 4] = t1;
                //     }
                // }
                // for (int l = 4; l < 8; l++) {
                //     int t0 = A[j + l][i];
                //     int t1 = A[j + l][i + 1];
                //     int t2 = A[j + l][i + 2];
                //     int t3 = A[j + l][i + 3];
                //     int t4 = A[j + l][i + 4];
                //     int t5 = A[j + l][i + 5];
                //     int t6 = A[j + l][i + 6];
                //     int t7 = A[j + l][i + 7];
                //     B[i + l][j] = t0;
                //     B[i + l][j + 1] = t1;
                //     B[i + l][j + 2] = t2;
                //     B[i + l][j + 3] = t3;
                //     B[i + l][j + 4] = t4;
                //     B[i + l][j + 5] = t5;
                //     B[i + l][j + 6] = t6;
                //     B[i + l][j + 7] = t7;
                // }
                // for (int l = 0; l < 4; l++) {
                //     for (int r = l; r < 4; r++) {
                //         int t1 = B[i + l + 4][j + r];
                //         int t2 = B[i + r + 4][j + l];
                //         B[i + l + 4][j + r] = t2;
                //         B[i + r + 4][j + l] = t1;
                //         t1 = B[i + l + 4][j + r + 4];
                //         t2 = B[i + r + 4][j + l + 4];
                //         B[i + l + 4][j + r + 4] = t2;
                //         B[i + r + 4][j + l + 4] = t1;
                //     }
                // }
                // for (int l = 4; l < 8; l++) {
                //     int t0 = B[i + l][j];
                //     int t1 = B[i + l][j + 1];
                //     int t2 = B[i + l][j + 2];
                //     int t3 = B[i + l][j + 3];
                //     int t4 = B[i + l - 4][j + 4];
                //     int t5 = B[i + l - 4][j + 5];
                //     int t6 = B[i + l - 4][j + 6];
                //     int t7 = B[i + l - 4][j + 7];
                //     B[i + l - 4][j + 4] = t0;
                //     B[i + l - 4][j + 5] = t1;
                //     B[i + l - 4][j + 6] = t2;
                //     B[i + l - 4][j + 7] = t3;
                //     B[i + l][j] = t4;
                //     B[i + l][j + 1] = t5;
                //     B[i + l][j + 2] = t6;
                //     B[i + l][j + 3] = t7;
                // }
                move(M, N, A, B, j, i, i, j);
                move(M, N, A, B, j + 4, i, i + 4, j);
                swap1(M, N, B, i + 4, j, i, j + 4);
            }
        }       
    } else {
        for (int i = 0; i < N; i+=8) {
            for (int j = 0; j < M; j+=8) {
                int L = i + 8 < N ? i + 8 : N;  
                for (int l = i; l < L; l++) {
                    if (j == 56) {
                        int t0 = A[l][j];
                        int t1 = A[l][j + 1];
                        int t2 = A[l][j + 2];
                        int t3 = A[l][j + 3];
                        int t4 = A[l][j + 4];
                        int t5 = A[l][j + 5];
                        B[j][l] = t0;
                        B[j + 1][l] = t1;
                        B[j + 2][l] = t2;
                        B[j + 3][l] = t3;
                        B[j + 4][l] = t4;
                        B[j + 5][l] = t5;
                    } else {
                        int t0 = A[l][j];
                        int t1 = A[l][j + 1];
                        int t2 = A[l][j + 2];
                        int t3 = A[l][j + 3];
                        int t4 = A[l][j + 4];
                        int t5 = A[l][j + 5];
                        int t6 = A[l][j + 6];
                        int t7 = A[l][j + 7];
                        B[j][l] = t0;
                        B[j + 1][l] = t1;
                        B[j + 2][l] = t2;
                        B[j + 3][l] = t3;
                        B[j + 4][l] = t4;
                        B[j + 5][l] = t5;
                        B[j + 6][l] = t6;
                        B[j + 7][l] = t7;
                    }
                }
            }
        }    
    }
}

char trans_f2_desc[] = "Transpose submission";
void trans_f2(int M, int N, int A[N][M], int B[M][N])
{
    if (N == 32) {
        for (int i = 0; i < N; i += 8) {
            for (int j = 0; j < M; j += 8) {
                for (int l = 0; l < 8; l++) {
                    int t0 = A[j + l][i + 0];
                    int t1 = A[j + l][i + 1];
                    int t2 = A[j + l][i + 2];
                    int t3 = A[j + l][i + 3];
                    int t4 = A[j + l][i + 4];
                    int t5 = A[j + l][i + 5];
                    int t6 = A[j + l][i + 6];
                    int t7 = A[j + l][i + 7];
                    B[i + l][j + 0] = t0;
                    B[i + l][j + 1] = t1;
                    B[i + l][j + 2] = t2;
                    B[i + l][j + 3] = t3;
                    B[i + l][j + 4] = t4;
                    B[i + l][j + 5] = t5;
                    B[i + l][j + 6] = t6;
                    B[i + l][j + 7] = t7;
                }
                for (int l = 0; l < 8; l++) {
                    for (int r = l; r < 8; r++) {
                        int t = B[i + l][j + r];
                        B[i + l][j + r] = B[i + r][j + l];
                        B[i + r][j + l] = t;
                    }
                }
            }
        }    
    } else if (N == 64) {   
        for (int i = 0; i < N; i += 8) {
            for (int j = 0; j < M; j += 16) {
                move(M, N, A, B, j + 12, i, i + 4, j);
                move(M, N, A, B, j + 8, i, i, j + 8);
                swap1(M, N, B, i + 4, j, i, j + 12);
                move(M, N, A, B, j, i, i, j);
                move(M, N, A, B, j + 4, i, i + 4, j + 8);
                swap1(M, N, B, i, j + 4, i + 4, j + 8);
                swap2(M, N, B, i + 4, j, i + 4, j + 8);
            }
        }  
    } else {
        for (int i = 0; i < N; i+=8) {
            for (int j = 0; j < M; j+=8) {
                int L = j + 8 < M ? j + 8 : M;  
                for (int l = j; l < L; l++) {
                    if (i == 64) {
                        int t0 = A[i + 0][l];
                        int t1 = A[i + 1][l];
                        int t2 = A[i + 2][l];
                        B[l][i + 0] = t0;
                        B[l][i + 1] = t1;
                        B[l][i + 2] = t2;
                    } else {
                        int t0 = A[i + 0][l];
                        int t1 = A[i + 1][l];
                        int t2 = A[i + 2][l];
                        int t3 = A[i + 3][l];
                        int t4 = A[i + 4][l];
                        int t5 = A[i + 5][l];
                        int t6 = A[i + 6][l];
                        int t7 = A[i + 7][l];
                        B[l][i + 0] = t0;
                        B[l][i + 1] = t1;
                        B[l][i + 2] = t2;
                        B[l][i + 3] = t3;
                        B[l][i + 4] = t4;
                        B[l][i + 5] = t5;
                        B[l][i + 6] = t6;
                        B[l][i + 7] = t7;
                    }
                }
            }
        }    
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(trans_f1, trans_f1_desc); 

    registerTransFunction(trans_f2, trans_f2_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

