#pragma once

#include <cgo/base/agent.hpp>
#include <cgo/base/state.hpp>

namespace cgo {
namespace standardin {

class MonteCarloAgent : public base::Agent {
public:
   MonteCarloAgent(base::Marker marker);
   virtual ~MonteCarloAgent();

   base::Move makeMove(base::State& state,
    const boost::optional< std::tuple< base::Move, base::State > >& predecessor);
};

} // namespace standardin
} // namespace cgo
