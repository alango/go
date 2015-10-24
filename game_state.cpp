#include <iostream>
#include <algorithm>
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
			board_state[row][col] = 0;
    }
  }
  to_play = 1;
  other_player = 2;
  ko.x = -1;
  ko.y = -1;
}

GameState::~GameState() {};

Coordinate GameState::request_move()
{
	Coordinate coordinate;
	std::cout << "Enter x co-ordinate:" << std::endl;
	std::cin >> coordinate.x;
	std::cout << "Enter y co-ordinate:" << std::endl;
	std::cin >> coordinate.y;
	return coordinate;
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

bool GameState::point_unoccupied(Coordinate coordinate)
{
	if (!point_on_board(coordinate))
	{
	  return false;
	}
	if (board_state[coordinate.y][coordinate.x] != 0)
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

int GameState::liberties_on_group(Coordinate coordinate)
{
	list_of_points liberties;
	int colour = board_state[coordinate.y][coordinate.x];
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
	int colour = board_state[point.y][point.x];
  Coordinate current_point;
  list_of_points points_to_check;
  points_to_check.push_back(point);
	while (!points_to_check.empty())
	{
    current_point = points_to_check.back();
    points_to_check.pop_back();
    board_state[current_point.y][current_point.x] = 0;
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
	board_state[move.y][move.x] = 0;
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
		if (board_state[group->y][group->x] != 0)
		{
			num_captures += remove_captures(*group);
		}
	}

	if (groups_captured.size() == 1 && num_captures == 1 && liberties_on_group(move) == 1)
	{
		list_of_points adjacent_points = get_adjacent_points(move);
		for (list_of_points::iterator point = adjacent_points.begin(); point != adjacent_points.end(); point++)
		{
			if (board_state[point->y][point->x] == 0)
			{
				ko = *point;
			}
		}
	}
	
	if (to_play == 1)
	{
		to_play = 2;
		other_player = 1;
	}
	else
	{
		to_play = 1;
		other_player = 2;
	}
}

void GameState::human_move()
{
/*	Coordinate move;
	bool capture_made = false;
	bool legal_move = false;
	while (!legal_move)
	{
		legal_move = true;
	  move = request_move();
	  if (!point_on_board(move))
	  {
	  	legal_move = false;
      std::cout << "That is not a point on the board" << std::endl;
	  }
	  else if (!point_unoccupied(move))
	  {
	  	legal_move = false;
	  	std::cout << "There is already a stone on that point" << std::endl;
	  }
	  // Check ko rule here.
	  if (move == ko)
	  {
	  	legal_move = false;
	  	std::cout << "Ko rule: You must play elsewhere first" << std::endl;
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
	  		legal_move = false;
	  		std::cout << "That move is suicide" << std::endl;
	  	}
	  }
	}*/

	Coordinate move;
	bool legal_move = false;
	is_legal_responses is_legal_response;
	while (!legal_move)
	{
		move = request_move();
		is_legal_response = is_legal(move);
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
			std::cout << "There is alread a stone on that point" << std::endl;
		}
		else if (is_legal_response == KO_POINT)
		{
			std::cout << "KO_RULE: You must play elsewhere first" << std::endl;
		}
		else if (is_legal_response == SUICIDE)
		{
			std::cout << "That move is suicide" << std::endl;
		}
	}
	play_move(move);
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