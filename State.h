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
    bool isAccepting=false;
    bool isStarting=false;

public:
    State(const std::string &naam) : naam(naam) {}
    State(const std::string &naam,  const bool &isStarting, const bool &isAccepting) :
    naam(naam), isAccepting(isAccepting), isStarting(isStarting) {}
    std::string get_naam() const {return naam;}
    bool get_isAccepting() const {return isAccepting;}
    bool get_isStarting() const {return isStarting;}

    void set_isStarting(const bool &new_isStarting) {isStarting=new_isStarting;}
    void set_isAccepting(const bool &new_isAccepting) {isAccepting=new_isAccepting;}
    void set_naam(const std::string &new_naam) {naam=new_naam;}
};


#endif //TA_OPDRACHT1_STATE_H
