#include "mcts_node.h"

MCTSNode::MCTSNode(GameState game_state)
{
	visits = 0;
	wins = 0;
	this->game_state = game_state;
}

MCTSNode::~MCTSNode() {}

bool MCTSNode::is_leaf()
{
	if (children.size() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MCTSNode::expand()
{
	Coordinate move;
  for (int y = 0; y < BOARD_SIZE; y++)
  {
    for (int x = 0; x < BOARD_SIZE; x++)
    {
      move.set(x,y);
      if (game_state.is_legal(move) == LEGAL_MOVE)
      {
      	MCTSNode new_node(game_state);
      	new_node.game_state.play_move(move);
      	children.push_back(&new_node);
      }
    }
  }
}