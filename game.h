#ifndef GAME_H
#define GAME_H

#include "game_state.h"
#include "human_player.h"
#include "random_player.h"

class Game
{
private:
	GameState game_state;
	RandomPlayer player1;
	HumanPlayer player2;

public:
	Game();
	~Game();
};

#endif