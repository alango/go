#ifndef MCTS_PLAYER_H
#define MCTS_PLAYER_H

#include "player.h"
#include "mcts_node.h"

class MCTSPlayer: public Player
{
private:
  MCTSNode* current_node;
public:
  MCTSPlayer();
  ~MCTSPlayer();
  Coordinate get_move(GameState game_state);
};

#endif