#ifndef GAME_H
#define GAME_H

#include "game_state.h"
#include "human_player.h"

class Game
{
private:
	GameState game_state;
	HumanPlayer player1;
	HumanPlayer player2;

public:
	Game();
	~Game();
};

#endif