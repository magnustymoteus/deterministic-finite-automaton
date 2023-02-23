//
// Created by gruzi on 23/02/2023.
//

#ifndef TA_OPDRACHT1_DFA_H
#define TA_OPDRACHT1_DFA_H

#include <iostream>
#include <set>
#include <functional>

class State; // forward declaration of State

class DFA {
private:
    std::set<State*> states;
    std::set<State*> final_states;
    State* start_state;
    std::set<std::string> alphabet;
    std::function<State*(State*, std::string)> transition_function;
public:
    std::set<State*> get_states() const {return states;}
    std::set<std::string> get_alphabet() const {return alphabet;}
    std::function<State*(State*, std::string)> get_transition_function() const {return transition_function;}
    std::set<State*> get_final_states() const {return final_states;}
    State* get_start_state() const {return start_state;}

    void set_states(std::set<State*> &new_states) {states=new_states;}
    void set_alphabet(std::set<std::string> &new_alphabet) {alphabet=new_alphabet;}
    void set_transition_function(std::function<State*(State*, std::string)> &new_transition_function)
    {transition_function=new_transition_function;}
    void set_final_states(std::set<State*> &new_final_states) {final_states=new_final_states;}
    void set_start_state(State* &new_start_state) {start_state=new_start_state;}
};


#endif //TA_OPDRACHT1_DFA_H
