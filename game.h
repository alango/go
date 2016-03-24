#ifndef GAME_H
#define GAME_H

#include "game_state.h"
#include "player.h"
#include "mcts_player.h"

class Game
{
private:
  GameState game_state;
  MCTSPlayer black;
  MCRAVEPlayer white;

public:
  Game();
  ~Game();
  void turn();
};

#endif
