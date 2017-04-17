#include <stdio.h>

int myAdd(int x){
	if(x >= 100000) return x;
	return myAdd(x+1);
}

int main(){
	int x = 0;
	x = myAdd(x);
	printf("x = %d\n", x);
	return 0;
}
