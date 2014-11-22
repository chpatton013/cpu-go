#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <cgo/montecarlo/montecarlo_agent.hpp>

using namespace std;
using namespace cgo::base;
using namespace cgo::montecarlo;

MonteCarloAgent::MonteCarloAgent(Marker marker) :
   Agent(marker)
{}

/* virtual */ MonteCarloAgent::~MonteCarloAgent() {}

int cur_player = 1;
boost::optional< std::tuple< Move, State > > predecessorPtr;
State m_state;
State tempState;
Marker temp_marker;
static const double UCTK = 0.44;
static const int numSims = 50;

Node* MonteCarloAgent::getBestChild(Node* root) {
   Node* child = root->child;
   Node* best_child = NULL;
   double best_win = -1;
   while (child) {
      Node* siblings = child;
      while (siblings) {
         Position position(siblings->x, siblings->y);
         Action action(this->_marker, position);
         if (siblings->getWinRate() > best_win && m_state.isActionValid(action, predecessorPtr)) {
             best_child = siblings;
             best_win = siblings->getWinRate();
         }
         siblings = siblings->sibling;
      }
      child = child->child;
   }
   return best_child;
}

Node* MonteCarloAgent::UCTSelect(Node* node) {
   Node* res = NULL;
   Node* next = node->child;
   double best_uct = 0;
   while (next != NULL) {
      double uctvalue = 0;
      if (next->visits > 0) {
         double winrate = next->getWinRate();
         double uct = UCTK * sqrt(log(node->visits) / (5 * next->visits));
         uctvalue = winrate + uct;
      }
      else {
         uctvalue = rand();
      }
      if (uctvalue > best_uct) {
         best_uct = uctvalue;
         res = next;
      }
      next = next->sibling;
   }
   return res;
}

int MonteCarloAgent::createChildren(Node* parent, State& state,
 const boost::optional< std::tuple< Move, State > >& predecessor) {
   Node* last = parent;
   for (int i = 0; i < BOARD_SIZE; i++) {
      for (int j = 0; j < BOARD_SIZE; j++) {
         Position position(i, j);
         Action action(this->_marker, position);
         if (state.isActionValid(action, predecessor)) {
            Node* n = new Node(i, j);
            if (last == parent) {
               last->child = n;
            }
            else {
               last->sibling = n;
            }
            last = n;
         }
      }
   }
   return 0;
 }

bool MonteCarloAgent::checkGameOver(State& state, Move move,
 const boost::optional< std::tuple< Move, State > >& predecessor) {
   const Predecessor& predecessorTuple = predecessor.get();
   Move prevMove = std::get< 0 >(predecessorTuple);

   if ((!boost::get<Action>(&move) && !boost::get<Action>(&prevMove)) ||
       state.getSuccessors(this->_marker, predecessor).empty()) { //Both Action is Pass or If no possible moves left
      return true;
   }
   else {
      return false;
   }
}

Move MonteCarloAgent::makeRandomMove(State& state,
 const boost::optional< std::tuple< base::Move, base::State > >& predecessor) {
   int x=0;
   int y=0;
   while (true) {
      x = rand() % BOARD_SIZE;
      y = rand() % BOARD_SIZE;
      Position position(x, y);
      Action action(this->_marker, position);
      if (state.isActionValid(action, predecessor)) {
         break;
      }
      if (state.getSuccessors(this->_marker, predecessor).empty()) {
         x = -1;
         y = -1;
         break;
      }
   }
   if (x >= 0 && y >= 0) {
      Position position(x, y);
      Action actionY(this->_marker, position);
      tempState = State::applyAction(state, actionY);
      return actionY;
   }
   return Pass();
}

int MonteCarloAgent::playRandomGame(State& state,
 const boost::optional< std::tuple< base::Move, base::State > >& predecessor) {
   Move prevAction;
   Marker temp = this->_marker;
   Predecessor pred = predecessor.get();
   State clone = State(state);
   int moveCount = 0;
   do {
      prevAction = makeRandomMove(clone, pred);
      cur_player == 0 ? cur_player = 1 : cur_player = 0;
      this->_marker = (this->_marker == white) ? black : white; // switch players
      pred = std::make_tuple(prevAction, clone);
      moveCount++;
   } while (!checkGameOver(tempState, prevAction, pred) && moveCount < 80); //Allow first move
   this->_marker = temp;
   if (std::get<0>(tempState.getScores()) > std::get<1>(tempState.getScores())) {
      if (cur_player == 0) {
         return cur_player == temp_marker ? 1 : 0;
      }
      else {
         return cur_player == temp_marker ? 0 : 1;
      }
   }
   else {
      if (cur_player == 1) {
         return cur_player == temp_marker ? 1 : 0;
      }
      else {
         return cur_player == temp_marker ? 0 : 1;
      }
   }
}

int MonteCarloAgent::playSimulation(Node* n, State& state,
  const boost::optional< std::tuple< base::Move, base::State > >& predecessor) {
   int randomresult = 0;
   if (n->visits == 0) {
      randomresult = playRandomGame(state, predecessor);
   }
   else {
      if (!n->child) {
         createChildren(n, tempState, predecessor);
      }
      Node *next = UCTSelect(n);

      Position position(next->x, next->y);
      Action action(this->_marker, position);
      tempState = State::applyAction(tempState, action);
      this->_marker = (this->_marker == white) ? black : white;
      State clone = State(tempState);
      Predecessor pred = std::make_tuple(action, clone);
      randomresult = playSimulation(next, clone, pred);
   }
   n->visits += 1;
   n->wins += randomresult;
   return randomresult;
}

Move MonteCarloAgent::makeMove(State& state,
 const boost::optional< std::tuple< Move, State > >& predecessor) {
   srand(time(NULL));
   Node *root = new Node(-1, -1);

   createChildren(root, state, predecessor);
   m_state = state;
   predecessorPtr = predecessor;
   temp_marker = this->_marker;

   for (int i = 0; i < numSims; i++) {
      State clone = State(state);
      cout << "simulation " << i << endl;
      playSimulation(root, clone, predecessor);
   }

   this->_marker = temp_marker;
   Node* n = getBestChild(root);
   Position position(n->x, n->y);
   return Action(this->_marker, position);
}
