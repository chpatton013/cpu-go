#pragma once

#include <boost/optional.hpp>
#include <cgo/base/types.hpp>

namespace cgo {
namespace base {

class State;

struct Agent {
   Agent(Marker marker);
   virtual ~Agent();

   virtual Move makeMove(State& state,
    const boost::optional< Predecessor >& predecessor) = 0;

protected:
   Marker _marker;
   Marker _opponentMarker;
};

} // namespace base
} // namespace cgo
