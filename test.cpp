#include <assert.h>
#include <iostream>
#include "test.h"
#include "game_state.h"
#include "mcts_node.h"

void ko_test()
{
  GameState game;
  Coordinate move;

  move.set(4,4);
  game.play_move(move);

  move.set(3,4);
  game.play_move(move);

  move.set(3,3);
  game.play_move(move);

  move.set(2,3);
  game.play_move(move);

  move.set(3,5);
  game.play_move(move);

  move.set(2,5);
  game.play_move(move);

  move.set(8,8);
  game.play_move(move);

  move.set(1,4);
  game.play_move(move);

  move.set(2,4);
  game.play_move(move);

  move.set(3,4);
  // Shouldn't be able to take the ko.
  assert(game.is_legal(move) == KO_POINT);

  move.set(8,7);
  game.play_move(move);

  move.set(8,0);
  game.play_move(move);
  
  move.set(3,4);
  // Should now be able to take the ko.
  assert(game.is_legal(move) == LEGAL_MOVE);

  std::cout << "Ko test passed" << std::endl;
}

void eye_test()
{
  GameState game;
  Coordinate move;

  move.set(4,4);
  game.play_move(move);

  move.set(1,0);
  game.play_move(move);

  move.set(4,6);
  game.play_move(move);

  move.set(0,1);
  game.play_move(move);
  // False eye in the corner.
  assert(game.is_eye(move, EMPTY) == false);

  move.set(5,5);
  game.play_move(move);

  move.set(1,1);
  game.play_move(move);
  
  // Real eye in the corner.
  move.set(0,0);
  assert(game.is_eye(move, EMPTY) == true);

  move.set(3,5);
  game.play_move(move);

  move.set(2,1);
  game.play_move(move);

  move.set(3,4);
  game.play_move(move);

  move.set(3,0);
  game.play_move(move);
  
  // False eye on the side.
  move.set(2,0);
  assert(game.is_eye(move, EMPTY) == false);

  move.set(5,6);
  game.play_move(move);
  
  // False eye in the middle.
  move.set(4,5);
  assert(game.is_eye(move, EMPTY) == false);

  move.set(3,1);
  game.play_move(move);
  
  // Real eye on the side
  move.set(2,0);
  assert(game.is_eye(move, EMPTY) == true);

  move.set(5,4);
  game.play_move(move);
  
  // Real eye in the middle.
  move.set(4,5);
  assert(game.is_eye(move, EMPTY) == true);

  std::cout << "Eye test passed" << std::endl;
}

void MCTSNode_test()
{
  GameState game_state;
  Coordinate move(0,0);
  game_state.play_move(move);
  move.set(0,1);
  game_state.play_move(move);
  MCTSNode mcts_node(game_state);
  mcts_node.print();
  MCTSNode* leaf;
  for (int i = 0; i < 100; i++)
  {
    std::cout << i << std::endl;
    leaf = mcts_node.descend_to_leaf();
    leaf->expand();
  }
  mcts_node.print();
  mcts_node.print_visit_map();
  mcts_node.print_uct_map();
}