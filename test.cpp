#include "test.h"
#include "game_state.h"

int ko_test()
{
	GameState game;
	Coordinate move;

	move.x = 4;
	move.y = 4;
	game.play_move(move);

	move.x = 3;
	move.y = 4;
	game.play_move(move);

	move.x = 3;
	move.y = 3;
	game.play_move(move);

	move.x = 2;
	move.y = 3;
	game.play_move(move);

	move.x = 3;
	move.y = 5;
	game.play_move(move);

	move.x = 2;
	move.y = 5;
	game.play_move(move);

	move.x = 8;
	move.y = 8;
	game.play_move(move);

	move.x = 1;
	move.y = 4;
	game.play_move(move);

	game.print();

	while (true)
  {
	  game.human_move();
	  game.print();
  }
	return 0;
}