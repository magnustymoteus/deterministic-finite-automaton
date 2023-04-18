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

typedef std::map<std::string, bool> FillingColumn;
typedef std::map<std::string, FillingColumn> FillingTable;

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
    void fill_table(FillingTable &table) const;
public:
    // constructors
    DFA(const std::string &relativeFile);
    DFA(const DFA &dfa1, const DFA &dfa2, const bool &intersection);
    DFA(const std::set<State*> new_states, const std::set<State*> new_end_states, State* new_start_state,
        const std::set<char> new_alphabet, const TransitionMap new_transition_map);

    // destructor
    ~DFA();

    // getter headers
    std::set<State*> get_states() const {return states;}
    std::set<char> get_alphabet() const {return alphabet;}
    std::set<State*> get_end_states() const {return end_states;}
    State* get_start_state() const {return start_state;}
    TransitionMap get_transition_map() const {return transition_map;}
    State* get_state_by_name(const std::string &name) const;



    // other
    bool accepts(const std::string&str) const;
    void print() const;
    void printTable() const;
    FillingTable get_table() const;
    DFA minimize() const;
};

bool operator==(const DFA &dfa1, const DFA &dfa2);


#endif //TA_OPDRACHT1_DFA_H
