#pragma once

#include <array>
#include <set>
#include <tuple>
#include <vector>
#include <boost/optional/optional.hpp>
#include <cgo/base/types.hpp>

namespace cgo {
namespace base {

#ifndef BOARD_DIMENSION
#define BOARD_DIMENSION 9
#endif

class State {
public:
   typedef std::array< Marker, BOARD_DIMENSION * BOARD_DIMENSION > Board;

   State();
   State(const State& state);
   State(const Board& board);
   ~State();

   const Marker& getMarker(const Position& position) const;

   std::tuple< int, int > getScore() const;
   std::vector< Position > getLiberties(Marker marker);
   std::vector< std::tuple< Move, State > > getSuccessors(Marker marker,
    boost::optional< std::tuple< Move, State > > predecessor) const;

   static struct InvalidMarker {} _invalidMarker;
   static struct InvalidPosition {} _invalidPosition;

   static State applyAction(const State& sourceState, const Action& action);

private:
   std::vector< Position > calculateLiberties(Marker marker);

   static void validateMarker(const Marker& marker);
   static void validatePlayerMarker(const Marker& marker);
   static void validatePosition(const Position& position);

   static unsigned int getIndex(const Position& position);
   static Position getPosition(unsigned int index);
   static std::vector< Position > getAdjacentPositions(const Position& position);

   const Board _board;
   boost::optional< std::vector< Position > > _liberties[2];
};

} // namespace base
} // namespace cgo