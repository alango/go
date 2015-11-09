#ifndef MCTS_NODE_H
#define MCTS_NODE_H

#include <vector>
#include "game_state.h"

class MCTSGameState : public GameState
// A derived class of GameState containing extra methods for use
// during MCTS.
{
  list_of_points all_points;
  Coordinate last_move;
public:
  MCTSGameState(GameState game_state);
  ~MCTSGameState();
  // Plays out a random game from the current position to the end and
  // returns black's score - white's score.
  int simulate_game();
private:
  // Selects a random, legal move and plays it.
  void random_move();
};

class MCTSNode
{
private:
  int visits;
  int wins;
  std::vector<MCTSNode*> children;
  MCTSGameState game_state;
public:
  MCTSNode(GameState game_state);
  ~MCTSNode();
  void print();
  // Returns true if the node has no children.
  bool is_leaf();
  // Creates child nodes for each possible move.
  void expand();
  // Randomly playout a game and update the win/visit information.
  void simulate_and_update();
};

#endif