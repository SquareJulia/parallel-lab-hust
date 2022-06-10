#include <stdio.h>


/***** Begin *****/
#define  N 1000
int a[N],tmp[N];
void merge_sort(int*a, int *tmp,int l,int r){
	if(l>=r){
		return;
	}
	int m=l+(r-l)/2;
	merge_sort(a,tmp,l,m);
	merge_sort(a,tmp,m+1,r);
	for(int i=l,j=m+1,t=l;t<=r;t++){
		if(j>r||i<=m && a[i]<=a[j]){
			tmp[t]=a[i++];
		}else{
			tmp[t]=a[j++];
		}
	}
	for(int i=l;i<=r;i++){
		a[i]=tmp[i];
	}
}
int main()
{
	int n;
	scanf("%d",&n);
	for(int i=0;i<n;i++){
		scanf("%d",a+i);
	}
	merge_sort(a,tmp,0,n-1);
	for(int i=0;i<n;i++){
		printf("%d ",a[i]);
	}
	return 0;
}
/***** End *****/