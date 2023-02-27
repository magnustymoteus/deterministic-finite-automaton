//
// Created by Patryk Pilichowski (s0220383) on 23/02/2023.
//

#ifndef TA_OPDRACHT1_DFA_H
#define TA_OPDRACHT1_DFA_H

#include <iostream>
#include <set>
#include <map>

#include "State.h"

class DFA {
private:
    const std::set<State*> states;
    const std::set<State*> end_states;
    State* start_state;
    const std::set<char> alphabet;

    std::map<std::pair<State*, char>, State*> transition_map;

    bool state_is_in_states(State* &state) const;
    bool char_is_in_alphabet(char &character) const;
    bool is_transition_valid(std::pair<std::pair<State*, char>, State*> &transition) const;
public:
    // constructors
    DFA(const std::set<State*> &states, const std::set<State*> &end_states, State* &start_state,
        const std::set<char> &alphabet);
    DFA() = default;

    // destructor
    ~DFA();

    // getter headers
    std::set<State*> get_states() const;
    std::set<char> get_alphabet() const;
    std::set<State*> get_end_states() const;
    const State* get_start_state() const;
    std::map<std::pair<State*, char>, State*> get_transition_map() const;

    // setter headers
    bool set_transition_map(std::map<std::pair<State*, char>, State*> &new_transition_map);

};


#endif //TA_OPDRACHT1_DFA_H
