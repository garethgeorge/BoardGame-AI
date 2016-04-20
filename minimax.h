#ifndef __MINIMAX_H_
#define __MINIMAX_H_

#include <type_traits>
#include <stdint.h>
#include <cassert>

/*
namespace minimax_concepts {
	template<int color>
	struct MPlayer {
		Player getOpponent() const;
	};

	struct Move {
		// applys or reverts the move!
		void toggle(Board* board); 
	};

	template<class PLAYER>
	struct MMoveIterator {
		typedef Move TransitionType;
		NMoveIterator(Board* board, Player player);

		inline bool getNext(transitionType& move) = 0;
	};

	struct Heuristic {
		// takes the board object pointer and a player object
		// returns the score, heuristics are from one player's perspective.
		// a different class is needed for the other player's perspective.
		// higher values are good for us
		// lower values good for opponent

		static int getScore(Board* board) = 0;
	};
}
*/

namespace minimax {

struct AbstractGameStateBaseClass { }; // needed for static_assert type checking

template<class BOARD, class HEURISTIC, class TRANSITION_ITERATOR, class PLAYER, typename SCORE>
struct AbstractGameState : public AbstractGameStateBaseClass {
	typedef BOARD BoardType;
	typedef HEURISTIC HeuristicType;
	typedef TRANSITION_ITERATOR IteratorType;
	typedef PLAYER PlayerType;
	typedef SCORE ScoreType;
	typedef typename IteratorType::TransitionType TransitionType;

	typedef AbstractGameState<BoardType, HeuristicType, IteratorType, PlayerType, ScoreType> nextTurn;
};

template<class AG, bool maximizing, int depth>
struct Minimax {
	Minimax() {
		static_assert(std::is_base_of<AbstractGameStateBaseClass, AG>::value, "template parameter AG must be a template specialization of AbstractGame.");
	}

	typedef Minimax<typename AG::nextTurn, !maximizing, depth - 1> NextMinimax;

	static typename AG::ScoreType run(typename AG::BoardType* board, typename AG::PlayerType player, typename AG::ScoreType alpha, typename AG::ScoreType beta, typename AG::TransitionType& bestTransition) {
		typename AG::PlayerType nextPlayer = player.getOpponent();
			
		typename AG::IteratorType moveIterator(board, player);
		typename AG::IteratorType::TransitionType transition;
		typename AG::IteratorType::TransitionType trash;

		// todo: template on maximizing vs minimizing!
		if (maximizing) {
			typename AG::ScoreType max = INT_MIN;

			while (moveIterator.getNext(transition)) {
				transition.apply(board);
				typename AG::ScoreType score = NextMinimax::run(board, nextPlayer, alpha, beta, trash);
				transition.apply(board);

				if (score > max) {
					bestTransition = transition;
					max = score;
				}

				if (score > alpha)
					alpha = score;
				if (beta <= alpha)
					break;
			}

			return max;
		} else {
			typename AG::ScoreType min = INT_MAX;

			while (moveIterator.getNext(transition)) {
				transition.apply(board);
				typename AG::ScoreType score = NextMinimax::run(board, nextPlayer, alpha, beta, trash);
				transition.apply(board);

				if (score < min) {
					bestTransition = transition;
					min = score;
				}

				if (score < beta)
					beta = score;
				if (beta <= alpha)
					break;
			}

			return min;
		} 
	}
};

template<class AG, bool maximizing>
struct Minimax<AG, maximizing, 0> {
	Minimax() {
		static_assert(std::is_base_of<AbstractGameStateBaseClass, AG>::value, "template parameter AG must be a template specialization of AbstractGame.");
	}

	static typename AG::ScoreType run(typename AG::BoardType* board, typename AG::PlayerType player, int alpha, int beta, typename AG::TransitionType& trash) {
		return AG::HeuristicType::getScore(board, maximizing ? player : player.getOpponent());
	}
};
	

}
#endif
