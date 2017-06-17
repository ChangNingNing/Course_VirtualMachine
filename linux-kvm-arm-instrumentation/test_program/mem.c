#include <stdio.h>
#include <stdlib.h>

#define TIMES 100
#define SIZE 1024
int main()
{
    int **in;
    int i, j, k;
    
    in = malloc(SIZE*sizeof(int*));
    for (i=0; i<TIMES; i++) {

        for (j=0; j<SIZE; j++) {
            in[j]  = (int*)malloc(SIZE*sizeof(int));

            for (k=0; k<SIZE; k++) 
                in[j][k] = 10;

//			free(in[j]);
        }
    }
//	free(in);

    return 0;
}
