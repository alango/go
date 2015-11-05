#ifndef MCTS_PLAYER_H
#define MCTS_PLAYER_H

#include "player.h"

class MCTSPlayer: public Player
{
private:
  list_of_points possible_moves;
public:
  MCTSPlayer();
  ~MCTSPlayer();
  Coordinate get_move(GameState game_state);
};

#endif