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

GameState::GameState()
{
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			board_state[row][col] = EMPTY;
    }
  }
  to_play = BLACK;
  other_player = WHITE;
  ko.x = -1;
  ko.y = -1;
}

GameState::~GameState() {};

bool GameState::point_on_board(Coordinate coordinate)
{
  if (coordinate.x < 0 || coordinate.x >= BOARD_SIZE ||
		  coordinate.y < 0 || coordinate.y >= BOARD_SIZE)
	{
		return false;
	}
	return true;
}

bool GameState::point_unoccupied(Coordinate coordinate)
{
	if (!point_on_board(coordinate))
	{
	  return false;
	}
	if (board_state[coordinate.y][coordinate.x] != EMPTY)
	{
		return false;
	}
	return true;
}

list_of_points GameState::get_adjacent_points(Coordinate coordinate)
{
	list_of_points adjacent_points;
	adjacent_points.reserve(4);
  Coordinate adjacent_point;
  
  // Left
  adjacent_point.x = coordinate.x - 1;
  adjacent_point.y = coordinate.y;
  if (point_on_board(adjacent_point))
  {
  	adjacent_points.push_back(adjacent_point);
  }

  // Right
  adjacent_point.x = coordinate.x + 1;
  adjacent_point.y = coordinate.y;
  if (point_on_board(adjacent_point))
  {
  	adjacent_points.push_back(adjacent_point);
  }
  
  // Up
  adjacent_point.x = coordinate.x;
  adjacent_point.y = coordinate.y - 1;
  if (point_on_board(adjacent_point))
  {
  	adjacent_points.push_back(adjacent_point);
  }

  // Down
  adjacent_point.x = coordinate.x;
  adjacent_point.y = coordinate.y + 1;
  if (point_on_board(adjacent_point))
  {
  	adjacent_points.push_back(adjacent_point);
  }

  return adjacent_points;
}

list_of_points GameState::get_diagonal_points(Coordinate coordinate)
{
	list_of_points diagonal_points;
	diagonal_points.reserve(4);
  Coordinate diagonal_point;
  
  // Top left
  diagonal_point.x = coordinate.x - 1;
  diagonal_point.y = coordinate.y - 1;
  if (point_on_board(diagonal_point))
  {
  	diagonal_points.push_back(diagonal_point);
  }

  // Top right
  diagonal_point.x = coordinate.x + 1;
  diagonal_point.y = coordinate.y - 1;
  if (point_on_board(diagonal_point))
  {
  	diagonal_points.push_back(diagonal_point);
  }
  
  // Bottom left
  diagonal_point.x = coordinate.x - 1;
  diagonal_point.y = coordinate.y + 1;
  if (point_on_board(diagonal_point))
  {
  	diagonal_points.push_back(diagonal_point);
  }

  // Bottom right
  diagonal_point.x = coordinate.x + 1;
  diagonal_point.y = coordinate.y + 1;
  if (point_on_board(diagonal_point))
  {
  	diagonal_points.push_back(diagonal_point);
  }

  return diagonal_points;
}

int GameState::liberties_on_group(Coordinate coordinate)
{
	list_of_points liberties;
	player colour = board_state[coordinate.y][coordinate.x];
	Coordinate current_stone;
	list_of_points checked_stones, stones_to_check;
	stones_to_check.push_back(coordinate);

	while (!stones_to_check.empty())
	{
		current_stone = stones_to_check.back();
		stones_to_check.pop_back();
		checked_stones.push_back(current_stone);
		list_of_points adjacent_points = get_adjacent_points(current_stone);
		for (list_of_points::iterator point = adjacent_points.begin(); point != adjacent_points.end(); point++)
		{
			if (point_unoccupied(*point))
			{
				if (std::find(liberties.begin(), liberties.end(), *point)==liberties.end())
				{
					liberties.push_back(*point);
				}
			}
			else if (board_state[point->y][point->x] == colour)
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
	player colour = board_state[point.y][point.x];
  Coordinate current_point;
  list_of_points points_to_check;
  points_to_check.push_back(point);
	while (!points_to_check.empty())
	{
    current_point = points_to_check.back();
    points_to_check.pop_back();
    board_state[current_point.y][current_point.x] = EMPTY;
    num_captures++;
    list_of_points adjacents = get_adjacent_points(current_point);
    for (list_of_points::iterator point = adjacents.begin(); point != adjacents.end(); point++)
    {
    	if (board_state[point->x][point->y] == colour)
    	{
    		points_to_check.push_back(*point);
    	}
    }
	}
	return num_captures;
}

bool GameState::is_suicide(Coordinate move)
{
	board_state[move.y][move.x] = to_play;
	int liberties = liberties_on_group(move);
	board_state[move.y][move.x] = EMPTY;
	if (liberties == 0) { return true; }
	else { return false; }
}

void GameState::play_move(Coordinate move)
{
	list_of_points groups_captured;
	list_of_points adjacent_points = get_adjacent_points(move);
  for (list_of_points::iterator point = adjacent_points.begin(); point != adjacent_points.end(); point++)
  {
  	if (board_state[point->y][point->x] == other_player)
  	{
  		if (liberties_on_group(*point) == 1)
  		{
  			groups_captured.push_back(*point);
  		}
  	}
  }

	board_state[move.y][move.x] = to_play;
	ko.x = -1;
	ko.y = -1;
	int num_captures = 0; // Needed to check for ko.

	for (list_of_points::iterator group = groups_captured.begin(); group != groups_captured.end(); group++)
	{
		if (board_state[group->y][group->x] != EMPTY)
		{
			num_captures += remove_captures(*group);
		}
	}

	if (groups_captured.size() == 1 && num_captures == 1 && liberties_on_group(move) == 1)
	{
		list_of_points adjacent_points = get_adjacent_points(move);
		for (list_of_points::iterator point = adjacent_points.begin(); point != adjacent_points.end(); point++)
		{
			if (board_state[point->y][point->x] == EMPTY)
			{
				ko = *point;
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
}

is_legal_responses GameState::is_legal(Coordinate move)
{
	bool capture_made = false;
  if (!point_on_board(move))
  {
    return POINT_NOT_ON_BOARD;
  }
  else if (!point_unoccupied(move))
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
  list_of_points adjacent_points = get_adjacent_points(move);
  for (list_of_points::iterator point = adjacent_points.begin(); point != adjacent_points.end(); point++)
  {
  	if (board_state[point->y][point->x] == other_player)
  	{
  		if (liberties_on_group(*point) == 1)
  		{
  			capture_made = true;
  		}
  	}
  }
  if (capture_made == false)
  {
  	if (is_suicide(move))
  	{
  		return SUICIDE;
  	}
  }
  return LEGAL_MOVE;
}

bool GameState::is_eye(Coordinate point)
{
	if (!point_unoccupied(point))
	{
		return false;
	}
	list_of_points adjacent_points = get_adjacent_points(point);
	player colour = EMPTY;
	for (list_of_points::iterator adj = adjacent_points.begin(); adj != adjacent_points.end(); adj++)
	{
		if (colour == EMPTY)
		{
			if (board_state[adj->y][adj->x] != EMPTY)
			{
				colour = board_state[adj->y][adj->x];
			}
		}
	}
	
	// No adjacent stones.
	if (colour == EMPTY) { return false; }
	
	for (list_of_points::iterator adj = adjacent_points.begin(); adj != adjacent_points.end(); adj++)
	{
		if (board_state[adj->y][adj->x] != colour)
		{
			// Adjacent points empty or wrong colour.
			return false;
		}
	}


	int diagonals_occupied = 0;
	list_of_points diagonal_points = get_diagonal_points(point);
	for (list_of_points::iterator diag = diagonal_points.begin(); diag != diagonal_points.end(); diag++)
	{
		if (board_state[diag->y][diag->x] == colour)
		{
			diagonals_occupied++;
		}
	}

	if (diagonal_points.size() == 4)
	// If the point is not on the edge of the board, then at least 3 out of the 4
	// diagonal points must be occupied.
	{
		if (diagonals_occupied >= 3) { return true; }
		else { return false; }
	}
	else if (diagonal_points.size() == 2)
	// If the point is on the edge of the board, then both diagonal points must be occupied.
	{
		if (diagonals_occupied == 2) { return true; }
		else { return false; }
	}
	else
	// If the point is on the corner of the board, then the diagonal point must be occupied.
	{
		if (diagonals_occupied == 1) { return true; }
		else { return false; }
	}
}

Coordinate GameState::random_move()
{
	Coordinate move;
	bool move_found = false;
	srand(time(NULL));
	while (!move_found)
	{
		move.x = rand() % (BOARD_SIZE+1);
		move.y = rand() % (BOARD_SIZE+1);
		if ((is_legal(move) == LEGAL_MOVE) && !is_eye(move))
		{
			std::cout << "Found one" << std::endl;
			move_found = true;
		}
	}
	return move;
}

void GameState::print()
{
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			std::cout << board_state[row][col];
    }
    std::cout << std::endl;
  }
}