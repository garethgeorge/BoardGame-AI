#ifndef __BOARD_H_
#define __BOARD_H_

#include <stdint.h>
#include <string>
#include <cassert>

namespace chess {

typedef int8_t Piece;
typedef int8_t Position;
typedef int32_t Player;
typedef int Score;

const int BOARD_SPACES = 64;
const int BOARD_DIM = 8;

const Piece PIECE_EMPTY = 0;
const Piece PIECE_NULL = INT8_MAX;
const Piece PIECE_PAWN = 1;
const Piece PIECE_KNIGHT = 2;
const Piece PIECE_BISHOP = 3;
const Piece PIECE_ROOK = 4;
const Piece PIECE_KING = 5;
const Piece PIECE_QUEEN = 6;

/*
	get piece values
*/
inline int pieceGetValue(Piece piece) {
	switch (piece) {
		case PIECE_PAWN:
			return 1;
		case PIECE_KNIGHT:
			return 3;
		case PIECE_BISHOP:
			return 3;
		case PIECE_ROOK:
			return 5;
		case PIECE_QUEEN:
			return 9;
		case PIECE_KING:
			return 1000;
		case PIECE_NULL:
			return -1;
		default:
			return 0;
	}
}

/*
	get piece letters, also very fast
*/

inline char pieceGetLetter(Piece piece) {
	switch (piece) {
		case PIECE_PAWN:
			return 'P';
		case PIECE_KNIGHT:
			return 'N';
		case PIECE_BISHOP:
			return 'B';
		case PIECE_ROOK:
			return 'R';
		case PIECE_QUEEN:
			return 'Q';
		case PIECE_KING:
			return 'K';
		case PIECE_NULL:
			return '-';
		case PIECE_EMPTY:
			return ' ';
		default:
			return '?';
	}
}

/*
	Move
	stores a single move in the game
	has room for 4 positions to change their pieces
	usage:
		- call apply once to apply it
		- call apply again to revert back to original state
		- be aware apply modifies the move!
*/
struct Board;
struct Move {
	struct PiecePosPair {
		Position index;
		Piece piece;
	};
	PiecePosPair changes[4]; // up to 4 changes can be made

	Move();
	Move(const Board* board, Position p1, Position p2);

	void apply(Board* board);
	Score score(Board* board);

	inline bool isNull() {
		return changes[0].index == -1;
	}

	std::string toString();
};


/*
	Board
	stores the current state of the game
	thats about it
*/
struct Board {
	Piece pieces[64];

	Board();
	~Board() {};

	inline Piece getPieceAt(int index) const {
		return pieces[index];
	}

	inline void setPieceAt(int index, Piece piece) {
		pieces[index] = piece;
	}

	Score getScore();

	template<typename T>
	static inline T indexToX(T index) { return index % BOARD_DIM; };
	template<typename T>
	static inline T indexToY(T index) { return index / BOARD_DIM; }; 
	template<typename T>
	static inline T xyToIndex(T x, T y) { return x + y * BOARD_DIM; };

	void print() const;
};

template<class STORE> 
void generateMoves(Board* board, Player player, STORE& store);

struct MoveCache { };

struct MoveIterator : public MoveCache {
	int moveCount;
	Move moves[128];

	MoveIterator(Board* board, Player player) : moveCount(0) {
		generateMoves<MoveIterator>(board, player, *this);
	}

	inline bool getNext(Move& move) {
		if (moveCount == 0) {
			move = moves[--moveCount];	
			return false;
		}
		else {
			move = moves[--moveCount];
			return true;
		}
	}

	inline void put(const Move move) {
		moves[moveCount++] = move;
	}
};

/*
	move inline implementations
*/
inline Move::Move() {
	changes[0].index = -1;
}

inline Move::Move(const Board* board, Position p1, Position p2) {
	changes[0].index = p1;
	changes[0].piece = PIECE_EMPTY;
	changes[1].index = p2;
	changes[1].piece = board->getPieceAt(p1);
	changes[2].index = -1;
}

inline void Move::apply(Board* board) {
	for (int i = 0; i < sizeof(changes) / sizeof(PiecePosPair); ++i) {
		if (changes[i].index < 0)
			break;
		Piece old = board->getPieceAt(changes[i].index);
		board->setPieceAt(changes[i].index, changes[i].piece);
		changes[i].piece = old;
	}
}

inline Score Move::score(Board* board) {
	apply(board);
	Score score = board->getScore();
	apply(board);
	return score;
}



}



#endif
