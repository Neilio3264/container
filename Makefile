BIN_DIR = ./bin
OBJS_DIR = ./obj
INCL_DIR = ./include
SRC_DIR = ./src

OBJS	= $(OBJS_DIR)/container.o
SOURCE	= $(SRC_DIR)/container.cpp
HEADER	= 
OUT	= $(BIN_DIR)/container
CC	 = g++
FLAGS	 = -g -c -std=c++17 -Wall
LFLAGS	 =

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

$(OBJS_DIR)/container.o: $(SRC_DIR)/container.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUT)