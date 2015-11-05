CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
SOURCES = game_state.cpp \
          game.cpp \
          human_player.cpp \
          random_player.cpp \
          mcts_node.cpp \
          test.cpp main.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = go

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	\rm *.o $(EXECUTABLE)
