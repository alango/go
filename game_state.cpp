#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include "game_state.h"

bool operator==(Coordinate point1, Coordinate point2)
{
  if (point1.x != point2.x) { return false; }
  if (point1.y != point2.y) { return false; }
  return true;
}

Coordinate::Coordinate()
{
  x = -1;
  y = -1;
}

Coordinate::Coordinate(int x, int y)
{
  this->x = x;
  this->y = y;
}

Coordinate::~Coordinate() {}

void Coordinate::set(int x, int y)
{
  this->x = x;
  this->y = y;
}

void Coordinate::print()
{
  // Print coordinate numbers from 1-N, not 0-(N-1).
  std::cout << "(" << x+1 << ", " << y+1 << ")" << std::endl;
}

GameState::GameState()
{
  Coordinate move;
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      board_state[row][col] = EMPTY;
      move.set(col, row);
      possible_moves.push_back(move);
    }
  }

  to_play = BLACK;
  other_player = WHITE;
  one_pass = false;
  game_over = false;
  ko.set(-1,-1);

  move.set(2,2);
  handicap_points.push_back(move);
  move.set(6,6);
  handicap_points.push_back(move);
  move.set(2,6);
  handicap_points.push_back(move);
  move.set(6,2);
  handicap_points.push_back(move);
  move.set(4,4);
  handicap_points.push_back(move);
}

GameState::~GameState() {};

int GameState::komi = 0;

Coordinate GameState::pass(-1,-1);

player GameState::get_point(Coordinate point)
{
  return board_state[point.y][point.x];
}

void GameState::set_point(Coordinate point, player colour)
{
  board_state[point.y][point.x] = colour;
}

bool GameState::point_on_board(Coordinate coordinate)
{
  if (coordinate.x < 0 || coordinate.x >= BOARD_SIZE ||
      coordinate.y < 0 || coordinate.y >= BOARD_SIZE)
  {
    return false;
  }
  return true;
}

list_of_points GameState::get_adjacent_points(Coordinate coordinate)
{
  int x = coordinate.x;
  int y = coordinate.y;
  list_of_points adjacent_points;
  adjacent_points.reserve(4);
  Coordinate adjacent_point;
  
  // Left
  adjacent_point.set(x-1, y);
  if (point_on_board(adjacent_point))
  {
    adjacent_points.push_back(adjacent_point);
  }

  // Right
  adjacent_point.set(x+1, y);
  if (point_on_board(adjacent_point))
  {
    adjacent_points.push_back(adjacent_point);
  }
  
  // Up
  adjacent_point.set(x, y-1);
  if (point_on_board(adjacent_point))
  {
    adjacent_points.push_back(adjacent_point);
  }

  // Down
  adjacent_point.set(x, y+1);
  if (point_on_board(adjacent_point))
  {
    adjacent_points.push_back(adjacent_point);
  }

  return adjacent_points;
}

list_of_points GameState::get_diagonal_points(Coordinate coordinate)
{
  int x = coordinate.x;
  int y = coordinate.y;
  list_of_points diagonal_points;
  diagonal_points.reserve(4);
  Coordinate diagonal_point;
  
  // Top left
  diagonal_point.set(x-1, y-1);
  if (point_on_board(diagonal_point))
  {
    diagonal_points.push_back(diagonal_point);
  }

  // Top right
  diagonal_point.set(x+1, y-1);
  if (point_on_board(diagonal_point))
  {
    diagonal_points.push_back(diagonal_point);
  }
  
  // Bottom left
  diagonal_point.set(x-1, y+1);
  if (point_on_board(diagonal_point))
  {
    diagonal_points.push_back(diagonal_point);
  }

  // Bottom right
  diagonal_point.set(x+1, y+1);
  if (point_on_board(diagonal_point))
  {
    diagonal_points.push_back(diagonal_point);
  }

  return diagonal_points;
}

int GameState::liberties_on_group(Coordinate coordinate)
{
  list_of_points liberties;
  player colour = get_point(coordinate);
  Coordinate current_stone;
  list_of_points checked_stones, stones_to_check;
  stones_to_check.push_back(coordinate);

  while (!stones_to_check.empty())
  {
    current_stone = stones_to_check.back();
    stones_to_check.pop_back();
    checked_stones.push_back(current_stone);
    list_of_points adjacent_points = get_adjacent_points(current_stone);
    for (list_of_points::iterator point = adjacent_points.begin();
         point != adjacent_points.end();
         point++)
    {
      if (get_point(*point) == EMPTY)
      {
        if (std::find(liberties.begin(), liberties.end(), *point)==liberties.end())
        {
          liberties.push_back(*point);
        }
      }
      else if (get_point(*point) == colour)
      {
        if ((std::find(checked_stones.begin(), checked_stones.end(), *point)==checked_stones.end())
         && (std::find(stones_to_check.begin(), stones_to_check.end(), *point)==stones_to_check.end()))
        {
          stones_to_check.push_back(*point);
        }
      }
    }
  }
  return liberties.size();
}

int GameState::remove_captures(Coordinate point)
{
  int num_captures = 0; // Needed to check for ko.
  player colour = get_point(point);
  Coordinate current_point;
  list_of_points points_to_check;
  points_to_check.push_back(point);
  while (!points_to_check.empty())
  {
    current_point = points_to_check.back();
    points_to_check.pop_back();
    set_point(current_point, EMPTY);
    // Re-add this point to the list of possible moves.
    possible_moves.push_back(current_point);
    num_captures++;
    list_of_points adjacents = get_adjacent_points(current_point);
    for (list_of_points::iterator point = adjacents.begin();
         point != adjacents.end();
         point++)
    {
      if (get_point(*point) == colour)
      {
        points_to_check.push_back(*point);
      }
    }
  }
  return num_captures;
}

bool GameState::is_suicide(Coordinate move)
{
  set_point(move,to_play);
  int liberties = liberties_on_group(move);
  set_point(move, EMPTY);
  if (liberties == 0) { return true; }
  else { return false; }
}

void GameState::place_handicap_stones(int handicap)
{
  Coordinate point;
  for (int i = 0; i < handicap; i++)
  {
    point = handicap_points[i];
    set_point(point, BLACK);
  }
  play_move(pass);
  komi = handicap;
}

void GameState::play_move(Coordinate move)
{
  ko.set(-1,-1);
  Coordinate pass(-1,-1);
  if (move == pass)
  {
    if (one_pass) { game_over = true; } // Both players have now passed so the game ends.
    else { one_pass = true; }
  }
  else
  {
    possible_moves.erase(std::remove(possible_moves.begin(),
                                     possible_moves.end(),
                                     move),
                         possible_moves.end()); 
    one_pass = false;
    list_of_points groups_captured;
    list_of_points adjacent_points = get_adjacent_points(move);
    for (list_of_points::iterator point = adjacent_points.begin();
         point != adjacent_points.end();
         point++)
    {
      if (get_point(*point) == other_player)
      {
        if (liberties_on_group(*point) == 1)
        {
          groups_captured.push_back(*point);
        }
      }
    }
    set_point(move, to_play);
    int num_captures = 0; // Needed to check for ko.

    for (list_of_points::iterator group = groups_captured.begin();
         group != groups_captured.end();
         group++)
    {
      if (get_point(*group) != EMPTY)
      {
        num_captures += remove_captures(*group);
      }
    }

    if (num_captures == 1 && liberties_on_group(move) == 1)
    {
      list_of_points adjacent_points = get_adjacent_points(move);
      for (list_of_points::iterator point = adjacent_points.begin();
           point != adjacent_points.end();
           point++)
      {
        if (get_point(*point) == EMPTY)
        // The empty adjacent point is the point where the capture has
        // just been made, so this would become the ko point.
        {
          ko = *point;
        }
      }
      for (list_of_points::iterator point = adjacent_points.begin();
           point != adjacent_points.end();
           point++)
      {
        if (get_point(*point) == to_play)
        // If the stone just played is part of a group, then that group
        // can legally be captured by playing on the empty adjacent point
        // and so there is no ko.
        {
          ko.set(-1,-1);
        }
      }
    }
  }
  if (to_play == BLACK)
  {
    to_play = WHITE;
    other_player = BLACK;
  }
  else
  {
    to_play = BLACK;
    other_player = WHITE;
  }
  game_record.push_back(move);
}

bool GameState::is_capture(Coordinate move)
{
  list_of_points adjacent_points = get_adjacent_points(move);
  for (list_of_points::iterator point = adjacent_points.begin();
       point != adjacent_points.end();
       point++)
  {
    if (get_point(*point) == other_player)
    {
      if (liberties_on_group(*point) == 1)
      {
        return true;
      }
    }
  }
  return false;
}

is_legal_responses GameState::is_legal(Coordinate move)
{
  Coordinate pass(-1,-1);
  if (move == pass)
  {
    return LEGAL_MOVE;
  }
  if (!point_on_board(move))
  {
    return POINT_NOT_ON_BOARD;
  }
  else if (get_point(move) != EMPTY)
  {
    return POINT_OCCUPIED;
  }
  // Check ko rule here.
  if (move == ko)
  {
    return KO_POINT;
  }

  // Check for capture. If a capture has been made, then the move is legal.
  // If not, then check if the move is suicide.
  bool capture_made = is_capture(move);
  if (capture_made == false)
  {
    if (is_suicide(move))
    {
      return SUICIDE;
    }
  }
  return LEGAL_MOVE;
}

bool GameState::is_eye(Coordinate point, player eye_colour)
{
  if (get_point(point) != EMPTY)
  {
    return false;
  }
  player colour = eye_colour;
  list_of_points adjacent_points = get_adjacent_points(point);
  if (colour == EMPTY)
  // No colour specified, so first determine whose eye the point potentially belongs to.
  {
    for (list_of_points::iterator adj = adjacent_points.begin(); adj != adjacent_points.end(); adj++)
    {
      if (colour == EMPTY)
      {
        if (get_point(*adj) != EMPTY)
        {
          colour = get_point(*adj);
        }
      }
    }
  }
  // No adjacent stones.
  if (colour == EMPTY) { return false; }
  
  for (list_of_points::iterator adj = adjacent_points.begin(); adj != adjacent_points.end(); adj++)
  {
    if (get_point(*adj) != colour)
    {
      // Adjacent points empty or wrong colour.
      return false;
    }
  }

  list_of_points unoccupied_diagonals;
  list_of_points diagonal_points = get_diagonal_points(point);
  for (list_of_points::iterator diag = diagonal_points.begin();
       diag != diagonal_points.end();
       diag++)
  {
    if (get_point(*diag) != colour)
    {
      unoccupied_diagonals.push_back(*diag);
    }
  }

  if (diagonal_points.size() == 4)
  // If the point is not on the edge of the board, then at least 3 out of the 4
  // diagonal points must be occupied.
  {
    if (unoccupied_diagonals.size() <= 1)
    {
      return true;
    }
    else if (unoccupied_diagonals.size() == 2)
    {
      if (connected_half_eyes(colour, unoccupied_diagonals))
      {
        return true;
      }
    }
  }
  else if (diagonal_points.size() == 2)
  // If the point is on the edge of the board, then both diagonal points must be occupied.
  {
    if (unoccupied_diagonals.size() == 0)
    {
      return true;
    }
    else if (unoccupied_diagonals.size() == 1)
    {
      if (connected_half_eyes(colour, unoccupied_diagonals))
      {
        return true;
      }
    }
  }
  else
  // If the point is on the corner of the board, then the diagonal point must be occupied.
  {
    if (unoccupied_diagonals.size() == 0)
    {
      return true;
    }
    else
    {
      if (connected_half_eyes(colour, unoccupied_diagonals))
      {
        return true;
      }
    }
  }
  return false;
}

bool GameState::connected_half_eyes(player colour, list_of_points unoccupied_diagonals)
{
  for (list_of_points::iterator point = unoccupied_diagonals.begin();
       point != unoccupied_diagonals.end();
       point++)
  {
    list_of_points adjacent_points = get_adjacent_points(*point);
    bool possible_eye = true;
    if (get_point(*point) != EMPTY)
    {
      possible_eye = false;
    }
    // First check that adjacent points are occupied.
    for (list_of_points::iterator adj = adjacent_points.begin(); adj != adjacent_points.end(); adj++)
    {
      if (get_point(*adj) != colour)
      {
        // Adjacent points empty or wrong colour.
        possible_eye = false;
      }
    }

    if (possible_eye)
    {
      // If all adjacent points are occupied, check how many diagonal points are occupied.
      list_of_points diagonal_points = get_diagonal_points(*point);
      int diagonals_occupied = 0;
      for (list_of_points::iterator diag = diagonal_points.begin();
           diag != diagonal_points.end();
           diag++)
      {
        if (get_point(*diag) == colour)
        {
          diagonals_occupied++;
        }
      }

      if (diagonal_points.size() == 4)
      // If the point is not on the edge of the board, then at least 2 out of the 4
      // diagonal points must be occupied for the point to be a false eye
      {
        if (diagonals_occupied >= 2)
        {
          return true;
        }
      }
      else if (diagonal_points.size() == 2)
      // If the point is on the edge of the board, then at leastone of the diagonal points
      // must be occupied.
      {
        if (diagonals_occupied >= 1)
        {
          return true;
        }
      }
      else
      // If the point is on the corner of the board, then the point must be a false eye.
      {
          return true;
      }
    }
  }
  return false;
}

bool GameState::game_finished()
{
  Coordinate point;
  for (int y = 0; y < BOARD_SIZE; y++)
  {
    for (int x = 0; x < BOARD_SIZE; x++)
    {
      point.set(x,y);
      if (get_point(point) == EMPTY)
      {
        if (!is_eye(point, EMPTY)) { return false; }
      }
    }
  }
  return true;
}

int GameState::score_game()
{
  Coordinate coordinate;
  int black_score = 0;
  int white_score = komi;
  for (int y = 0; y < BOARD_SIZE; y++)
  {
    for (int x = 0; x < BOARD_SIZE; x++)
    {
      coordinate.set(x,y);
      if (get_point(coordinate) == BLACK) { black_score++; }
      else if (get_point(coordinate) == WHITE) { white_score++; }
      else
      {
        list_of_points adjacent_points = get_adjacent_points(coordinate);
        if (get_point(adjacent_points.front()) == BLACK)
        {
          black_score++;
        }
        else
        {
          white_score++;
        }
      }
    }
  }
  // std::cout << "Black: " << black_score << std::endl;
  // std::cout << "White: " << white_score << std::endl;
  return black_score - white_score;
}

void GameState::print()
{
  Coordinate coordinate;
  std::cout << "To play: " << to_play << std::endl;
  std::cout << "Ko point: ";
  ko.print();
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      coordinate.set(col,row);
      std::cout << get_point(coordinate);
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void GameState::print_game_record()
{  
  for (list_of_points::iterator move = game_record.begin();
       move != game_record.end();
       move++)
  {
    move->print();
  }
}