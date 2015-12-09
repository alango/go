#ifndef GAME_H
#define GAME_H

#include "game_state.h"
#include "human_player.h"
#include "random_player.h"
#include "mcts_player.h"

class Game
{
private:
  GameState game_state;
  MCRAVEPlayer black;
  MCRAVEPlayer white;

public:
  Game();
  ~Game();
  void turn();
};

#endif
