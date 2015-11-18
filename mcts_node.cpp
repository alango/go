#include <math.h>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include "mcts_node.h"

MCTSGameState::MCTSGameState(GameState game_state)
{
  srand(time(NULL));
  memcpy(board_state, game_state.board_state, sizeof(board_state));
  to_play = game_state.to_play;
  other_player = game_state.other_player;
  ko = game_state.ko;
  one_pass = game_state.one_pass;
  game_record = game_state.game_record;
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
  static double time_to_find_move = 0;
  static double time_to_play_move = 0;
  std::clock_t start = std::clock();
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
    std::cout << "Time to find move: " << time_to_find_move/(double)CLOCKS_PER_SEC << std::endl;
    std::cout << "Playing time: " << time_to_play_move/(double)CLOCKS_PER_SEC << std::endl;
  }
  double found_move = std::clock();
  time_to_find_move += found_move-start;
  play_move(move);
  double move_played = std::clock();
  time_to_play_move += move_played-found_move;
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
  is_leaf(true),
  visits(0),
  wins(0),
  game_state(game_state),
  parent_node(NULL)
{}

MCTSNode::~MCTSNode()
{
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    delete *child;
  }
}

void MCTSNode::print()
{
  std::cout << "Visits: " << visits << std::endl;
  std::cout << "Wins: " << wins << std::endl;
  std::cout << "Leaf node: " << is_leaf << std::endl;
  std::cout << "Number of children: " << children.size() << std::endl;
  game_state.print();
}

void MCTSNode::print_uct_map()
{
  double board[BOARD_SIZE][BOARD_SIZE];
  // Initialise board to all 0s.
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      board[row][col] = 0;
    }
  }

  double uct;
  // For each child node, add its UCT value to the board.
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    uct = get_uct(*child);
    board[(*child)->current_move.y][(*child)->current_move.x] = uct;
  }

  // Print the board.
  std::cout << std::setprecision(3) << std::fixed;
  std::cout << "UCT map: " << std::endl;
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      std::cout << board[row][col] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void MCTSNode::print_visit_map()
{
  int board[BOARD_SIZE][BOARD_SIZE];
  // Initialise board to all 0s.
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      board[row][col] = 0;
    }
  }

  // For each child node, add the number of times it has been
  // visited to the board.
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    board[(*child)->current_move.y][(*child)->current_move.x] = (*child)->visits;
  }

  // Print the board.
  std::cout << "Visit map: " << std::endl;
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      std::cout << board[row][col] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void MCTSNode::print_win_ratio_map()
{
  double board[BOARD_SIZE][BOARD_SIZE];
  // Initialise board to all 0s.
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      board[row][col] = 0;
    }
  }

  // For each child node, add its win ratio to the board.
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    board[(*child)->current_move.y][(*child)->current_move.x] = (*child)->wins/(*child)->visits;
  }

  // Print the board.
  std::cout << std::setprecision(2) << std::fixed;
  std::cout << "Win ratio map: " << std::endl;
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      std::cout << board[row][col] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void MCTSNode::expand()
{
  is_leaf = false;
  Coordinate move;
  for (int y = 0; y < BOARD_SIZE; y++)
  {
    for (int x = 0; x < BOARD_SIZE; x++)
    {
      move.set(x,y);
      if (game_state.is_legal(move) == LEGAL_MOVE
       && !game_state.is_eye(move, game_state.to_play))
      {
        MCTSNode* child_node = NULL;
        if (!(child_node = new MCTSNode(game_state)))
        {
          std::cout << "Out of memory" << std::endl;
          exit(1);
        }
        child_node->parent_node = this;
        child_node->game_state.play_move(move);
        child_node->current_move = move;
        children.push_back(child_node);
      }
    }
  }
  move.set(-1,-1);
  MCTSNode* child_node = NULL;
  if (!(child_node = new MCTSNode(game_state)))
  {
    std::cout << "Out of memory" << std::endl;
    exit(1);
  }
  child_node->parent_node = this;
  child_node->game_state.play_move(move);
  child_node->current_move = move;
  children.push_back(child_node);
}

double MCTSNode::get_uct(MCTSNode* node)
{
  if (node->visits == 0) { return 1000; }
  double ans = (node->wins/node->visits) + sqrt(log(2*visits)/node->visits);
  return ans;
}

bool MCTSNode::compare_uct(MCTSNode* node1, MCTSNode* node2)
// Returns true if node1 has a lower UCT value than node2.
{
  if (get_uct(node1) < get_uct(node2)) { return true; }
  else { return false; }
}

MCTSNode* MCTSNode::select_child()
{
  MCTSNode* best = *(children.begin());
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    if (compare_uct(best, *child))
    {
      best = *child;
    }
  }
  return best;
}

Coordinate MCTSNode::select_move()
{
  Coordinate move(-1,-1);
  int most_visits = 0;
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    if ((*child)->visits > most_visits)
    {
      most_visits = (*child)->visits;
      move = (*child)->current_move;
    }
  }
  return move;
}

MCTSNode* MCTSNode::descend_to_leaf()
{
  if (is_leaf)
  {
    return this;
  }
  else
  {
    return select_child()->descend_to_leaf();
  }
}

void MCTSNode::update(bool win)
{
  if (win) { wins++; }
  visits++;
  if (parent_node != NULL)
  {
    // A win for the current node is a loss for the parent
    // node and vice versa.
    parent_node->update(!win);
  }
}

void MCTSNode::simulate_and_update()
{
  int result = game_state.simulate_game();
  // Simulate a game and check if it is win for the player who played
  // the last move.
  if (result > 0 && game_state.other_player == BLACK) { update(true); }
  else if (result < 0 && game_state.other_player == WHITE) { update(true); }
  else { update(false); }
}

MCTSNode* MCTSNode::move(Coordinate move)
{
  MCTSNode* new_current_node;
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    if (!((*child)->current_move == move))
    {
      delete *child;
    }
    else
    {
      new_current_node = *child;
      new_current_node->parent_node = NULL;
    }
  }
  return new_current_node;
}