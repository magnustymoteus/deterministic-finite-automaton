//
// Created by Patryk Pilichowski (s0220383) on 23/02/2023.
//

#include "DFA.h"
#include <cassert>

bool is_subset_of(const std::set<State*> &subset, const std::set<State*> &superset) {
    for(State* current_state1 : subset) {
        if(superset.find(current_state1) == superset.end()) return false;
    }
    return true;
}
bool is_element_of(State* const &element, const std::set<State *> &state_set) {
    return state_set.find(element) != state_set.end();
}
void DFA::init(std::set<State *> &states_arg, std::set<State *> &end_states_arg, State *&start_state_arg,
               std::set<char> &alphabet_arg, TransitionMap &transition_map_arg){
    states = states_arg, end_states = end_states_arg, start_state = start_state_arg, alphabet=alphabet_arg;
    transition_map = transition_map_arg;
    validate();
}

void DFA::validate() const {
    assert(is_subset_of(end_states, states));
    assert(is_element_of(start_state, states));

    TransitionMap transition_map_copy = transition_map;

    for(State* current_state : states) {
        auto found_state = transition_map_copy.find(current_state);
        for(char character : alphabet) {
            assert(found_state != transition_map_copy.end() && "Expected state to be in transition_map");
            auto found_output_state = found_state->second.find(character);
            assert(found_output_state != found_state->second.end() &&
                   "Expected character and state combination to be in transition map");
            found_state->second.erase(found_output_state);
        }
        assert(found_state->second.empty() && "Expected precisely one mapping per unique state and input combination");
        transition_map_copy.erase(found_state);
    }
    assert(transition_map_copy.empty() && "Expected each state in transition map to be legal");
}

DFA::DFA(std::set<State*> &states, std::set<State*> &end_states, State* &start_state,
         std::set<char> &alphabet, TransitionMap &transition_map) : states(states), end_states(end_states),
         start_state(start_state), alphabet(alphabet), transition_map(transition_map) {

    /*  Preconditions:
     * For each unique combination of state and input there exists precisely one output state
     * All symbols re in the alphabet
     * The end states set is a subset of states set
     * The start state is an element of states set
     */
    init(states, end_states, start_state, alphabet, transition_map);
}
DFA::DFA() {
    std::set<char> alphabet_arg = {'0', '1'};

    auto s0 = new State("s0");
    auto s1 = new State("s1");
    auto s2 = new State("s2");

    std::set<State*> states_arg = {s0,s1,s2};
    std::set<State*> end_states_arg = {s0};

    Transitions transitions_s0 = {s0, {{'0', s0}, {'1', s1}}};
    Transitions transitions_s1 = {s1, {{'0', s0}, {'1', s2}}};
    Transitions transitions_s2 = {s2, {{'0', s1}, {'1', s2}}};

    TransitionMap transition_map_arg = {transitions_s0, transitions_s1, transitions_s2};

    init(states_arg, end_states_arg, s0, alphabet_arg, transition_map_arg);
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
TransitionMap DFA::get_transition_map() const {
    return transition_map;
}

// other
bool DFA::accepts(const std::string &str) const {
    State* current_state = start_state;
    for(char current_char : str) {
        current_state = transition_map.at(current_state).at(current_char);
    }
    return is_element_of(current_state, end_states);
}