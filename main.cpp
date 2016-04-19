#include <iostream>
#include "chessboard.h"
#include "minimax.h"

using namespace std;

// basically just an integer but abstracted a bit
struct ChessPlayer {
	chess::Player player;

	inline ChessPlayer(chess::Player player) : player(player) {
		
	}

	inline ChessPlayer getOpponent() {
		return ChessPlayer(-player);
	}
};

struct ChessHeuristic {
	inline static int getScore(chess::Board* board, ChessPlayer player) {
		return board->getScore() * player.player;
	}
};

struct ChessMoveIterator {
	chess::MoveIterator moveIterator;
	inline ChessMoveIterator(chess::Board* board, ChessPlayer player) : moveIterator(board, player.player) { };
	inline bool getNext(chess::Move& move) { return moveIterator.getNext(move); };
	typedef chess::Move TransitionType; // for compatability with minimax.h
};

typedef minimax::AbstractGameState<chess::Board, ChessHeuristic, ChessMoveIterator, ChessPlayer, int> ChessGameTypes;
typedef minimax::Minimax<ChessGameTypes, true, 5> ChessGameMinimax;

int main(int argc, const char** args) {
	cout << "Chess Engine v2 by Gareth George" << endl;

	ChessPlayer player1(1);
	ChessPlayer player2(-1);
	chess::Board board;

	chess::Move move;
	ChessGameMinimax::run(&board, player1, INT_MIN, INT_MAX, move);

	move.apply(&board);

	chess::Move move2;
	ChessGameMinimax::run(&board, player2, INT_MIN, INT_MAX, move2);

	move2.apply(&board);


	board.print();

	cout << "Done, shutdown." << endl;
}
