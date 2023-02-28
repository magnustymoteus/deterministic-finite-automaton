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


public:
    State(const std::string &naam) : naam(naam) {}
    std::string get_naam() const {return naam;}

    void set_naam(const std::string &new_naam) {naam=new_naam;}
};


#endif //TA_OPDRACHT1_STATE_H
