# a7 - Connect Four

In this week’s assignment you will build a networked “Connect4” application. 

Your networked application will mean two users in different locations can play a Connect4 game together! Users will be able to make moves on the board by sending a request to the server.

Our game server will also be able to crash and recover games by using persistent storage (aka the disk & filesystem). 

You will have to use the POSIX networking API to create a server and client program using sockets. You will also leverage the POSIX file API to open, read, and write files on disk for the purpose of saving and loading an in-progress game. 

# Implementing the Game Logic

The game of connect four is simple. Players take turns placing pieces in a chosen column. When a column is chosen, the piece drops down to the bottom of that column, possibly stacking onto earlier placed pieces.

The goal of the game is to "connect four" of these pieces either vertically, horizontally, or diagonally.

A game in our C code is represented as follows: 

```c
struct game {
    int* board; //pointer to a 7x6 game board
    int turn; //encodes the current color of who moves next 0/1
};
```

## Make the Board

It is up to you how you want to represent your game board. For inspiration, consider how we represent images in a3. 
A standard board for connect4 has 7 columns and 6 rows. 

> [!IMPORTANT]
> Task: Implement the logic that creates a new game in `game.c`.

## Placing Pieces

We now will implement the logic of making a move on our game board.

Consider an example of how the board changes when a `1` is placed in column 1.

```
  1
  |
  v
_ _ _ _ _ _ _
_ _ _ _ _ _ _
_ _ _ _ _ _ _
_ _ _ _ _ _ _
_ _ _ 1 _ 0 0
_ 0 1 1 1 0 0
```
This move results in the `1` landing "on top" of the existing `0` in column 1.
```
_ _ _ _ _ _ _
_ _ _ _ _ _ _
_ _ _ _ _ _ _
_ _ _ _ _ _ _
_ 1 _ 1 _ 0 0
_ 0 1 1 1 0 0
```

Making a move should also update bookkeeping on whose turn it currently is.

> [!IMPORTANT]
> Task: Implement the logic that allows a player to place a piece in a given chosen column in `game.c`.


## Win Detection

> [!IMPORTANT]
> Task: Implement the logic that checks if either player has won the game.

# Adding Networking

## Server Setup

The server has the following functionalities:
- MakeMove (gameID, column, playerID) -> success?
- NewGame -> gameID
- DisplayBoard (gameID) -> boardString

## Client Requests

The client interface will be a CLI interface where clients are allowed to type in their moves. These moves will be transmitted to the server. 
The client can also request a new game board visual which will be printed to the terminal.

# Saving and Loading Games

We want to make our server **fault tolerant** by saving game data to disk. After a move is made, save the game to a file. 

When our server starts up, check for any existing game files and load them into memory.

## Saving a Game to File

After every move made, we should update the saved version of the game's file.


> [!IMPORTANT]
> Task: Implement the logic in `save_game_to_file` that saves a in-memory game struct to a file in the `games/` directory.

## Loading a Game from a File

When your server first runs, in main, the first thing we should do is call `recover_games`. 

This will look through the `games/` directory for any in-progress games and restore them by reading the corresponding files into memory.

> [!IMPORTANT]
> Task: Implement the logic in `recover_games` that loads all found files in the `games/` directory to an in-memory game struct.
