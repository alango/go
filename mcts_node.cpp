#include "mcts_node.h"

MCTSGameState::MCTSGameState(GameState game_state)
{
  srand(time(NULL));
  memcpy(board_state, game_state.board_state, sizeof(board_state));
  to_play = game_state.to_play;
  other_player = game_state.other_player;
  ko = game_state.ko;
  one_pass = game_state.one_pass;
  Coordinate move;
  for (int y = 0; y < BOARD_SIZE; y++)
  {
    for (int x = 0; x < BOARD_SIZE; x++)
    {
      move.set(x,y);
      all_points.push_back(move);
    }
  }
}

MCTSGameState::~MCTSGameState() {}

void MCTSGameState::random_move()
{
  list_of_points possible_moves = all_points;
  Coordinate move;
  bool move_found = false;
  int move_index;
  while (!move_found && !possible_moves.empty())
  {
    move_found = true;
    move_index = rand() % (possible_moves.size());
    move = possible_moves[move_index];
    if (is_legal(move) != LEGAL_MOVE || is_eye(move, to_play))
    {
      possible_moves.erase(possible_moves.begin() + move_index);
      move_found = false;
    }
  }
  if (possible_moves.empty())
  {
    // No moves remaining so pass.
    move.set(-1,-1);
  }
  play_move(move);
}

int MCTSGameState::simulate_game()
{
  // Create a copy of the current game_state to run the simulation on.
  MCTSGameState playout_game_state = *this;
  while (!playout_game_state.game_over)
  {
    playout_game_state.random_move();
  }
  return playout_game_state.score_game();
}

MCTSNode::MCTSNode(GameState game_state):
  visits(0),
  wins(0),
  game_state(game_state)
{}

MCTSNode::~MCTSNode() {}

void MCTSNode::print()
{
  std::cout << "Visits: " << visits << std::endl;
  std::cout << "Wins: " << wins << std::endl;
  std::cout << "Leaf node: " << is_leaf() << std::endl;
  std::cout << "Number of children: " << children.size() << std::endl;
  game_state.print();
}

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

void MCTSNode::simulate_and_update()
{
  int result = game_state.simulate_game();
  // Simulate a game and check if it is win for the player who played
  // the last move.
  if (result > 0 && game_state.other_player == BLACK) { wins++; }
  else if (result < 0 && game_state.other_player == WHITE) { wins++; }
  visits++;
}