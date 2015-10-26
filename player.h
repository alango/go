#ifndef PLAYER_H
#define PLAYER_H

#include "game_state.h"

class Player
{
public:
	virtual Coordinate get_move(GameState game_state) = 0;
};

#endif