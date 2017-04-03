#include "myLib.h"
#include <stdlib.h>

// Game Logic
Game createGame() {
    // Initialize the Snake
    Snake s = createSnake();

    // Initialize the Game
    Game g;
    g.snake = s;
    g.numFoods = 0;
    g.score = 0;

    return g;
}

void processGame(Game *g, u32 keysPressed) {
	// Start by incrementing game cycle
	g->currentCycle++;

    // Remove expired foods
    u16 newFoodCount = 0;
    for (u16 i = 0; i < g->numFoods; i++) {
        Food f = g->foods[i];
        if (g->currentCycle < f.deleteOnCycle) {
            g->foods[newFoodCount++] = f;
        }
    }

    g->numFoods = newFoodCount;

    Snake *s = &(g->snake);

    // Move the snake
    u16 speed = SNAKESPEED(g->score);

	switch (s->facing) {
	    case DOWN:
	        s->head.y += speed;
			break;
	    case RIGHT:
	        s->head.x += speed;
			break;
	    case UP:
	        s->head.y = s->head.y < speed ? speed : s->head.y;
	        s->head.y -= speed;
			break;
	    case LEFT:
	        s->head.x = s->head.x < speed ? speed : s->head.x;
	        s->head.x -= speed;
			break;
	}

	// If the snake needs to be grown, do that:
	s->length += speed;
	if (s->length > s->growToLength) s->length = s->growToLength;

    // If either value is above MAX fix it
    s->head.x = s->head.x >= SNAKE_BOARD_WIDTH - 1 ? SNAKE_BOARD_WIDTH - 1 : s->head.x;
    s->head.y = s->head.y >= SNAKE_BOARD_HEIGHT - 1 ? SNAKE_BOARD_HEIGHT - 1 : s->head.y;

    // Now check if we have collided into anything
    s->dead = checkSelfCollision(*s) || checkWallCollision(*s);

    // Eat any food if necessary
    // TODO: why is food not being eaten?
    for (u16 i = 0; i < g->numFoods; i++) {
        Food *f = &(g->foods[i]);
        if (checkFoodCollision(*s, *f)) {
            eatFood(f, g);
        }
    }

    // Generate any new foods if we must
    unsigned short numFoods = g->numFoods;
    while (numFoods < FOODCOUNT(g->score)) {
        g->foods[numFoods++] = createRandomFood(g);
    }
    g->numFoods = numFoods;

	keysPressed |= ~(BUTTONS);

    // Update snake facing
    if (keysPressed & BUTTON_UP) {
        turnSnake(s, UP);
    } else if (keysPressed & BUTTON_RIGHT) {
        turnSnake(s, RIGHT);
    } else if (keysPressed & BUTTON_DOWN) {
        turnSnake(s, DOWN);
    } else if (keysPressed & BUTTON_LEFT) {
        turnSnake(s, LEFT);
    }

	// Update score once every 50 cycles
	if (g->currentCycle % 50 == 0) {
		g->score += s->length;
	}

    // And we're done!
}

// Snake Logic
Snake createSnake() {
    Snake s;
    Point head;
    head.x = SNAKE_START_X;
    head.y = SNAKE_START_Y;
    s.head = head;
    s.length = SNAKE_INITIAL_LENGTH;
	s.growToLength = SNAKE_INITIAL_LENGTH;
    s.facing = SNAKE_INITIAL_FACING;
    s.numTurns = 0;
    s.dead = 0;

    return s;
}

void turnSnake(Snake *s, Direction facing) {
    if (s->facing != facing) {
        if (getOpposite(s->facing) != facing) {
            // This is a valid turn! Move the turns by one.
            for (u16 i = s->numTurns; i > 0; i--) {
                s->turns[i] = s->turns[i - 1];
            }

			Turn t;
			t.location = s->head;
			t.previouslyFacing = s->facing;

            s->turns[0] = t;
            s->numTurns++;

			if (s->numTurns > 49) s->numTurns = 49;

            s->facing = facing;
        }
    }
}

int checkSelfCollision(Snake s) {
	// Get the minX minY all that
	if (s.numTurns < 3) {
		return 0;
	}

	Point head = s.head;
	Point lastTurn = s.turns[0].location;

	// Travel through the snake
	Point current = s.turns[0].location;
	Direction facing = getOpposite(s.facing);
	u16 turnIdx = 0;
	Turn turn;
	int removedDist = distBetween(head, lastTurn);
	u16 remainingLength = (s.length > removedDist) ? s.length - removedDist : 0;

	while (remainingLength > 0) {
		// Check if this is a turn
		if (turnIdx < s.numTurns) {
			turn = s.turns[turnIdx];
			if (current.x == turn.location.x && current.y == turn.location.y) {
				facing = getOpposite(turn.previouslyFacing);
				turnIdx++;
			}
		}

		// Move the current ptr
		switch (facing) {
			case UP:
				current.y--;
				break;
			case DOWN:
				current.y++;
				break;
			case LEFT:
				current.x--;
				break;
			case RIGHT:
				current.x++;
				break;
		}

		remainingLength--;

		if (isBetween(current, head, lastTurn)) {
			return 1;
		}

		// If we flew out, break.
		if (current.x <= 0 || current.x >= SNAKE_BOARD_WIDTH - 1 ||
			current.y <= 0 || current.y >= SNAKE_BOARD_HEIGHT - 1) {
				break;
		}
	}

    return 0;
}

int checkWallCollision(Snake s) {
    return  s.head.x <= 0 ||
            s.head.x >= SNAKE_BOARD_WIDTH - 1 ||
            s.head.y <= 0 ||
            s.head.y >= SNAKE_BOARD_HEIGHT - 1;
}

// Food Logic
Food createRandomFood(Game *g) {
    Food f;
	f.value = FOOD_LENGTH_INCREMENT;
	f.deleteOnCycle = g->currentCycle + FOOD_DURATION;

	Point p;

	/*do {
		p.x = rand() % (TWOPOWGEQWIDTH - 1);
	} while (p.x >= (SNAKE_BOARD_WIDTH * 9) / 10 || p.y <= SNAKE_BOARD_WIDTH / 10);

	do {
		p.y = rand() % (TWOPOWGEQHEIGHT - 1);
	} while (p.y >= (SNAKE_BOARD_HEIGHT * 9) / 10 || p.y <= SNAKE_BOARD_HEIGHT / 10);*/

    p.x = qran_range((SNAKE_BOARD_WIDTH * 9) / 10, SNAKE_BOARD_WIDTH / 10);
    p.y = qran_range((SNAKE_BOARD_HEIGHT * 9) / 10, SNAKE_BOARD_HEIGHT / 10);

	f.location = p;

    return f;
}

int checkFoodCollision(Snake s, Food f) {
	if (s.numTurns > 0) {
		if (isBetween(f.location, s.head, s.turns[0].location)) {
			if (distBetween(f.location, s.head) <= s.length) {
				return 1;
			}
		}
	}

	// Otherwise we just follow the snake towards its back facing direction
	// until we run out of length.
	Point current = s.head;
	Direction facing = getOpposite(s.facing);
	u16 remainingLength = s.length;

	while (remainingLength > 0) {\
		// Check if the food is here
		if (current.x == f.location.x && current.y == f.location.y) {
			return 1;
		}

		// Move the current ptr
		switch (facing) {
			case UP:
				current.y--;
				break;
			case DOWN:
				current.y++;
				break;
			case LEFT:
				current.x--;
				break;
			case RIGHT:
				current.x++;
				break;
		}

		remainingLength--;

		// If we flew out, break.
		if (current.x <= 0 || current.x >= SNAKE_BOARD_WIDTH - 1 ||
			current.y <= 0 || current.y >= SNAKE_BOARD_HEIGHT - 1) {
				break;
		}
	}

	return 0;
}

void eatFood(Food *f, Game *g) {
    // We set the food to expire now so
    // that it will be deleted next cycle
    f->deleteOnCycle = g->currentCycle;

	Snake *s = &(g->snake);
	s->growToLength += f->value;
}

// Auxiliary Logic
int isBetween(Point point, Point p1, Point p2) {
    u16 minX = p1.x < p2.x ? p1.x : p2.x;
    u16 maxX = p1.x > p2.x ? p1.x : p2.x;

    u16 minY = p1.y < p2.y ? p1.y : p2.y;
    u16 maxY = p1.y > p2.y ? p1.y : p2.y;

    u16 x = point.x;
    u16 y = point.y;

    if (x >= minX && x <= maxX && y >= minY && y <= maxY) {
        return 1;
    }

    return 0;
}

int distBetween(Point p1, Point p2) {
	if (p1.x != p2.x) {
		// This means the line is horizontal
		return (p1.x > p2.x) ? p1.x - p2.x : p2.x - p1.x;
	} else {
		return (p1.y > p2.y) ? p1.y - p2.y : p2.y - p1.y;
	}
}

Direction getOpposite(Direction d) {
	switch (d) {
		case UP:
			return DOWN;
		case DOWN:
			return UP;
		case LEFT:
			return RIGHT;
		case RIGHT:
			return LEFT;
	}

	return UP;
}
