#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N (1<<20)
int A[N];

int compare(const void *a, const void *b){
	return *(int *)a - *(int *)b;
}
int main(){
	srand(time(NULL));
	for (int i=0; i<N; i++)
		A[i] = rand();
	qsort(A, N, sizeof(int), compare);
	printf("last = %d\n", A[N-1]);
	return 0;
}
