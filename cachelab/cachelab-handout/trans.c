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

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int i, j;
	int gap=8;
	if(M==64)
		gap=4;
        else if(M==61)
		gap=16;
	for(i=0; i<N; i+=gap){
		for(j=0; j<M; j+=gap){
			for(int m=i; m<i+gap && m<M; m++){
				for(int n=j; n<j+gap && n<N ;n++){
					B[n][m]=A[m][n];
				}
			}
		}
	}
}
/*
-----------------------------------------------------------
矩阵转置的分块优化（Blocking）通过提高数据局部性和减少缓存冲突，显著降低缓存不命中次数（cache miss）
-----------------------------------------------------------
矩阵转置的原始实现中，访问模式存在以下问题：
写入目标矩阵B的列访问：矩阵按行存储，B的列访问会导致空间局部性差。每次写入B[j][i]时，相邻列元素在内存中相距较远，无法利用缓存行（cache line）的预取。
缓存容量与冲突问题：当矩阵较大时，A和B的同一行/列可能映射到缓存同一组（set），导致冲突不命中（conflict miss）或容量不命中（capacity miss）。
-----------------------------------------------------------
分块将大矩阵划分为更小的子块（如8x8），逐块处理。每个子块的大小经过设计，使其能完全驻留在缓存中，从而：

提高空间局部性：块内元素的访问集中在连续内存区域，充分利用缓存行。
减少缓存行替换：处理完一个块后，再处理下一个块，避免频繁换入换出。
避免A和B的冲突：块的大小经过计算，确保A和B的块在缓存中映射到不同组，减少冲突。
-----------------------------------------------------------
进一步优化可能：
寄存器暂存（Register Blocking）：在块内进一步分块，用寄存器暂存数据，减少缓存访问次数。
循环展开（Loop Unrolling）：减少分支预测开销，提高指令级并行。
内存对齐：确保块起始地址对齐缓存行，避免额外不命中。
-----------------------------------------------------------
*/

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

