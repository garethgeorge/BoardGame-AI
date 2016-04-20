#include "chessboard.h"
#include "include/termcolor.h"
#include <type_traits>
#include <string>
#include <sstream>

namespace chess {


/*
	Methods for Board
*/
Board::Board() {
	// construct a board

	for (int i = BOARD_SPACES - 1; i >= 0; --i)
		this->pieces[i] = 0;


	for (int i = 0; i < BOARD_DIM; ++i) {
		this->pieces[BOARD_DIM * 1 + i] =  PIECE_PAWN;
		this->pieces[BOARD_DIM * 6 + i] = -PIECE_PAWN;
	}
	this->pieces[3] = PIECE_QUEEN;
	this->pieces[4] = PIECE_KING;
	this->pieces[0] = this->pieces[7] = PIECE_ROOK;
	this->pieces[1] = this->pieces[6] = PIECE_KNIGHT;
	this->pieces[2] = this->pieces[5] = PIECE_BISHOP;

	const int blackOffset = BOARD_DIM * 7;
	this->pieces[blackOffset + 0] = this->pieces[blackOffset + 7] = -PIECE_ROOK;
	this->pieces[blackOffset + 1] = this->pieces[blackOffset + 6] = -PIECE_KNIGHT;
	this->pieces[blackOffset + 2] = this->pieces[blackOffset + 5] = -PIECE_BISHOP;
	this->pieces[blackOffset + 3] = -PIECE_QUEEN;
	this->pieces[blackOffset + 4] = -PIECE_KING;
}

Score Board::getScore() {
	int score = 0;
	for (int i = BOARD_SPACES - 1; i >= 0; --i) {
		if (pieces[i] < 0)
			score -= pieceGetValue(-pieces[i]);
		else if(pieces[i] > 0)
			score += pieceGetValue(pieces[i]);
	}
	return score;
}

void Board::print() const {
	auto& ss = std::cout;
	ss << termcolor::reset << " " << termcolor::grey << termcolor::on_white;
	for (int i = 0; i < BOARD_DIM; ++i) {
		ss << (char) ('a' + i);
	}
	ss << termcolor::reset << std::endl;

	for (int y = 0; y < BOARD_DIM; ++y) {
		ss << termcolor::on_white << termcolor::grey << (y + 1);
		for (int x = 0; x < BOARD_DIM; ++x) {
			int i = y * BOARD_DIM + x;
			Piece p = this->pieces[i];
			if ((i + i / 8) % 2 == 0)
				ss << termcolor::on_grey;
			else
				ss << termcolor::on_cyan;

			if (p != 0) {
				if (p < 0) {
					p = -p;
					ss << termcolor::red;
				} else 
					ss << termcolor::white;

				ss << pieceGetLetter(p);
			} else
				ss << " ";

			ss << termcolor::reset;
		}
		ss << termcolor::reset << std::endl;
	}
}


/*
	move list generation which is heavily templated!
*/

// move validity checks
enum MOVE_VALIDITY {
	ADD, STOP, ADD_STOP
};
struct OnlyIfEmpty {
	//inline bool operator() (const Player player, const Piece piece) {
	//	return piece == 0;
	//}
	static inline MOVE_VALIDITY shouldAdd(const Player player, const Piece piece) {
		return piece == 0 ? ADD : STOP;
	}
};
struct OnlyIfEmptyOrCapture {
	static inline MOVE_VALIDITY shouldAdd(const Player player, const Piece piece) {
		if (piece == 0)
			return ADD;
		else if (piece * player < 0)
			return ADD_STOP;
		else
			return STOP;
	}
};
struct OnlyIfCapture {
	static inline MOVE_VALIDITY shouldAdd(const Player player, const Piece piece) {
		if (piece * player < 0)
			return ADD_STOP;
		else
			return STOP;
	}
};

template<int upper>
struct UpperBoundCheck {
	inline bool operator() (const int val) {
		return val < upper;
	};

	typedef UpperBoundCheck<upper> type;
};
template<int lower>
struct LowerBoundCheck {
	inline bool operator() (const int val) {
		return val >= lower;
	}

	typedef LowerBoundCheck<lower> type;
};
struct NoBoundCheck {
	inline bool operator() (const int val) {
		return true;
	}

	typedef NoBoundCheck type;
};


/*
	move to a position with an offset!
*/
template<class STORE, int dx, int dy, class CanMoveTo>
bool moveTo(Board* board, Player player, int x, int y, STORE& store) {
	typename std::conditional < (dx > 0), UpperBoundCheck<BOARD_DIM>::type, LowerBoundCheck<0>::type >::type xCheck;
	typename std::conditional < (dy > 0), UpperBoundCheck<BOARD_DIM>::type, LowerBoundCheck<0>::type >::type yCheck;

	if (!xCheck(x + dx) || !yCheck(y + dy))
		return false;
	

	const int from = Board::xyToIndex(x, y);
	const int to = Board::xyToIndex(x + dx, y + dy);

	switch (CanMoveTo::shouldAdd(player, board->pieces[to])) {
		case MOVE_VALIDITY::ADD:
			store.put(Move(board, from, to));
			return true;
		case MOVE_VALIDITY::ADD_STOP:
			store.put(Move(board, from, to));
			return false;
		case MOVE_VALIDITY::STOP:
			return false;
	}
}

/*
	move along a vector
*/
template<class STORE, int dx, int dy, class CanMoveTo, int distance>
struct _MoveAlongVector {
	static inline void run(Board* board, Player player, int x, int y, STORE& store) {
		if (moveTo<STORE, dx * distance, dy * distance, CanMoveTo>(board, player, x, y, store))
			return _MoveAlongVector<STORE, dx, dy, CanMoveTo, distance + 1>::run(board, player, x, y, store);
	}
};

template<class STORE, int dx, int dy, class CanMoveTo>
struct _MoveAlongVector<STORE, dx, dy, CanMoveTo, 8> {
	static inline void run(Board* board, Player player, int x, int y, STORE& store) { }
};

template<class STORE, int dx, int dy, class CanMoveTo>
void moveAlongVector(Board* board, Player player, int x, int y, STORE& store) {
	_MoveAlongVector<STORE, dx, dy, CanMoveTo, 1>::run(board, player, x, y, store);
}

template<class STORE>
void generateMoves(Board* board, Player player, int index, STORE& store) {
	int x = Board::indexToX(index);
	int y = Board::indexToY(index);

	Piece p = board->getPieceAt(index) * player;
	
	switch (p) {
		case PIECE_PAWN:

			if (player > 0) {
				if (moveTo<STORE, 0, 1, OnlyIfEmpty>(board, player, x, y, store)) {
					if (y == 1) {
						moveTo<STORE, 0, 2, OnlyIfEmpty>(board, player, x, y, store);
					}
				}
				moveTo<STORE, -1,1, OnlyIfCapture>(board, player, x, y, store);
				moveTo<STORE, 1,1, OnlyIfCapture>(board, player, x, y, store);
			} else {
				if (moveTo<STORE, 0, -1, OnlyIfEmpty>(board, player, x, y, store)) {
					if (y == 6)
						moveTo<STORE, 0, -2, OnlyIfEmpty>(board, player, x, y, store);
				}
				moveTo<STORE, -1,-1, OnlyIfCapture>(board, player, x, y, store);
				moveTo<STORE, 1,-1, OnlyIfCapture>(board, player, x, y, store);
			}

			break ;

		case PIECE_BISHOP:
			
			moveAlongVector<STORE, -1, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE, -1,  1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  1, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  1,  1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			
			break ;

		case PIECE_ROOK:
			
			moveAlongVector<STORE, -1,  0, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  1,  0, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  0, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  0,  1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			
			break ;

		case PIECE_KNIGHT:

			moveTo<STORE, 2,1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, 1,2, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			
			moveTo<STORE, 2,-1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, 1,-2, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			
			moveTo<STORE, -2,1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, -1,2, OnlyIfEmptyOrCapture>(board, player, x, y, store);

			moveTo<STORE, -2,-1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, -1,-2, OnlyIfEmptyOrCapture>(board, player, x, y, store);

			break ;

		case PIECE_QUEEN:
			
			moveAlongVector<STORE, -1,  0, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  1,  0, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  0, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  0,  1, OnlyIfEmptyOrCapture>(board, player, x, y, store);

			moveAlongVector<STORE, -1, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE, -1,  1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  1, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveAlongVector<STORE,  1,  1, OnlyIfEmptyOrCapture>(board, player, x, y, store);

			break;

		case PIECE_KING:

			moveTo<STORE, -1, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, 1, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, -1, 1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, 1, 1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, 0, 1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, 0, -1, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, 1, 0, OnlyIfEmptyOrCapture>(board, player, x, y, store);
			moveTo<STORE, -1, 0, OnlyIfEmptyOrCapture>(board, player, x, y, store);

			break ;

	}
}


template<class STORE> 
void generateMoves(Board* board, Player player, STORE& store) {
	static_assert(std::is_base_of<MoveCache, STORE>::value, "typename STORE is not an instance of a MoveStore (must implement put)");

	for (int i = BOARD_SPACES - 1; i >= 0; --i) {
		if (board->getPieceAt(i) * player > 0) {
			generateMoves(board, player, i, store);
		}
	}
}


// specializations of the template code
template void generateMoves<MoveIterator>(Board*, Player, MoveIterator& store);


// move class
std::string Move::toString() {
	std::stringstream ss;
	ss << "(";
	for (int i = 0; i < sizeof(changes) / sizeof(PiecePosPair); ++i) {
		ss << "[" << (int) (changes[i].index) << ":" << (int) (changes[i].piece) << "],";
	}
	ss << ")";
	return ss.str();
}
	
};