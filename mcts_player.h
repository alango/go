#ifndef MCTS_PLAYER_H
#define MCTS_PLAYER_H

#include "player.h"
#include "mcts_node.h"

class MCTSPlayer: public Player
{
protected:
  MCTSNode* current_node;
  int simulations_per_turn;
public:
  MCTSPlayer();
  ~MCTSPlayer();
  // Initialises an MCTSNode if no current_node exists.
  void initialise_current_node(GameState game_state);
  // Runs a single simulation and update.
  void run_step();
  Coordinate get_move(GameState game_state);
};

class MCRAVEPlayer: public MCTSPlayer
{
public:
  MCRAVEPlayer();
  ~MCRAVEPlayer();
  void initialise_current_node(GameState game_state);
  void run_step();
};

#endif