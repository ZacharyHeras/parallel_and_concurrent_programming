/*
Name: Zachary Heras
ID: 916316192
Homework #: 3
To Compile: mpicc -o hw3 -DDEBUG_PRINT hw3.c
To Run: mpiexec -n <T> ./hw3 <N> <I>
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <string.h>

/*
 * This function allocates memory for the board array
 */
int **alloc_board(int P, int Q) {
    int i;
    int *p;
    int **game_board;

    p = (int *)malloc(P*Q*sizeof(int));
    game_board = (int **)malloc(P*sizeof(int*));

    if (p == NULL || game_board == NULL) {
        printf("Error allocating memory\n");
        MPI_Finalize();
        exit(-1);
    }

    /* for row major storage */
    for (i = 0; i < P; i++)
        game_board[i] = &p[i*Q];

    return game_board;
}

/*
 * This function randomly initializes each non-ghost
 * cell to a value of 0 for dead or 1 for alive
 */
void init_board(int **game_board, int mrows, int ncols) {
    int i;
    int j;

    for (i=1; i < mrows-1; i++) {
        for (j=1; j < ncols-1; j++) {
            game_board[i][j] = (drand48() < 0.5) ? 0 : 1;
        }
    }
}

/*
 * This function updates the ghost cells based on the
 * current non-ghost cells
 */
void update_ghost_columns(int **game_board, int mrows, int ncols) {
    int i;
    int j;

    for (i=0; i < mrows; i++) {
        game_board[i][0] = game_board[i][ncols-2];
        game_board[i][ncols-1] = game_board[i][1];
    }
}

double get_time(void) {
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return(double)tval.tv_sec + (double)tval.tv_usec/1000000.0;
}

/*
 * This function determines if a cell will
 * be alive in the next iteration
 */
int is_alive(int **game_board, int i, int j) {
    int num_neighbors = 0;

    num_neighbors += game_board[i-1][j-1] == 1 ? 1 : 0;
    num_neighbors += game_board[i-1][j]   == 1 ? 1 : 0;
    num_neighbors += game_board[i-1][j+1] == 1 ? 1 : 0;

    num_neighbors += game_board[i][j-1] == 1 ? 1 : 0;
    num_neighbors += game_board[i][j+1] == 1 ? 1 : 0;

    num_neighbors += game_board[i+1][j-1] == 1 ? 1 : 0;
    num_neighbors += game_board[i+1][j]   == 1 ? 1 : 0;
    num_neighbors += game_board[i+1][j+1] == 1 ? 1 : 0;

    if (num_neighbors == 3)
        return 1;

    if (!game_board[i][j])
        return 0;

    if (num_neighbors == 2)
        return 1;

    return 0;

}

/*
 * This function updates each cell's status and sets the updated parameter to 1
 * if the status of any cell changed
 */
void update_board(int **game_board, int **temp_board, int mrows, int ncols, int *updated) {
    int i;
    int j;

    *updated = 0;

    for (i=1; i < mrows-1; i++) { 
        for (j=1; j < ncols-1; j++) {
            temp_board[i][j] = is_alive(game_board, i, j);
            
            if (game_board[i][j] != temp_board[i][j]) {
                *updated = 1;
            }
        }
    }

    for (i=1; i < mrows-1; i++) { 
        for (j=1; j < ncols-1; j++) {
            game_board[i][j] = temp_board[i][j];
        }
    }
}

int main(int argc, char **argv) {
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    if (argc != 3) {
        if (rank == 0)
            printf("Usage: %s <N> <I>\n", argv[0]);
        MPI_Finalize();
        exit(-1);
    }

    // this prevents process collision issues
    int N = atoi(argv[1]) + 2;
    if (2 * size > N - 2) {
        printf("N must be at least twice the number of processes\n");
        MPI_Finalize();
        exit(-1);
    }
   
    int max_iterations = atoi(argv[2]);

    // calculate how many rows should be handed off to each process
    int rows_per_proc = (N - 2) / size;
    int remainder = (N - 2) % size;

    // make sure to handle non-zero remainder by adding rows to some processes
    int M = rows_per_proc + (rank < remainder ? 1 : 0) + 2;
    
    // Allocate memory for the game board
    int **game_board = alloc_board(M, N);

    // Allocate memory for the temp board
    int **temp_board = alloc_board(M, N);

    // initialize the game board
    srand48(rank * get_time());
    init_board(game_board, M, N);

    int prev = (rank == 0) ? size - 1 : rank - 1;
    int next = (rank == size - 1) ? 0 : rank + 1;

    update_ghost_columns(game_board, M, N);

    MPI_Sendrecv(game_board[M-2], N, MPI_INT, next, 0, game_board[0], N, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Sendrecv(game_board[1], N, MPI_INT, prev, 1, game_board[M-1], N, MPI_INT, next, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int iterations = 0;

    int terminate = 0;
    int updated = 0;
    double starttime = get_time();

    // Run game loop until termination or max generation count is reached
    while (!terminate && iterations < max_iterations) {
        update_board(game_board, temp_board, M, N, &updated);

        MPI_Sendrecv(game_board[M-2], N, MPI_INT, next, 0, game_board[0], N, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Sendrecv(game_board[1], N, MPI_INT, prev, 1, game_board[M-1], N, MPI_INT, next, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        update_ghost_columns(game_board, M, N);

        int global_updated;
        MPI_Allreduce(&updated, &global_updated, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);

        terminate = !global_updated;

        iterations ++;

        // handles printing the board by gathering onto rank 0
        // this replaces print board function that was present in other
        // versions of program because it is no longer relevant do to
        // sync issues betwee processes.
#ifdef DEBUG_PRINT
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            printf("%d\n", iterations);
        }

        int local_rows = M - 2;
        int *sendbuf = malloc(local_rows * N * sizeof(int));
        for (int i = 0; i < local_rows; i++) {
            for (int j = 0; j < N; j++) {
                sendbuf[i*N + j] = game_board[i+1][j];
            }
        }

        int *recvbuf = NULL;

        // number of items to receive per process
        int *recvcounts = NULL;
        int *position = NULL;

        if (rank == 0) {
            // kind of like row major storage for simplicity
            recvbuf = malloc((N - 2) * N * sizeof(int));
            recvcounts = malloc(size * sizeof(int));
            position = malloc(size * sizeof(int));

            int offset = 0;
            for (int i = 0; i < size; i++) {
                int rpp = (N - 2) / size + (i < (N - 2) % size ? 1 : 0);
                recvcounts[i] = rpp * N;
                position[i] = offset;
                offset += recvcounts[i];
            }
        }

        MPI_Gatherv(sendbuf, local_rows * N, MPI_INT, recvbuf, recvcounts, position, MPI_INT, 0, MPI_COMM_WORLD);

        int *top_ghost = malloc(N * sizeof(int));
        int *bottom_ghost = malloc(N * sizeof(int));

        if (rank == 0) {
            memcpy(top_ghost, game_board[0], N * sizeof(int));
        }

        if (rank == size - 1) {
            MPI_Send(game_board[M - 1], N, MPI_INT, 0, 4272001, MPI_COMM_WORLD);
        }

        if (rank == 0 && size > 1) {
            MPI_Recv(bottom_ghost, N, MPI_INT, size - 1, 4272001, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (rank == 0) {
            // Print top ghost row
            for (int j = 0; j < N; j++)
                printf("%d ", top_ghost[j]);
            printf("\n");

            // Print inner rows
            int total_rows = 0;
            for (int i = 0; i < size; i++)
                total_rows += recvcounts[i] / N;

            for (int i = 0; i < total_rows; i++) {
                for (int j = 0; j < N; j++)
                    printf("%d ", recvbuf[i*N + j]);
                printf("\n");
            }

            // Print bottom ghost row
            for (int j = 0; j < N; j++)
                printf("%d ", bottom_ghost[j]);
            printf("\n\n");
        }

        free(sendbuf);
        free(top_ghost);
        free(bottom_ghost);

        if (rank == 0) {
            free(recvbuf);
            free(recvcounts);
            free(position);
        }

        MPI_Barrier(MPI_COMM_WORLD);
#endif
        
    }

    double endtime = get_time();

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Time taken = %lf seconds\n", endtime - starttime);
        printf("Num iterations = %d\n", iterations);
    }

    MPI_Finalize();
}
