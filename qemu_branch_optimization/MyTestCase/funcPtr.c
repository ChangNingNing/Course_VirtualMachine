#include <stdio.h>
#include <stdlib.h>

int myAdd(int x){
	return x + 1;
}

int mySub(int x){
	return x - 1;
}

int main(){
	int x = 0;
	int (*funcPtr[])(int) = {myAdd, mySub};
	for (int i=0; i<10000000; i++) {
		int (*func)(int) = funcPtr[rand()%2];
		x = func(x);
	}
	printf("x = %d\n", x);
	return 0;
}
