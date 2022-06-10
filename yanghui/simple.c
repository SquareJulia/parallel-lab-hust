#include <stdio.h>
#include <malloc.h>


/***** Begin *****/
void print(int *arr,int n){
	printf("[");
	for(int i=0;;i++){
		printf("%d",arr[i]);
		if(i==n-1){
			break;
		}
		printf(",");
	}
	printf("]");
}

int main()
{
	int n;
	scanf("%d",&n);
	int *a=(int *)malloc(n*(sizeof n));
	a[0]=1;
	printf("[");
	for(int l=1;;l++){
		print(a,l);
		if(l==n){
			break;
		}
		a[l]=1;
		for(int i=l-1;i>0;i--){
			a[i]+=a[i-1];
		}
		printf(",");
	}
	printf("]");
	return 0;
}
/***** End *****/