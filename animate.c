/*
 * Conway's Game of Life (http://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
 * implementation for Nokia Lights Player.
 *
 * It uses a looped board in order to use the 16x8 in a more effective way.
 *
 * Colors for alive and dying states are chosen randomly within each step
 * of the game. Color of the dead state is chosen semi randomly (so that
 * it always stays green).
 *
 * If all cells die during the animation time, another configuration is
 * launched.
 *
 * Author: Antoni Roœciszewski, 2015
 *
 * */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

/* Board dimensions */
#define WIDTH   16
#define HEIGHT  6

/* Internal representation of the board */
typedef struct Board {
    uint32_t data[HEIGHT][WIDTH];
} board_t;

/* Enum used to show colors not only for dead/alive state
 * but also for a cells which died last round
 * (to make the animation more colorful). */
typedef enum {
    STATE_DEAD = 0, STATE_ALIVE = 1, STATE_DYING = 2,
} pixel_state;

const pixel_t PIXEL_BLACK = { 0, 0, 0 };
const pixel_t PIXEL_DEAD  = { 54, 172, 58 };
const pixel_t PIXEL_DYING = { 184, 172, 58 };

/* Game Configuration stored, for the purpose of readability,
   as a 2D int array */

/* Example configuration: glider,
   see: http://en.wikipedia.org/wiki/Glider_%28Conway%27s_Life%29 */
const uint8_t CONFIG_GLIDER[HEIGHT][WIDTH] = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/* A slightly more interesting configuration, consisting of few gliders */
const uint8_t CONFIG_DEFAULT[HEIGHT][WIDTH] = {
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0 }
};

/* Alternating configuration, shown when all cells on the
  first configuration have died.
  Slightly modified loaf and two hives,
  see: http://en.wikipedia.org/wiki/Still_life_(cellular_automaton */
const uint8_t CONFIG_ALTERNATE[HEIGHT][WIDTH] = {
        { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
        { 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0 },
        { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0 },
        { 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/* Util functions */

/* Completely random color */
static pixel_t random_color() {
    pixel_t color = {rand() % 255, rand() % 255, rand() % 255};
    return color;
}

/* Random green color (with fixed green level and
   red, blue levels randomized in a range. */
static pixel_t random_dead_color() {
    pixel_t dead_color = {rand() % 128, 236, rand() % 50};
    return dead_color;
}

/* Fills a single animation frame */
void fill_frame(frame_t* frame, board_t* active_board) {
    pixel_t colors[3] = { random_dead_color(), random_color(), PIXEL_DYING };

    for (int y = 0; y != HEIGHT; ++y)
        for (int x = 0; x != WIDTH; ++x)
            frame->pixel[x + (y * WIDTH)] = colors[active_board->data[y][x]];
}

/* Game functions */

/* Number of alive cells neighbouring with cell at (x,y) position */
static int neighbours(board_t* board, int y, int x) {
    uint32_t neighb_sum = 0;
    int32_t ii, jj;

    for (int32_t i = y - 1; i <= y + 1; i++) {
        for (int32_t j = x - 1; j <= x + 1; j++) {
            ii = i < 0 ? i + HEIGHT : i % HEIGHT;
            jj = j < 0 ? j + WIDTH : j % WIDTH;

            if (!(i == y && j == x) && board->data[ii][jj] == 1)
                neighb_sum++;
        }
    }
    return neighb_sum;
}

/* Fills the board with a given initial configuration */
void fill_board(board_t* board,
                const uint8_t config[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            board->data[i][j] = config[i][j];
        }
    }
}

/* Loads a given config to the game */
void load_config(board_t board[2],
                 const uint8_t config[HEIGHT][WIDTH]) {
    fill_board(&board[1], config);  // Active board
    fill_board(&board[0], config);  // Inactive board
}

/* Single game step, which works by the following rules:
 *   - a cell is born, if it has exactly three neighbours
 *   - a cell dies of loneliness, if it has less than two neighbours
 *   - a cell dies of overcrowding, if it has more than three neighbours
 *   - a cell survives to the next generation, if it does not die of loneliness
 *     or overcrowding
 *
 *     Here we also try to make the animation more colorful by setting
 *     STATE_DYING, which is one step before STATE_DEAD and has another color
 *     assigned.
 */
void step(board_t board[2], uint32_t* active, uint32_t* is_alive) {
    // Asume the game has ended to restart the game
    // in case there aren't any alive cells)
    *is_alive = 0;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            uint32_t n = neighbours(&board[*active], i, j);
            uint32_t previous = board[*active].data[i][j];
            uint32_t state_next = STATE_DEAD;

            if (n == 2)
                state_next = (previous == STATE_DYING) ? STATE_DEAD : previous;
            else if (n == 3)
                state_next = STATE_ALIVE;
            else if (n < 2 || n > 3)
                state_next = (previous == STATE_ALIVE) ?
                                           STATE_DYING : STATE_DEAD;

            board[!(*active)].data[i][j] = state_next;

            // Check whether there are any alive cells
            if (board[!(*active)].data[i][j] == STATE_ALIVE)
                *is_alive = 1;
        }
    }
    // Alternate the active board
    *active = !*active;
}

void animate(const uint32_t timeout) {
    // Whether there are any alive cells
    uint32_t is_alive = 1;

    // Current frame, out frame, frame to be shown when
    // all cells died and a new game is about to begin
    frame_t frame, frame_out, frame_between;
    memset(&frame, 0, sizeof(frame_t));
    frame.delay = 200;
    frame_out.delay = 50;
    frame_between.delay = 400;

    // Fill in the between frame
    for (int y = 0; y != HEIGHT; ++y) {
        pixel_t row_color = random_color(); // Get random color for a given row
        for (int x = 0; x != WIDTH; ++x)
            frame_between.pixel[x + (y * WIDTH)] = row_color;
    }

    uint32_t x, y;
    int32_t fade = 256, fade_dir = 8;
    write_frame(&frame);

    // Current and next configuration of the board
    board_t board[2];
    uint32_t active = 1;

    // Start with an empty board
    memset(board, STATE_DEAD, sizeof(board[0].data[0][0]) * 2 * WIDTH * HEIGHT);
    // Fill the board with a starting configuration
    load_config(board, CONFIG_DEFAULT);

    while (fade > 0) {
        // Let's play the Game of Life
        step(board, &active, &is_alive);

        // Fill all pixels with color
        fill_frame(&frame, &board[active]);

        if (!is_alive) {
            write_frame(&frame_between);
            // Let's play again! (with another config this time)
            load_config(board, CONFIG_ALTERNATE);
        }

        // Fade in & out
        if (total_time() > timeout && fade >= 256) {
            fade = 255;
            fade_dir = -fade_dir;
        }
        if (fade > 0 && fade < 256) {
            for (y = 0; y != HEIGHT; ++y)
                for (x = 0; x != WIDTH; ++x) {
                    frame_out.pixel[x + (WIDTH * y)].r = (frame.pixel[x
                            + (WIDTH * y)].r * fade) / 256;
                    frame_out.pixel[x + (WIDTH * y)].g = (frame.pixel[x
                            + (WIDTH * y)].g * fade) / 256;
                    frame_out.pixel[x + (WIDTH * y)].b = (frame.pixel[x
                            + (WIDTH * y)].b * fade) / 256;
                }
            fade += fade_dir;
        }

        // Output frame
        write_frame((fade < 256) ? &frame_out : &frame);
    }
}
