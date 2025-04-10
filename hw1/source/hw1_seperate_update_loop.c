/*
Name: Zachary Heras
ID: 916316192
Homework #: 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int **alloc_board(int P, int Q) {
    int i;
    int *p;
    int **board;

    p = (int *)malloc(P*Q*sizeof(int));
    board = (int **)malloc(P*sizeof(int*));

    if (p == NULL || board == NULL) 
        printf("Error allocating memory\n");

    /* for row major storage */
    for (i = 0; i < P; i++)
        board[i] = &p[i*Q];

    return board;
}

/*
 * This function randomly initializes each non-ghost
 * cell to a value of 0 for dead or 1 for alive
 */
void **init_board(int **board, int mrows, int ncols) {
    int i;
    int j;

    for (i=1; i < mrows-1; i++) {
        for (j=1; j < ncols-1; j++) {
            board[i][j] = (drand48() < 0.5) ? 0 : 1;
        }
    }
}

/*
 * This function updates the ghost cells based on the
 * current non-ghost cells
 */
void **update_ghost_cells(int **board, int mrows, int ncols) {
    int i;
    int j;

    // These lines update the corner ghost cells
    board[0][0] = board[mrows-2][ncols-2];
    board[0][ncols-1] = board[mrows-2][1];
    board[mrows-1][0] = board[1][ncols-2];
    board[mrows-1][ncols-1] = board[1][1];

    // This updates the ghost cells in the leftmost column
    for (i=1; i < mrows-1; i++)
        board[i][0] = board[i][ncols-2];

    // This updates the ghost cells in the rightmost column
    for (i=1; i < mrows-1; i++)
        board[i][ncols-1] = board[i][1];

    // This updates the ghost cells in the topmost row
    for (j=1; j < ncols-1; j++)
        board[0][j] = board[mrows-2][j];

    // This updates the ghost cells in the bottommost row
    for (j=1; j < ncols-1; j++)
        board[mrows-1][j] = board[1][j];
}

/*
 * This function prints the game board
 */
void print_board(int **board, int mrows, int ncols) {
    int i;
    int j;
  
    for (i=0; i < mrows; i++) {
        for (j=0; j < ncols; j++)
              printf("%d ", board[i][j]);
        printf("\n");
    }

    printf("\n");
}

double get_time(void) {
    struct timeval tval;

    gettimeofday(&tval, NULL);

    return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

/*
 * This function determines if a cell will
 * be alive in the next iteration
 */
int is_alive(int **board, int i, int j) {
    int num_neighbors = 0;

    num_neighbors += board[i-1][j-1] == 1 ? 1 : 0;
    num_neighbors += board[i-1][j]   == 1 ? 1 : 0;
    num_neighbors += board[i-1][j+1] == 1 ? 1 : 0;

    num_neighbors += board[i][j-1] == 1 ? 1 : 0;
    num_neighbors += board[i][j+1] == 1 ? 1 : 0;

    num_neighbors += board[i+1][j-1] == 1 ? 1 : 0;
    num_neighbors += board[i+1][j]   == 1 ? 1 : 0;
    num_neighbors += board[i+1][j+1] == 1 ? 1 : 0;

    if (num_neighbors == 3)
        return 1;

    if (!board[i][j])
        return 0;

    if (num_neighbors == 2)
        return 1;

    return 0;

}

/*
 * This function does something
 */
void update_board(int **current_board, int **next_board, int mrows, int ncols) {
    int i;
    int j;

    for (i=1; i < mrows-1; i++) { 
        for (j=1; j < ncols-1; j++) {
            next_board[i][j] = is_alive(current_board, i, j);
        }
    }
}

/*
 * This function does something
 */
int check_board_equality(int **current_board, int **next_board, int mrows, int ncols) {
    int i;
    int j;
    int equal = 1;

    for (i=1; i < mrows-1; i++) { 
        for (j=1; j < ncols-1; j++) {
            next_board[i][j] = is_alive(current_board, i, j);

            if (next_board[i][j] != current_board[i][j]) {
                equal = 0;
            }
        }
    }

    return equal;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <N> <I>\n", argv[0]);
        exit(-1);
    }
    
    int N = atoi(argv[1])+2;
    int max_iterations = atoi(argv[2]);
    int num_threads = atoi(argv[1]);
    
    // Allocate memory for the game board
    int **current_board = alloc_board(N, N);

    // Allocate memory for the temp board
    int **next_board = alloc_board(N, N);

    // Allocate memory for the temp board
    int **temp_board = alloc_board(N, N);

    // seed the random number generator
#ifdef DEBUG_PRINT
    srand48(123456);
#endif

    // initialize the game board
    init_board(current_board, N, N);
    update_ghost_cells(current_board, N, N);

    int iterations = 0;

#ifdef DEBUG_PRINT
    // printf("%d\n", iterations);
    // print_board(current_board, N, N);
    // printf("\n");
#endif

    int terminate = 0;
    double starttime = get_time();

    // Run game loop until termination or max generation count is reached
    while (!terminate && iterations < max_iterations) {
        update_board(current_board, next_board, N, N);
        terminate = check_board_equality(current_board, next_board, N, N);
        update_ghost_cells(next_board, N, N);

        temp_board = current_board;
        current_board = next_board;
        next_board = temp_board;
        
#ifdef DEBUG_PRINT
        // printf("%d\n", iterations);
        // print_board(current_board, N, N);
        // printf("\n");
#endif
        
        iterations ++;
    }

    double endtime = get_time();


    printf("Time taken = %lf seconds\n", endtime-starttime);

    return 0;
}
