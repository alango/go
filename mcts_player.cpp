#include "mcts_player.h"

MCTSPlayer::MCTSPlayer():
  current_node(NULL),
  simulations_per_turn(3000)
{}

MCTSPlayer::~MCTSPlayer() {}

void MCTSPlayer::initialise_current_node(GameState game_state)
{
  current_node = new MCTSNode(game_state);
}

void MCTSPlayer::run_step()
{
  MCTSNode* leaf = current_node->descend_to_leaf();
  MCTSNode* new_node = leaf->expand();
  new_node->simulate_and_update();
}

Coordinate MCTSPlayer::get_move(GameState game_state)
{
  // If there is no current_node, then initialise using the given game_state.
  if (current_node == NULL)
  {
    initialise_current_node(game_state);
  }
  // Not the first move, so update the current node based on the last move.
  else if (!game_state.game_record.empty())
  {
    Coordinate last_move = game_state.game_record.back();
    current_node = current_node->move(last_move);
  }
  // Run MCTS steps.
  for (int i = 0; i < simulations_per_turn; i++)
  {
    if (i%1000==0) {std::cout<<i<<std::endl;}
    run_step();
  }
  current_node->print();
  current_node->print_visit_map();
  current_node->print_win_ratio_map();
  // current_node->print_uct_map();

  // If the player is really far ahead or really far behind,
  // then just finishe the game quickly.
  if (current_node->wins / current_node->visits < 0.02
   || current_node->wins / current_node->visits > 0.98)
  {
    simulations_per_turn = 500;
  }

  // Find move with most visits.
  Coordinate move = current_node->select_move();
  // Delete unnecessary branches.
  current_node = current_node->move(move);

  return move;
}

MCRAVEPlayer::MCRAVEPlayer()
{
  simulations_per_turn = 3000;
}

MCRAVEPlayer::~MCRAVEPlayer()
{}

void MCRAVEPlayer::initialise_current_node(GameState game_state)
{
  current_node = new MCTSNode(game_state);
}

void MCRAVEPlayer::run_step()
{
  MCRAVENode* leaf = (MCRAVENode*) current_node->descend_to_leaf();
  MCRAVENode* new_node = (MCRAVENode*) leaf->expand();
  new_node->simulate_and_update();
}
