//
// Created by Patryk Pilichowski (s0220383) on 23/02/2023.
//

#ifndef TA_OPDRACHT1_STATE_H
#define TA_OPDRACHT1_STATE_H

#include <iostream>
#include <vector>

class State {
private:
    std::string naam;
    bool is_start_state=false, is_end_state=false;


public:
    State(const std::string &naam) : naam(naam) {}
    std::string get_naam() const {return naam;}
    bool get_is_start_state() const {return is_start_state;}
    bool get_is_end_state() const {return is_end_state;}

    void set_naam(const std::string &new_naam) {naam=new_naam;}
    void set_is_start_state(const bool &new_is_start_state) {is_start_state=new_is_start_state;}
    void set_is_end_state(const bool &new_is_end_state) {is_end_state=new_is_end_state;}
};


#endif //TA_OPDRACHT1_STATE_H
