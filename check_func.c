#include<stdio.h>
#include<math.h>

int is_pow2(int s){
    int count = 0;
    double n = log10(s)/log10(2);
    printf("%f ", n);
	for(double i=0; i< n+1; i++){
        // int count=0;
        if(s == 1)  return 0;
		if(s%2 != 0) return -1;
        printf("%d ", s);
		s/=2;
        count++;
	}
    return count;
}

int main()
{
    printf("%d\n", if_pow2(96));
    return 0;
}