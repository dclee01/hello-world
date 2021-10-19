#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const int directions[][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

int **board = NULL;
int BOARD_SIZE;

/**
 * Create a game board.
 * @param board size: the size of the board to create
 * @return NULL if board size is invalid, return the board if succeed.
 */
int **create_board(int board_size);

/**
 * Free all memories of the game board.
 * @param board: the board to destroy
 * @param board_size: the size of the board to destroy
 */
void destroy_board(int **board, int board_size);

/**
 * Print setting up the game and initialize the game.
 * @param new_board_size: new board size to initialize.
 * @return 1 if succeed, 0 if not.
 */
int initialization(int new_board_size);

/**
 * Display the game state.
 * @param output: where to display
 */
void display_game_state(FILE *output);

/**
 * Print ending the game, and destroy the board.
 */
void tear_down();

/**
 * Check if the position is in the board.
 * @param row: row of the position
 * @param col: column of the position
 * @return 1 if the position is in the board, and 0 if not.
 */
int in_board(int row, int col);

/**
 * Decide whether moving a tile at the given position is possible.
 * @param row: row of the tile
 * @param col: column of the tile
 * @return -1 if not possible to move the tile, return the direction (0-3) it
 * can move to if it is possible.
 */
int is_move_valid(int row, int col);

/**
 * If moving a given tile is valid, perform the move by updating the game state.
 * If invalid, do nothing.
 * @param tile: the tile to move
 * @return 1 if it is valid and game state updated, 0 if invalid.
 */
int move_title(int tile);

/**
 * Check if the player has won the game.
 * @return 1 if the player has won, 0 if not.
 */
int won_the_game();

/**
 * Save the game state to a file.
 * @param filename: filename to save the game state
 * @return 1 if succeed, 0 if not.
 */
int save_the_game(const char *filename);

/**
 * Load the game state from a file.
 * @param filename: filename to load the game state
 * @return 1 if succeed, 0 if not.
 */
int load_the_game(const char *filename);

//*==========================================================================*/

int main() {
    srand((unsigned)time(NULL));

    // Initialize the game
    initialization(4);

    char ch;
    int tile;
    int board_size;
    char filename[FILENAME_MAX];
    do {
        printf("Menu: [p]rint, [m]ove, [q]uit [n]ew, [s]ave, [l]oad? ");

        // Get user choice
        scanf(" %c", &ch);

        switch (ch) {
        case 'p':
        case 'P': // Print game state
            display_game_state(stdout);
            break;
        case 'm':
        case 'M': // Move tile
            // Which pcie you want to move
            printf("Whice tile to move? ");
            scanf("%d", &tile);
            if (move_title(tile)) {
                // If not error, move success
                printf("Moving %d.\n", tile);
                if (won_the_game()) {
                    printf("***Congratulations! you win.***\n");

                    // Start a new game
                    initialization(BOARD_SIZE);
                }
            } else
                // If error, move invaild
                printf("Invalid Move.\n");
            break;
        case 'q':
        case 'Q': // Quit
            // End the game
            break;
        case 'n':
        case 'N': // New game board
            printf("What board size to play? ");
            scanf("%d", &board_size);
            initialization(board_size);
            break;
        case 's':
        case 'S': // Save the game state to a file
            printf("Filename to save: ");
            scanf("%s", filename);
            if (save_the_game(filename)) {
                printf("Saved.\n");
            } else {
                printf("Save error.\n");
            }
            break;
        case 'l':
        case 'L': // Load the game from file
            printf("Filename to load: ");
            scanf("%s", filename);
            if (load_the_game(filename)) {
                printf("Loaded.\n");
            } else {
                printf("Load error.\n");
            }
            break;
        default:
            printf("Error choice!\n");
        }
    } while (ch != 'Q' && ch != 'q');

    // End the game
    tear_down();

    return 0;
}

/**
 * Create a game board.
 * @param board size: the size of the board to create
 * @return NULL if board size is invalid, return the board if succeed.
 */
int **create_board(int board_size) {
    // Check if the board size is valid
    if (!(board_size >= 2 && board_size <= 10)) {
        printf("Invalid board size\n");
        return NULL;
    }

    // Create the board
    int **board = (int **)malloc(sizeof(int *) * board_size);
    for (int i = 0; i < board_size; i++) {
        board[i] = (int *)malloc(sizeof(int) * board_size);
        for (int j = 0; j < board_size; j++) {
            board[i][j] = i * board_size + j;
        }
    }
    return board;
}

/**
 * Free all memories of the game board.
 */
void destroy_board(int **board, int board_size) {
    if (board) {
        for (int i = 0; i < board_size; i++) {
            free(board[i]);
        }
        free(board);
    }
}

/**
 * Print setting up the game and initialize the game.
 */
int initialization(int new_board_size) {
    printf("Setting up the game.\n");

    // Allocate the game board
    int **new_board = create_board(new_board_size);
    if (new_board == NULL) {
        return 0;
    }

    // Destroy the game board and replace it to new_board
    destroy_board(board, BOARD_SIZE);
    board = new_board;
    BOARD_SIZE = new_board_size;

    // Randomizing
    int size = new_board_size * new_board_size;
    for (int i = size - 1; i >= 0; i--) {
        // Treat the board as a 1D array arr
        // Swap arr[i] and arr[rand() % (i + 1)]
        int random_val = rand();
        int r1 = i / new_board_size;
        int c1 = i % new_board_size;
        int r2 = (random_val % (i + 1)) / new_board_size;
        int c2 = (random_val % (i + 1)) % new_board_size;
        int tmp = board[r1][c1];
        board[r1][c1] = board[r2][c2];
        board[r2][c2] = tmp;
    }

    return 1;
}

/**
 * Display the game state.
 */
void display_game_state(FILE *output) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != 0 || output != stdout)
                fprintf(output, "%3d", board[i][j]);
            else {
                fprintf(output, "   ");
            }
        }
        fprintf(output, "\n");
    }
}

int won_the_game() {
    int num = 1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != 0 && board[i][j] != num++) {
                return 0;
            }
        }
    }
    return 1;
}

/**
 * Print ending the game.
 */
void tear_down() {
    printf("Ending the game.\n");
    destroy_board(board, BOARD_SIZE);
}

/**
 * Check if the position is in the board.
 * @param row: row of the position
 * @param col: column of the position
 * @return 1 if the position is in the board, and 0 if not.
 */
int in_board(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

/**
 * Decide whether moving a tile at the given position is possible.
 * @param row: row of the tile
 * @param col: column of the tile
 * @return -1 if not possible to move the tile, return the direction (0-3) it
 * can move to if it is possible.
 */
int is_move_valid(int row, int col) {
    int r, c;
    for (int k = 0; k < 4; k++) {
        r = row + directions[k][0];
        c = col + directions[k][1];
        if (in_board(r, c) && board[r][c] == 0) {
            return k;
        }
    }
    return -1;
}

/**
 * If moving a given tile is valid, perform the move by updating the game state.
 * If invalid, do nothing.
 * @param tile: the tile to move
 * @return 1 if it is valid and game state updated, 0 if invalid.
 */
int move_title(int tile) {
    // Find the tile in the board
    int row = -1, col = -1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == tile) {
                row = i;
                col = j;
                break;
            }
        }
    }
    // If not find the tile, error
    if (row == -1) {
        return 0;
    }

    int dir;
    if ((dir = is_move_valid(row, col)) != -1) { // check if the move is valid
        // If valid, perform the move and return 1
        int zero_row = row + directions[dir][0];
        int zero_col = col + directions[dir][1];
        board[zero_row][zero_col] = board[row][col];
        board[row][col] = 0;
        return 1;
    }
    // Not valid move, error
    return 0;
}

/**
 * Save the game state to a file.
 * @param filename: filename to save the game state
 * @return 1 if succeed, 0 if not.
 */
int save_the_game(const char *filename) {
    if (board == NULL) {
        return 0;
    }

    // Open the file
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen");
        return 0;
    }

    // Write the board size
    fprintf(fp, "%d\n", BOARD_SIZE);

    // Write the board
    display_game_state(fp);

    // Close file
    fclose(fp);

    // Return successful
    return 1;
}

/**
 * Load the game state from a file.
 * @param filename: filename to load the game state
 * @return 1 if succeed, 0 if not.
 */
int load_the_game(const char *filename) {
    // Open the file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        return 0;
    }

    // Load the board size
    int new_board_size;
    if (fscanf(fp, "%d", &new_board_size) != 1) {
        printf("Invalid game file\n");
        fclose(fp);
        return 0;
    }

    // Create a new board with that size
    int **new_board;
    if ((new_board = create_board(new_board_size)) == NULL) {
        printf("Invalid game file\n");
        fclose(fp);
        return 0;
    }

    // Load the board
    for (int i = 0; i < new_board_size; i++) {
        for (int j = 0; j < new_board_size; j++) {
            if (fscanf(fp, "%d", &new_board[i][j]) != 1) {
                fclose(fp);
                destroy_board(new_board, new_board_size);
                return 0;
            }
        }
    }

    // Close file
    fclose(fp);

    // Destroy the game board and replace it to new_board
    destroy_board(board, BOARD_SIZE);
    board = new_board;
    BOARD_SIZE = new_board_size;

    // Return successful
    return 1;
}
