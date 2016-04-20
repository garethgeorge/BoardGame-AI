#include <iostream>
#include "chessboard.h"
#include "minimax.h"
#include <unistd.h>

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

template<int capture_threshold>
struct ChessHeuristic {
	inline static int getScore(chess::Board* board, ChessPlayer player) {
		return board->getScore() * player.player;
	}

	inline static bool shouldSearchDeeper(chess::Board* boardA, chess::Board* boardB) {
		int pieceCountOriginal = 0;
		int pieceCountNow = 0;
		for (int i = 0; i < chess::BOARD_SPACES; ++i) {
			if (boardA->getPieceAt(i) != 0)
				pieceCountOriginal++;
			if (boardB->getPieceAt(i) != 0)
				pieceCountNow++;
		}

		return pieceCountOriginal - pieceCountNow >= capture_threshold;
	}
};

struct ChessMoveIterator {
	chess::MoveIterator moveIterator;
	inline ChessMoveIterator(chess::Board* board, ChessPlayer player) : moveIterator(board, player.player) { };
	inline bool getNext(chess::Move& move) { return moveIterator.getNext(move); };
	typedef chess::Move TransitionType; // for compatability with minimax.h
};

typedef minimax::AbstractGame<chess::Board, ChessHeuristic<2>, ChessMoveIterator, ChessPlayer, int> ChessGameTypes;
typedef minimax::Minimax<ChessGameTypes, true, std::integral_constant<int, 4>, std::integral_constant<int, 2>, std::integral_constant<int, 1>> ChessGameMinimax;

int main(int argc, const char** args) {
	cout << "Chess Engine v2 by Gareth George" << endl;

	ChessPlayer player1(1);
	ChessPlayer player2(-1);
	chess::Board board;

	int moveCount = 0;
	while (true) {
		std::cout << "Move #" << ++moveCount << " @ PLAYER 1" << std::endl;
		chess::Move move;
		ChessGameMinimax::getBestMove(&board, player1, INT_MIN, INT_MAX, move);
		std::cout << "\tmove: " << move.toString() << std::endl;
		assert(!(move.changes[1].piece == 0));
		move.apply(&board);
		board.print();


		std::cout << "Move #" << ++moveCount << " @ PLAYER 2" << std::endl;
		ChessGameMinimax::getBestMove(&board, player2, INT_MIN, INT_MAX, move);
		std::cout << "\tmove: " << move.toString() << std::endl;
		assert(!(move.changes[1].piece == 0));
		move.apply(&board);
		board.print();

	}

	cout << "Done, shutdown." << endl;
}
