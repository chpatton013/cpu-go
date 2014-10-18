#include <cgo/base/state.hpp>

using namespace cgo::base;

State::State() :
   State(Board())
{}

State::State(const State::State& state) :
   State(state._board)
{}

State::State(const State::Board& board) :
   _board(board)
{}

State::~State() {}

const Marker& State::getMarker(const Position& position) const {
   State::validatePosition(position);

   return this->_board[State::getIndex(position)];
}

std::tuple< int, int > State::getScore() const {
   // For now we are only counting the number of pieces on the board. Eventually
   // we will want to add territory control.
   int whiteScore = 0;
   int blackScore = 0;
   for (int ndx = 0; ndx < BOARD_DIMENSION * BOARD_DIMENSION; ++ndx) {
      if (this->_board[ndx] == white) {
         ++whiteScore;
      } else if (this->_board[ndx] == black) {
         ++blackScore;
      }
   }
   return {whiteScore, blackScore};
}

std::vector< Position > State::getLiberties(Marker marker) {
   State::validatePlayerMarker(marker);

   int libertyIndex = (marker == black) ? 0 : 1;

   std::vector< Position > liberties;

   if (!this->_liberties[libertyIndex]) {
      liberties = this->calculateLiberties(marker);
      this->_liberties[libertyIndex] = liberties;
   }

   return liberties;
}

std::vector< std::tuple< Move, State > > State::getSuccessors(Marker marker,
 boost::optional< std::tuple< Move, State > > predecessor) const {
   State::validatePlayerMarker(marker);

   std::vector< std::tuple< Move, State > > successors;
   for (int row = 0; row < BOARD_DIMENSION; ++row) {
      for (int col = 0; col < BOARD_DIMENSION; ++col) {
         Position position(row, col);

         int index = State::getIndex(position);
         if (this->_board[index] != none) {
            continue;
         }

         // if move is illegal
         //    continue

         Action action = {marker, position};
         successors.push_back({action, State::applyAction(*this, action)});
      }
   }

   successors.push_back({Pass(), *this});

   return successors;
}

/* static */ State State::applyAction(const State& sourceState, const Action& action) {
   Marker marker = action.marker;
   State::validatePlayerMarker(marker);

   Position position = action.position;
   State::validatePosition(position);

   int index = State::getIndex(position);

   Board successorBoard = sourceState._board;

   // Take the position.
   successorBoard[index] = marker;

   // And capture enemies.
   // Unimplemented.

   return State(successorBoard);
}

std::vector< Position > State::calculateLiberties(Marker marker) {
   std::array< bool, BOARD_DIMENSION * BOARD_DIMENSION > hash;
   hash.fill(false);

   std::vector< Position > liberties;

   for (int row = 0; row < BOARD_DIMENSION; ++row) {
      for (int col = 0; col < BOARD_DIMENSION; ++col) {
         unsigned int index = State::getIndex(Position(row, col));

         if (this->_board[index] != marker) {
            continue;
         }

         auto adjacentPositions = State::getAdjacentPositions(Position(row, col));
         for (Position adjPos : adjacentPositions) {
            unsigned int adjIndex = State::getIndex(adjPos);

            if (hash[adjIndex]) {
               continue;
            }

            if (this->_board[adjIndex] != none) {
               continue;
            }

            hash[adjIndex] = true;
            liberties.push_back(adjPos);
         }
      }
   }

   return liberties;
}

/* static */ void State::validateMarker(const Marker& marker) {
   if (marker < first || marker > last) {
      throw State::_invalidMarker;
   }
}

/* static */ void State::validatePlayerMarker(const Marker& marker) {
   State::validateMarker(marker);
   if (marker == none) {
      throw State::_invalidMarker;
   }
}

/* static */ void State::validatePosition(const Position& position) {
   if (position.row < 0 || position.row >= BOARD_DIMENSION ||
    position.col < 0 || position.col >= BOARD_DIMENSION) {
      throw State::_invalidPosition;
   }
}

/* static */ unsigned int State::getIndex(const Position& position) {
   return position.row * BOARD_DIMENSION + position.col;
}

/* static */ Position State::getPosition(unsigned int index) {
   return Position(index / BOARD_DIMENSION, index % BOARD_DIMENSION);
}

/* static */ std::vector< Position > State::getAdjacentPositions(const Position& position) {
   State::validatePosition(position);

   std::vector< Position > adjacents;

   int row = position.row;
   int col = position.col;

   if (position.row > 0) {
      adjacents.push_back(Position(row - 1, col));
   }

   if (position.col > 0) {
      adjacents.push_back(Position(row, col - 1));
   }

   if (position.row < BOARD_DIMENSION - 1) {
      adjacents.push_back(Position(row + 1, col));
   }

   if (position.col < BOARD_DIMENSION - 1) {
      adjacents.push_back(Position(row, col + 1));
   }

   return adjacents;
}

/* static */ State::InvalidMarker State::_invalidMarker;
/* static */ State::InvalidPosition State::_invalidPosition;