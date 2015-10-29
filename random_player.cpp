#include "random_player.h"

RandomPlayer::RandomPlayer()
{
	srand(time(NULL));
	Coordinate move;
	for (int y = 0; y < BOARD_SIZE; y++)
	{
		move.y = y;
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			move.x = x;
			possible_moves.push_back(move);
    }
  }
}

RandomPlayer::~RandomPlayer() {};

Coordinate RandomPlayer::get_move(GameState game_state)
{
	possible_moves_this_turn = possible_moves;
	Coordinate move;
	bool move_found = false;
	int move_index;
	while (!move_found && !possible_moves_this_turn.empty())
	{
		move_found = true;
		move_index = rand() % (possible_moves_this_turn.size());
		move = possible_moves_this_turn[move_index];
		if (game_state.is_legal(move) != LEGAL_MOVE || game_state.is_eye(move))
		{
			possible_moves_this_turn.erase(possible_moves_this_turn.begin() + move_index);
			move_found = false;
		}
	}
	if (possible_moves_this_turn.empty())
	{
		move.x = -1;
		move.y = -1;
	}
	return move;
}