#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

	double startSendTime = 0, startReceiveTime = 0, endSendTime = 0, endReceiveTime = 0;
	int world_size, world_rank, n, partialSum = 0, totalSum = 0;
	int* data = NULL;

	FILE* inputElements = fopen("C:\\Users\\rebeca.vacaru\\Documents\\suma_elem\\elem.txt", "r");
	fscanf(inputElements, "%d", &n);

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (world_rank == 0) {
		data = (int*)malloc(n * sizeof(int));
		for (int i = 0; i < n; i++) {
			fscanf(inputElements, "%d", &data[i]);
		}

		startSendTime = MPI_Wtime();
		for (int i = n / world_size, j = 1; j < world_size; i += n / world_size, j++) {
			MPI_Send(&data[i], n / world_size, MPI_INT, j, 0, MPI_COMM_WORLD);
		}
	}
	else {
		data = (int*)malloc(n / world_size * sizeof(int));
		MPI_Recv(data, n / world_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	startReceiveTime = MPI_Wtime();

	for (int i = 0; i < n / world_size; i++) {
		partialSum += data[i];
	}
	endReceiveTime = MPI_Wtime() - startReceiveTime;

	if (world_rank != 0) {
		MPI_Send(&partialSum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	else {
		startReceiveTime = MPI_Wtime();
		for (int i = 0; i < n % world_size; i++) {
			partialSum += data[n - n % world_size + i];
		}
		endReceiveTime += MPI_Wtime() - startReceiveTime;

		totalSum = partialSum;
		for (int i = 1; i < world_size; i++) {
			MPI_Recv(&partialSum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			totalSum += partialSum;
		}
		endSendTime = MPI_Wtime() - startSendTime;


        printf("Final sum is %d",totalSum);
        printf("\nComputed in %f seconds",endSendTime);
        printf("\nIt needed %f seconds to compute the sum by each process",endReceiveTime);
	}
	MPI_Finalize();
}
