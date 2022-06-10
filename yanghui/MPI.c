#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#define N 100

void neighbour_sum(int *buf, int len)
{
    for (int i = 0; i < len - 1; i++)
    {
        buf[i] += buf[i + 1];
    }
}

void print(int *arr, int n)

{
    printf("[");
    for (int i = 0; i < n; i++)
    {
        printf("%d", arr[i]);
        if (i < n - 1)
        {
            printf(",");
        }
    }
    printf("]");
}

int main(int argc, char *argv[])
{
\
    int numprocs, myid, source;
    MPI_Status status;
    int buffer[N];
    int task_count[10];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    if (myid != 0)
    { 
        int recv_num = 2, three_count = 0;
        while (1)
        {
            MPI_Recv(buffer, recv_num, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
            if (!buffer[0])
            {
                break;
            }
            neighbour_sum(buffer, recv_num);
            MPI_Send(buffer, recv_num - 1, MPI_INT, 0, 99, MPI_COMM_WORLD);
            if (++three_count == 3)
            {
                three_count = 0;
                recv_num++;
            }
        }
    }
    else
    { 
        int n;
        scanf("%d", &n);
        printf("[");
        for (int i = 1; i < numprocs; i++)
        {
            task_count[i] = 0;
        }
        buffer[0] = 1;
        print(buffer, 1);
        for (int l = 2; l <= n; l++)
        {
			printf(",");
            int *head = buffer;
            for (int i = 1; i < numprocs; i++)
            {
                if (!task_count[i])
                {
                    break;
                }
                MPI_Send(head, task_count[i] + 1, MPI_INT, i, 99, MPI_COMM_WORLD);
                head += task_count[i];
            }
            head = buffer + 1;
            for (int i = 1; i < numprocs; i++)
            {
                if (!task_count[i])
                {
                    break;
                }
                MPI_Recv(head, task_count[i], MPI_INT, i, 99, MPI_COMM_WORLD, &status);
                head += task_count[i];
            }
            *head = 1;
            print(buffer, l);
            task_count[((l + 1) % 3) + 1]++;
        }
        printf("]");
        // send done message
        memset(buffer, 0, sizeof(buffer));
        for (source = 1; source < numprocs; source++)
        {
            MPI_Send(buffer, 1, MPI_INT, source, 99, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
}
/***** End *****/

