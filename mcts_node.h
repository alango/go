#ifndef MCTS_NODE_H
#define MCTS_NODE_H

#include <vector>
#include "game_state.h"

class MCTSGameState : public GameState
// A derived class of GameState containing extra methods for use
// during MCTS.
{
  list_of_points all_points;
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
  GameState game_state;
public:
  MCTSNode(GameState game_state);
  ~MCTSNode();
  bool is_leaf();
  void expand();
};

#endif