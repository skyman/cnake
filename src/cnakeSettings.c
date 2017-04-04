#include "myLib.h"

u32 SNAKE_BOARD_WIDTH;
u32 SNAKE_BOARD_HEIGHT;
u32 DRAW_SCALE;
u32 SNAKE_INITIAL_LENGTH;

/**
 * setMapSize
 * This sets the global board w/h, drawscale and snake initial length
 * variables according to the mapSize parameter.
 *
 * This was a last minute addition with all four variables originally
 * being concieved as macros, that's why such a sloppy implementation
 * had to be used.
 *
 * @param mapSize Size of map: 0 for small, anything else for large.
 */
void setMapSize(u16 mapSize) {
    if (!mapSize) {
        SNAKE_BOARD_WIDTH = SMALL_BOARD_WIDTH;
        SNAKE_BOARD_HEIGHT = SMALL_BOARD_HEIGHT;
        DRAW_SCALE = SMALL_DRAW_SCALE;
        SNAKE_INITIAL_LENGTH = SMALL_INITIAL_LENGTH;
    } else {
        SNAKE_BOARD_WIDTH = LARGE_BOARD_WIDTH;
        SNAKE_BOARD_HEIGHT = LARGE_BOARD_HEIGHT;
        DRAW_SCALE = LARGE_DRAW_SCALE;
        SNAKE_INITIAL_LENGTH = LARGE_INITIAL_LENGTH;
    }
}
