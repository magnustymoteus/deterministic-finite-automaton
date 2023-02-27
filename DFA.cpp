//
// Created by Patryk Pilichowski (s0220383) on 23/02/2023.
//

#include "DFA.h"

DFA::DFA(const std::set<State*> &states, const std::set<State*> &end_states, State* &start_state,
         const std::set<char> &alphabet) : states(states), end_states(end_states), start_state(start_state),
         alphabet(alphabet) {
    for(State* current_state : end_states) {
        current_state->set_is_end_state(true);
    }
    start_state->set_is_start_state(true);
}

DFA::~DFA() {
    for(State* current_state : states) {
        delete current_state;
    }
}

// getters implementation
std::set<State*> DFA::get_states() const {
    return states;
}
std::set<char> DFA::get_alphabet() const {
    return alphabet;
}
std::set<State*> DFA::get_end_states() const {
    return end_states;
}
const State* DFA::get_start_state() const {
    return start_state;
}
std::map<std::pair<State*, char>, State*> DFA::get_transition_map() const {
    return transition_map;
}

// setters implementation
bool DFA::set_transition_map(std::map<std::pair<State *, char>, State *> &new_transition_map) {

    return true;
}

// other
bool DFA::is_transition_valid(std::pair<std::pair<State *, char>, State *> &transition) const {

}
bool DFA::state_is_in_states(State* &state) const {
    return states.find(state) != states.end();
}
bool DFA::char_is_in_alphabet(char &character) const {
    return alphabet.find(character) != alphabet.end();
}