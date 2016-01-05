#include <iomanip>
#include "mcts_node.h"

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