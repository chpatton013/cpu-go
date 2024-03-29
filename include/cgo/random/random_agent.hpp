#pragma once

#include <cgo/base/agent.hpp>
#include <cgo/base/state.hpp>

namespace cgo {
namespace random {

class RandomAgent : public base::Agent {
public:
   RandomAgent(base::Marker marker);
   virtual ~RandomAgent();

   base::Move makeMove(base::State& state,
    const boost::optional< base::Predecessor >& predecessor);
};

} // namespace random
} // namespace cgo
