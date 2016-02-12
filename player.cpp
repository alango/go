#include <iostream>
#include "player.h"

Coordinate HumanPlayer::request_move()
{
  int x,y;
  std::cout << "Enter x co-ordinate:" << std::endl;
  std::cin >> x;
  std::cout << "Enter y co-ordinate:" << std::endl;
  std::cin >> y;
  Coordinate coordinate(x-1, y-1);
  return coordinate;
}

Coordinate HumanPlayer::get_move(GameState game_state)
{
  Coordinate move;
  bool legal_move = false;
  is_legal_responses is_legal_response;
  while (!legal_move)
  {
    move = request_move();
    is_legal_response = game_state.is_legal(move);
    if (is_legal_response == LEGAL_MOVE)
    {
      legal_move = true;
    }
    else if (is_legal_response == POINT_NOT_ON_BOARD)
    {
      std::cout << "That is not a point on the board" << std::endl;
    }
    else if (is_legal_response == POINT_OCCUPIED)
    {
      std::cout << "There is already a stone on that point" << std::endl;
    }
    else if (is_legal_response == KO_POINT)
    {
      std::cout << "Ko rule: You must play elsewhere first" << std::endl;
    }
    else if (is_legal_response == SUICIDE)
    {
      std::cout << "That move is suicide" << std::endl;
    }
  }
  return move;
}

RandomPlayer::RandomPlayer()
{
  srand(time(NULL));
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

RandomPlayer::~RandomPlayer() {};

Coordinate RandomPlayer::get_move(GameState game_state)
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
    if (game_state.is_legal(move) != LEGAL_MOVE || game_state.is_eye(move, game_state.to_play))
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

NeuralNetPlayer::NeuralNetPlayer():
  net(163,40)
{
  net.read_weights_from_file();
}

NeuralNetPlayer::~NeuralNetPlayer() {}

Coordinate NeuralNetPlayer::get_move(GameState game_state)
{
  list_of_points moves;
  std::vector<double> scores;
  for (list_of_points::iterator move = game_state.possible_moves.begin();
       move != game_state.possible_moves.end();
       move++)
  {
    if (game_state.is_legal(*move) == LEGAL_MOVE && !game_state.is_eye(*move, game_state.to_play))
    {
      moves.push_back(*move);
      game_state.set_point(*move, game_state.to_play);
      scores.push_back(net.process_inputs(game_state.create_net_inputs()));
      game_state.set_point(*move, EMPTY);
    }
  }

  Coordinate best_move;
  double best_score = 0;
  for (int i=0; i < moves.size(); i++)
  {
    if (scores[i] > best_score)
    {
      best_score = scores[i];
      best_move = moves[i];
    }
  }
  net.update_weights(game_state.create_net_inputs(), best_score);
  return best_move;
}

