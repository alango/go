#ifndef RANDOM_PLAYER_H
#define RANDOM_PLAYER_H

#include "player.h"

class RandomPlayer: public Player
{
public:
	Coordinate get_move(GameState game_state);
};

#endif