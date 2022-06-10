
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
int thread_count = 10;
#define N 1000
static int arr[N];
static int tmp[N];
typedef struct to_be_merge_sorted
{
    int l, r;
} to_be_merge_sorted;
typedef struct to_be_merged
{
    int *a, *b, *buf;
    int na, nb;
} to_be_merged;
void *merge(to_be_merged *p)
{
    int *a = p->a, *b = p->b, *buf = p->buf;
    int na = p->na, nb = p->nb;
    for (int ia = 0, ib = 0, i = 0; i < na + nb; i++)
    {
        if (ib >= nb || ia < na && a[ia] <= b[ib])
        {
            buf[i] = a[ia++];
        }
        else
        {
            buf[i] = b[ib++];
        }
    }
    return NULL;
}

int min(int a, int b)
{
    return a <= b ? a : b;
}
void kth_min(int *a, int *b, int ra, int rb, int k, int *pa, int *pb)
{
    int la = 0, lb = 0; //[la,ra)
    while (1)
    {
        if (la == ra)
        {
            lb += k - 1;
            break;
        }
        if (lb == rb)
        {
            la += k - 1;
            break;
        }
        if (k == 1)
        {
            if (min(a[la], b[lb]) == a[la])
            {
                la++;
            }
            else
            {
                lb++;
            }
            break;
        }
        int na = min(la + k / 2 - 1, ra - 1);
        int nb = min(lb + k / 2 - 1, rb - 1);
        int pivot_a = a[na];
        int pivot_b = b[nb];
        if (pivot_a <= pivot_b)
        {
            k -= na - la + 1;
            la = na + 1;
        }
        else
        {
            k -= nb - lb + 1;
            lb = nb + 1;
        }
    }
    *pa = la;
    *pb = lb;
}
void find_median(int *buf, int len, int len_a, int *pa, int *pb)
{
    kth_min(buf, buf + len_a, len_a, len - len_a, (len + 1) / 2, pa, pb);
}

void *merge_sort(to_be_merge_sorted *p)
{
    int l = p->l, r = p->r;
    if (l >= r)
    {
        return NULL;
    }
    pthread_t thread_handles[2];
    to_be_merge_sorted merge_sort_args[2];
    int mid = l + (r - l) / 2;
    // In parallel:
    // merge_sort(a, tmp, l, mid);
    // merge_sort(a, tmp, mid + 1, r);
    merge_sort_args[0] = (to_be_merge_sorted){.l = l, .r = mid};
    merge_sort_args[1] = (to_be_merge_sorted){.l = mid + 1, .r = r};
    for (int i = 0; i < 2; i++)
    {
        pthread_create(thread_handles + i, NULL, (void *)merge_sort, merge_sort_args + i);
    }
    for (int i = 0; i < 2; i++)
    {
        pthread_join(thread_handles[i], NULL);
    }

    int pa, pb;
    find_median(arr + l, r - l + 1, mid - l + 1, &pa, &pb);
    // In parallel:
    to_be_merged merge_args[2];
    merge_args[0] = (to_be_merged){
        .a = arr + l,
        .b = arr + mid + 1,
        .buf = tmp + l,
        .na = pa,
        .nb = pb};
    merge_args[1] = (to_be_merged){
        .a = arr + l + pa,
        .b = arr + mid + 1 + pb,
        .buf = tmp + l + pa + pb,
        .na = mid + 1 - l - pa,
        .nb = r - mid - pb};
    for (int i = 0; i < 2; i++)
    {
        pthread_create(thread_handles + i, NULL, (void *)merge, merge_args + i);
    }
    for (int i = 0; i < 2; i++)
    {
        pthread_join(thread_handles[i], NULL);
    }
    // merge(merge_args);
    // merge(merge_args + 1);
    for (int i = l; i <= r; i++)
    {
        arr[i] = tmp[i];
    }
    return NULL;
}

// gcc pthread.c -o pt -lpthread
int main()
{
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", arr + i);
    }
    to_be_merge_sorted merge_sort_args = (to_be_merge_sorted){
        .l = 0, .r = n - 1};
    merge_sort(&merge_sort_args);
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    return 0;
}