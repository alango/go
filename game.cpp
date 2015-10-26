#include "game.h"

Game::Game()
{
	Coordinate move;
	while (true)
	{
		game_state.print();
		move = player1.get_move(game_state);
		game_state.play_move(move);
		
		game_state.print();
		move = player2.get_move(game_state);
		game_state.play_move(move);
	}
}

Game::~Game() {};