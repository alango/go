#include <iostream>
#include <vector>
#include "game_state.h"
#include "test.h"

int main()
{
	ko_test();

	GameState game;

	game.print();

  while (true)
  {
	  game.human_move();
	  game.print();
  }
	return 0;
}

