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
public:
  double visits;
  double wins;
  std::vector<MCTSNode*> children;
  MCTSGameState game_state;
  MCTSNode* parent_node;
  Coordinate current_move;
public:
  MCTSNode(GameState game_state);
  ~MCTSNode();
  void print();
  // Print the board with the UCT values for each move.
  void print_uct_map();
  // Returns true if the node has no children.
  bool is_leaf();
  // Creates child nodes for each possible move.
  void expand();
  // Returns the UCT value for the given node.
  double get_uct(MCTSNode* node);
  // Compares the UCT values between two nodes. Used by select_child();
  bool compare_uct(MCTSNode* node1, MCTSNode* node2);
  // Selects the child node with the highest UCT value.
  MCTSNode* select_child();
  // Descends through the tree using select_child until a leaf node is reached.
  MCTSNode* descend_to_leaf();
  // Randomly playout a game and update the win/visit information.
  void simulate_and_update();
  // Update the win/visit information, and call update on parent nodes.
  void update(bool win);
};

#endif