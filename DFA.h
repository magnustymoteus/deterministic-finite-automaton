//
// Created by Patryk Pilichowski (s0220383) on 23/02/2023.
//

#ifndef TA_OPDRACHT1_DFA_H
#define TA_OPDRACHT1_DFA_H

#include <iostream>
#include <set>
#include <map>
#include <fstream>

#include "State.h"

typedef std::map<State*, std::map<char, State*>> TransitionMap;
typedef std::pair<State*, std::map<char, State*>> Transitions;
typedef std::pair<State*, std::pair<char, State*>> Transition;

class DFA {
private:
    std::set<State*> states;
    std::set<State *> end_states;
    State* start_state;
    std::set<char> alphabet;
    TransitionMap transition_map;
    void validate() const;
    State* find_or_make_product(const State* const state1, const State* const state2, const bool &intersection);
    void make_product_transition_map(State* const &dfa1_state, State* const &dfa2_state,
                                     const TransitionMap &dfa1_tm, const TransitionMap &dfa2_tm,
                                     const bool &intersection);
    State* make_product_state(const State* const state1, const State* const state2, const bool &intersection);
public:
    // constructors
    DFA(const std::string &relativeFile);
    DFA(const DFA &dfa1, const DFA &dfa2, const bool &intersection);

    // destructor
    ~DFA();

    // getter headers
    std::set<State*> get_states() const;
    std::set<char> get_alphabet() const;
    std::set<State*> get_end_states() const;
    State* get_start_state() const;
    TransitionMap get_transition_map() const;
    State* get_state_by_name(const std::string &name) const;

    // other
    bool accepts(const std::string &str) const;
    void print() const;
};


#endif //TA_OPDRACHT1_DFA_H
