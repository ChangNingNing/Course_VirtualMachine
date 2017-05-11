#include <stdio.h>
#define ULL unsigned long long int

ULL myRecur(ULL x){
	if (x <= 1) return x;
	return myRecur(x-1) + myRecur(x-2);
}

int main(){
	ULL x = 35;
	x = myRecur(x);
	printf("x = %lld\n", x);
	return 0;
}
