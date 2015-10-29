#ifndef GAME_H
#define GAME_H

#include "game_state.h"
#include "human_player.h"
#include "random_player.h"

class Game
{
private:
  GameState game_state;
  RandomPlayer black;
  RandomPlayer white;
  Coordinate pass;
  bool first_pass;
  bool game_over;
  list_of_points game_record;

public:
  Game();
  ~Game();
  void turn();
};

#endif