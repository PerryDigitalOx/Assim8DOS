#define EMPTY     0   // Value which indicates an empty board square
#define PLAYERX   1   // Value which indicates a piece owned by player "O"
#define PLAYERO   2   // Value which indicates a piece owned by player "X"

#define BSIZE   8     // Board size

#define ALPHA_BETA_PRUNING     // Yes, do alpha-beta pruning for speed

#define OTHER(player)     (3-(player))
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define ON_BOARD(x, y) ((x) >= 0 && (x) < BSIZE && (y) >= 0 && (y) < BSIZE)
#define COPY_BOARD(dest, source) \
	  (memcpy((char *)dest, (char *)source, sizeof(board_t)))
#define SAME_BOARD(board1, board2) \
     (memcmp((char *)board1, (char *)board2, sizeof(board_t)) == 0)

typedef int board_t[BSIZE][BSIZE];

int far play(board_t board, int lev, int player);
int do_move(board_t, int from_x, int from_y, int to_x, int to_y);
int eval(board_t, int player);

static int far aix[16] = { 1, 1,-1,-1, 0, 1, 0,-1, 0, 2, 2, 2, 0,-2,-2,-2};
static int far aiy[16] = { 1,-1,-1, 1, 1, 0,-1, 0, 2, 2, 0,-2,-2,-2, 0, 2};
static int far bix[16] = { 0, 1, 1, 1, 0,-1,-1,-1, 0, 2, 2, 2, 0,-2,-2,-2};
static int far biy[16] = {-1,-1, 0, 1, 1, 1, 0,-1,-2,-2, 0, 2, 2, 2, 0,-2};


static board_t best_play[3+1][3+1];
static int     best_eval[3+1];
static int     best_from_x[4], best_from_y[4];
static int     best_to_x[4], best_to_y[4];

char far brain;
int far hintselected;
int depth;
unsigned int far boardx[BSIZE];
unsigned int far boardy[BSIZE];
int aix[];
int aiy[];
