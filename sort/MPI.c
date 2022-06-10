
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#define N 1000

void merge(int *a, int *b, int na, int nb, int *dest)
{
    for (int ia = 0, ib = 0, i = 0; i < na + nb; i++)
    {
        if (ib >= nb || ia < na && a[ia] <= b[ib])
        {
            dest[i] = a[ia++];
        }
        else
        {
            dest[i] = b[ib++];
        }
    }
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
void send_array_to_merge(int channel, int *a, int *b, int na, int nb)
{
    MPI_Send(&na, 1, MPI_INT, channel, 99, MPI_COMM_WORLD);
    MPI_Send(&nb, 1, MPI_INT, channel, 99, MPI_COMM_WORLD);
    MPI_Send(a, na, MPI_INT, channel, 99, MPI_COMM_WORLD);
    MPI_Send(b, nb, MPI_INT, channel, 99, MPI_COMM_WORLD);
}
void merge_sort(int *a, int *tmp, int l, int r)
{
    if (l >= r)
    {
        return;
    }
    int mid = l + (r - l) / 2;
    merge_sort(a, tmp, l, mid);
    merge_sort(a, tmp, mid + 1, r);
    int pa, pb;
    find_median(a + l, r - l + 1, mid - l + 1, &pa, &pb);
    send_array_to_merge(1, a + l, a + mid + 1, pa, pb);
    send_array_to_merge(2, a + l + pa, a + mid + 1 + pb, mid + 1 - l - pa, r - mid - pb);
    MPI_Status status;
    MPI_Recv(tmp + l, pa + pb, MPI_INT, 1, 99, MPI_COMM_WORLD, &status);
    MPI_Recv(tmp + l + pa + pb, r + 1 - l - pa - pb, MPI_INT, 2, 99, MPI_COMM_WORLD, &status);
    for (int i = l; i <= r; i++)
    {
        a[i] = tmp[i];
    }
}
void notify_finished(int procs)
{
    int buf[2];
    buf[0] = buf[1] = 0;
    for (int proc = 1; proc < procs; proc++)
    {
        MPI_Send(buf, 1, MPI_INT, proc, 99, MPI_COMM_WORLD);
        MPI_Send(buf+1, 1, MPI_INT, proc, 99, MPI_COMM_WORLD);
    }
}
int main(int argc, char *argv[])
{
    int numprocs, myid, source;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    int buf[N];
    int na, nb;
    if (myid != 0)
    {
        int a[N], b[N];
        while (1)
        {
            MPI_Recv(&na, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
            MPI_Recv(&nb, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
            if (!na && !nb)
            {
                break;
            }
            MPI_Recv(a, na, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
            MPI_Recv(b, nb, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
            merge(a, b, na, nb, buf);
            MPI_Send(buf, na + nb, MPI_INT, 0, 99, MPI_COMM_WORLD);
        }
    }
    else
    {
        int a[N];
        int n;
        scanf("%d", &n);
        for (int i = 0; i < n; i++)
        {
            scanf("%d", a + i);
        }
        merge_sort(a, buf, 0, n - 1);
        notify_finished(numprocs);
        for (int i = 0; i < n; i++)
        {
            printf("%d ", a[i]);
        }
    }
    MPI_Finalize();
}
/***** End *****/

