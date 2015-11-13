#include "mcts_player.h"

MCTSPlayer::MCTSPlayer()
{
  GameState game_state;
  current_node = new MCTSNode(game_state);
  current_node->expand();
}

MCTSPlayer::~MCTSPlayer() {}

Coordinate MCTSPlayer::get_move(GameState game_state)
{
  // Update the current node and delete unnecessary branches.
  if (!game_state.game_record.empty())
  {
    Coordinate last_move = game_state.game_record.back();
    current_node = current_node->move(last_move);
  }
  current_node->print();
  // Run MCTS steps.
  for (int i = 0; i < 500; i++)
  {
    if (i%100==0) {std::cout<<i<<std::endl;}
    MCTSNode* leaf = current_node->descend_to_leaf();
    leaf->expand();
    leaf->simulate_and_update();
  }
  current_node->print();
  current_node->print_visit_map();
  current_node->print_win_ratio_map();
  current_node->print_uct_map();

  // Find move with most visits.
  Coordinate move = current_node->select_move();
  // Delete unnecessary branches.
  current_node = current_node->move(move);

  return move;
}