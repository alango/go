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
  virtual void initialise_current_node(GameState game_state);
  // Runs a single simulation and update.
  virtual void run_step();
  Coordinate get_move(GameState game_state);
};

class MCRAVEPlayer: public MCTSPlayer
{
public:
  MCRAVEPlayer();
  ~MCRAVEPlayer();
  virtual void initialise_current_node(GameState game_state);
  virtual void run_step();
};

#endif