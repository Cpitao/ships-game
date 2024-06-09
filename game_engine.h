#define BOARD_SIZE 10

#define HIT 1
#define MISS -1
#define INVALID_MOVE 0

enum CellStatus {
    EMPTY,
    SHIP,
    HIT,
    MISS
};

typedef struct {
    enum CellStatus grid[BOARD_SIZE][BOARD_SIZE];
} Board;

typedef struct {
    Board player;
    Board server;
} Game;

int shoot(Board *board, int x, int y);
int check_win(Board *board);
Board* mask_board(Board *board);
void board_to_string(Board* board, char* dest);