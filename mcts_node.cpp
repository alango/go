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

Coordinate MCTSGameState::select_random_move()
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
  return move;
}

Coordinate MCTSGameState::best_of_n_move()
{
  Coordinate best_move(-1,-1);
  int best_score = -100;
  Coordinate current_move;
  int current_score;
  for (int i=0; i<BEST_OF_N; i++)
  {
    current_move = select_random_move();
    current_score = heuristic_score(current_move);
    if (current_score > best_score)
    {
      best_score = current_score;
      best_move = current_move;
    }
  }
  return best_move;
}

int MCTSGameState::heuristic_score(Coordinate move)
{
  int score = 0;
  // Pass receives the lowest score.
  if (move == pass) { return -100; }
  
  // Penalise moves on the first two lines.
  if (move.x <= 1 || move.y <= 1 ||
           move.x >= BOARD_SIZE-2 || move.y >= BOARD_SIZE-2)
  {
    score -= 5;
  }

  // Favour moves that respond locally to the last move.
  Coordinate last_move = game_record.back();
  if (!(last_move == pass))
  {
    int manhattan_dist = std::abs(move.x - last_move.x) +
                         std::abs(move.y - last_move.y);
    if (manhattan_dist <= 3)
    {
      score += 1;
    }
  }

  return score;
}

GameState MCTSGameState::simulate_game()
{
  // Create a copy of the current game_state to run the simulation on.
  MCTSGameState playout_game_state = *this;
  while (!playout_game_state.game_over)
  {
    Coordinate move = playout_game_state.select_random_move();
    playout_game_state.play_move(move);
  }
  return playout_game_state;
}

GameState MCTSGameState::heavy_simulate_game()
{
  // Create a copy of the current game_state to run the simulation on.
  MCTSGameState playout_game_state = *this;
  while (!playout_game_state.game_over)
  {
    Coordinate move = playout_game_state.best_of_n_move();
    playout_game_state.play_move(move);
  }
  return playout_game_state;
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
  int result = game_state.simulate_game().score_game();
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
  // GameState result_game_state = game_state.simulate_game();
  GameState result_game_state = game_state.heavy_simulate_game();
  int result = result_game_state.score_game();
  list_of_points game_record = result_game_state.game_record;
  if ((result > 0 && game_state.other_player == BLACK)
   || (result < 0 && game_state.other_player == WHITE))
  {
    rave_update(true, game_record);
  }
  else
  {
    rave_update(false, game_record);
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
