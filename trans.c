/* 
 * trans.c - Matrix transpose B = A^T
 *
 * First Name: Santosh Nikhil Kumar 
 * Last Name:  Adireddy
 * UserId: sadiredd
 * EmailId: sadiredd@andrew.cmu.edu
 
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    int blockrow, blockcolumn, i = 0, j = 0;
    int blocksize;
    int temp, r, l = 0; //r- stores the diagonal index when diagonal element is accessed; l- stores the status of whether the diagonal element is accessed or not
    /*
     * I have used the access pattern in such a way that "all the elements loaded into the cache are utilized compeletely before loading the next set of elements" to maximize the hits.
     * Hence, the concept of blocking is applied
     */
    // 32x32 Matrix transpose
    if (M == 32 && N == 32) {
	blocksize = 8;		//Blocking with 8x8 matrices since the blockbits=5 => 32 bytes. 8 Integers require 32 bytes. Hence 8x8.
	//These 8x8 blocks are accessed horizontally from left to right(4 blocks) in set 1. Then, the next set of 4 blocks in the second set 
	for (blockrow = 0; blockrow < N; blockrow += blocksize) {
	    for (blockcolumn = 0; blockcolumn < M;
		 blockcolumn += blocksize) {
		for (i = blockrow; i < blockrow + blocksize; i++) {
		    for (j = blockcolumn; j < blockcolumn + blocksize; j++) {

			if (i != j)
			    B[j][i] = A[i][j];	//When not a diagonal element, perform the normal transpose
			else {
			    // Temporary variables are used so that 'B' matrix is not accessed, thereby the 'A' value is not replaced in the cache.
			    temp = A[i][j];
			    r = i;  //Diagonal index saved
			    l = 1;	//Indicating that diagonal element is accessed
			}
		    }
		    //Putting the diagonal elements in the B matrix after every row access in a block
		    if (l == 1)
			B[r][r] = temp;
		    l = 0;
		}

	    }
	}

    }
    // 67x61 Matrix transpose
    else if (M == 61 && N == 67) {
	//Blocking using 20x20 matrix. This allows for a symmetric block
	blocksize = 20;
	//Similar access as for 32x32 matrix
	for (blockrow = 0; blockrow < N; blockrow += blocksize) {
	    for (blockcolumn = 0; blockcolumn < M;
		 blockcolumn += blocksize) {
		for (i = blockrow; i < blockrow + blocksize; i++) {
		    for (j = blockcolumn; j < blockcolumn + blocksize; j++) {
			if (i > 66 || j > 60)	// The last column(M=61) and the last row(N=67) are skipped. They are by default transposed.
			    continue;
			else
			    B[j][i] = A[i][j];
		    }
		}
	    }
	}
    }
    // 64x64 Matrix transpose
    else if (M == 64 && N == 64) {
	//SIMILAR to 32x32 matrix---> All the variables loaded into the cache utilized compeletely
	blocksize = 4;
	for (blockcolumn = 0; blockcolumn < M; blockcolumn += blocksize) {
	    for (blockrow = 0; blockrow < N; blockrow += blocksize) {
		for (i = blockrow; i < blockrow + blocksize; i++) {
		    for (j = blockcolumn; j < blockcolumn + blocksize; j++) {

			if (i != j)
			    B[j][i] = A[i][j];
			else {
			    temp = A[i][j];
			    r = i;
			    l = 1;	//Indicating that diagonal element is accessed
			}
		    }
		    //Putting the diagonal elements in the B matrix after every row access in a block
		    if (l == 1)
			B[r][r] = temp;
		    l = 0;
		}

	    }
	}

    }

    ENSURES(is_transpose(M, N, A, B));
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

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
	for (j = 0; j < M; j++) {
	    tmp = A[i][j];
	    B[j][i] = tmp;
	}
    }

    ENSURES(is_transpose(M, N, A, B));
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
    registerTransFunction(transpose_submit, transpose_submit_desc);

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
