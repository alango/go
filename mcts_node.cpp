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
  possible_moves = game_state.possible_moves;
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
  is_leaf(true),
  visits(0),
  wins(0),
  potential_children(game_state.possible_moves),
  game_state(game_state),
  parent_node(NULL)
{
  if (!game_state.game_record.empty())
  {
    current_move = game_state.game_record.back();
  }
}

Coordinate MCTSNode::pass(-1,-1);

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
  std::cout << "Win ratio: " << wins / visits << std::endl;
  std::cout << "Leaf node: " << is_leaf << std::endl;
  std::cout << "Number of children: " << children.size() << std::endl;
  game_state.print();
}

void MCTSNode::print_maps()
{
  print();
  print_visit_map();
  print_win_ratio_map();
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

  // For each child node, add its UCT value to the board.
  double uct;
  Coordinate current_move;
  double pass_uct = 0;
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    uct = get_node_score(*child);
    current_move = (*child)->current_move;
    if (current_move == pass)
    {
      pass_uct = uct;
    }
    else
    {
      board[current_move.y][current_move.x] = uct;
    }
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
  std::cout << "Pass UCT: " << pass_uct << std::endl;
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
  Coordinate current_move;
  int passes = 0;
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    current_move = (*child)->current_move;
    if (current_move == pass)
    {
      passes = (*child)->visits;
    }
    else
    {
      board[current_move.y][current_move.x] = (*child)->visits;
    }
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
  std::cout << "Passes: " << passes << std::endl;
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
  Coordinate current_move;
  double pass_win_ratio;
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    current_move = (*child)->current_move;
    if (current_move == pass)
    {
      pass_win_ratio = (*child)->wins/(*child)->visits;
    }
    else
    {
      board[current_move.y][current_move.x] = (*child)->wins/(*child)->visits;
    }
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
  std::cout << "Pass: " << pass_win_ratio << std::endl;
  std::cout << std::endl;
}

MCTSNode* MCTSNode::create_child(Coordinate move)
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
  return child_node;
}

MCTSNode* MCTSNode::expand()
{
  MCTSNode* child_node = NULL;
  Coordinate move;
  int move_index;
  bool child_created = false;
  while (!child_created && !potential_children.empty())
  {
    child_created = true;
    move_index = rand() % potential_children.size();
    move = potential_children[move_index];
    if (game_state.is_legal(move) != LEGAL_MOVE
     || game_state.is_eye(move, game_state.to_play))
    {
      child_created = false;
    }
    else
    {
      child_node = create_child(move);
    }
    potential_children.erase(potential_children.begin() + move_index);
  }
  if (potential_children.empty() && !children.empty())
  {
    is_leaf = false;
    // If there were no legal moves from this position, then return the
    // current node as the leaf node.
  }
  if (!child_created) { return this; }
  return child_node;
}

double MCTSNode::get_node_score(MCTSNode* node)
{
  if (node->visits == 0) { return 1000; }
  double ans = (node->wins/node->visits) + sqrt(log(2*visits)/node->visits);
  return ans;
}

bool MCTSNode::compare_node_scores(MCTSNode* node1, MCTSNode* node2)
// Returns true if node1 has a lower UCT value than node2.
{
  if (get_node_score(node1) < get_node_score(node2)) { return true; }
  else { return false; }
}

MCTSNode* MCTSNode::select_child()
{
  MCTSNode* best = *(children.begin());
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    if (compare_node_scores(best, *child))
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
    if ((*child)->visits > most_visits && !((*child)->current_move == pass))
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
  // Create a copy of the current game_state to run the simulation on.
  MCTSGameState playout_game_state = game_state;
  while (!playout_game_state.game_over)
  {
    playout_game_state.random_move();
  }
  int result = playout_game_state.score_game();
  // Simulate a game and check if it is win for the player who played
  // the last move.
  if (result > 0 && game_state.other_player == BLACK) { update(true); }
  else if (result < 0 && game_state.other_player == WHITE) { update(true); }
  else { update(false); }
}

MCTSNode* MCTSNode::move(Coordinate move)
{
  MCTSNode* new_current_node = NULL;
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
  if (new_current_node == NULL)
  {
    new_current_node = create_child(move);
    new_current_node->parent_node = NULL;
  }
  return new_current_node;
}

MCRAVENode::MCRAVENode(GameState game_state):
  MCTSNode(game_state),
  rave_visits(0),
  rave_wins(0)
{}

MCRAVENode* MCRAVENode::create_child(Coordinate move)
{
  MCRAVENode* child_node = NULL;
  if (!(child_node = new MCRAVENode(game_state)))
  {
    std::cout << "Out of memory" << std::endl;
    exit(1);
  }
  child_node->parent_node = this;
  child_node->game_state.play_move(move);
  child_node->current_move = move;
  children.push_back(child_node);
  return child_node;
}

double MCRAVENode::k = 1000;

void MCRAVENode::simulate_and_update()
{
  // Create a copy of the current game_state to run the simulation on.
  MCTSGameState playout_game_state = game_state;
  while (!playout_game_state.game_over)
  {
    playout_game_state.random_move();
  }
  int result = playout_game_state.score_game();
  if ((result > 0 && game_state.other_player == BLACK)
   || (result < 0 && game_state.other_player == WHITE))
  {
    rave_update(true, playout_game_state.game_record);
  }
  else
  {
    rave_update(false, playout_game_state.game_record);
  }
}

double MCRAVENode::get_node_score(MCTSNode* node)
{
  if (node->visits == 0) { return 1000; }
  double beta = sqrt(k / ((3 * node->visits) + k));
  double mc_score = node->wins / node->visits;
  double rave_score = (((MCRAVENode*)node)->rave_wins/((MCRAVENode*)node)->rave_visits);
  return  (beta*rave_score) + ((1-beta)*mc_score) + (sqrt(log(2*visits)/node->visits));
}

void MCRAVENode::rave_update(bool win, list_of_points game_record)
{
  // Update normal and RAVE counts for current node.
  if (win)
  { 
    wins++;
    rave_wins++;
  }
  visits++;
  rave_visits++;
  
  // Update AMAF counts for any child nodes.
  if (!is_leaf)
  {
    // The move number of the children of this node.
    int move_number = game_state.game_record.size();
    int current_move_number = move_number;
    Coordinate move = game_record.at(current_move_number);
    while (!(move == pass))
    {
      // Check if the move has previously been played.
      if (std::find(game_record.begin() + move_number,
                    game_record.begin() + current_move_number,
                    move) == game_record.begin() + current_move_number)
      {
        // Update AMAF values for the corresponding child node.
        for (std::vector<MCTSNode*>::iterator child = children.begin();
             child != children.end();
             child++)
        {
          if (move == (*child)->current_move)
          {
            if (!win)
            {
              ((MCRAVENode*)(*child))->rave_wins++;
            }
            ((MCRAVENode*)(*child))->rave_visits++;
          }
        }
      }
      current_move_number += 2;
      move = game_record.at(current_move_number);
    }
  }
  if (parent_node != NULL)
  {
    // A win for the current node is a loss for the parent
    // node and vice versa.
    ((MCRAVENode*)parent_node)->rave_update(!win, game_record);
  }
}

void MCRAVENode::print_maps()
{
  std::cout << "Printing RAVE maps" << std::endl;
  MCTSNode::print_maps();
  print_rave_win_ratio_map();
  print_rave_visit_map();
}

void MCRAVENode::print_rave_visit_map()
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
  Coordinate current_move;
  int passes = 0;
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    MCRAVENode* mcrave_child = (MCRAVENode*) *child;
    current_move = mcrave_child->current_move;
    if (current_move == pass)
    {
      passes = mcrave_child->rave_visits;
    }
    else
    {
      board[current_move.y][current_move.x] = mcrave_child->rave_visits;
    }
  }

  // Print the board.
  std::cout << "RAVE visit map: " << std::endl;
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      std::cout << board[row][col] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "Passes: " << passes << std::endl;
  std::cout << std::endl;
}

void MCRAVENode::print_rave_win_ratio_map()
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

  // For each child node, add the number of times wins that have been
  // using AMAF to the board.
  double pass_win_ratio = 0;
  Coordinate current_move;
  for (std::vector<MCTSNode*>::iterator child = children.begin();
       child != children.end();
       child++)
  {
    MCRAVENode* mcrave_child = (MCRAVENode*) *child;
    current_move = mcrave_child->current_move;
    if (current_move == pass)
    {
      pass_win_ratio = mcrave_child->rave_wins / mcrave_child->rave_visits;
    }
    else
    {
      board[current_move.y][current_move.x] = mcrave_child->rave_wins / mcrave_child->rave_visits;
    }
  }

  // Print the board.
  std::cout << "RAVE win ratio map: " << std::endl;
  std::cout << std::setprecision(2) << std::fixed;
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