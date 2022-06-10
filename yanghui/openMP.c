#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

#define N 100
int a[N],tmp[N];
void print(int *arr,int n){
	printf("[");
	for(int i=0;i<n;i++){
		printf("%d",arr[i]);
		if(i<n-1){
			printf(",");
		}
	}
	printf("]");
}

int main()
{
	int n;
	scanf("%d",&n);
	a[0]=1;
	printf("[");
	for(int l=1;l<=n;l++){
		print(a,l);
		a[l]=1;
		#pragma omp parallel for  
		for(int i=1;i<l;i++){
			tmp[i]=a[i-1]+a[i];
		}
		#pragma omp parallel for  
		for(int i=1;i<l;i++){
			a[i]=tmp[i];
		}
		if(l<n){
			printf(",");
		}
	}
	printf("]");
	return 0;
}
/***** End *****/