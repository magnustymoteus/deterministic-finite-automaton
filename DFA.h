//
// Created by Patryk Pilichowski (s0220383) on 23/02/2023.
//

#ifndef TA_OPDRACHT1_DFA_H
#define TA_OPDRACHT1_DFA_H

#include <iostream>
#include <set>
#include <map>

class State; // forward declaration of State

class DFA {
private:
    std::set<State*> states;
    std::set<State*> final_states;
    State* start_state;
    std::set<std::string> alphabet;
    std::map<std::pair<State*, std::set<std::string>>, State*> transition_map;
public:
    std::set<State*> get_states() const {return states;}
    std::set<std::string> get_alphabet() const {return alphabet;}
    std::set<State*> get_final_states() const {return final_states;}
    State* get_start_state() const {return start_state;}
    std::map<std::pair<State*, std::set<std::string>>, State*> get_transition_map() const {return transition_map;}

    void set_states(std::set<State*> &new_states) {states=new_states;}
    void set_alphabet(std::set<std::string> &new_alphabet) {alphabet=new_alphabet;}
    void set_final_states(std::set<State*> &new_final_states) {final_states=new_final_states;}
    void set_start_state(State* &new_start_state) {start_state=new_start_state;}
    void set_transition_map(std::map<std::pair<State*, std::set<std::string>>, State*> &new_transition_map)
    {transition_map=new_transition_map;}
};


#endif //TA_OPDRACHT1_DFA_H
