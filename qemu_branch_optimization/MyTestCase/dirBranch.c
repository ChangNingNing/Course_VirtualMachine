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
	for (int i=0; i<10000000; i++) {
		if(rand()&1 == 1)
			x = myAdd(x);
		else
			x = mySub(x);
	}
	printf("x = %d\n", x);
	return 0;
}
