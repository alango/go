#include "random_player.h"

Coordinate RandomPlayer::get_move(GameState game_state)
{
	return game_state.random_move();
}