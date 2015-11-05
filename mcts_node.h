#ifndef MCTS_NODE_H
#define MCTS_NODE_H

#include <vector>
#include "game_state.h"

class MCTSNode
{
private:
	int visits;
	int wins;
	std::vector<MCTSNode*> children;
	GameState game_state;
public:
  MCTSNode(GameState game_state);
  ~MCTSNode();
  bool is_leaf();
  void expand();
};

#endif