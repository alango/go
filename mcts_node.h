#ifndef MCTS_NODE_H
#define MCTS_NODE_H

#include <vector>

class MCTSNode
{
private:
	int visits;
	int wins;
	std::vector<*MCTSNode> children;
public:
  MCTSNode();
  ~MCTSNode();
};

#endif