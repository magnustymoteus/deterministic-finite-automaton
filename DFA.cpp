//
// Created by Patryk Pilichowski (s0220383) on 23/02/2023.
//

#include "DFA.h"
#include "json.hpp"
#include <iomanip>
#include <fstream>
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
bool get_table_element(const FillingTable &table, const std::string &strA, const std::string &strB) {
    if(table.find(strA) == table.end()) {
        assert(table.find(strB) != table.end());
        return table.find(strB)->second.find(strA)->second;
    }
    assert(table.find(strA) != table.end());
    return table.find(strA)->second.find(strB)->second;
}
void set_table_element(FillingTable &table, const std::string &strA, const std::string &strB) {
    if(table.find(strA) == table.end()) {
        table.find(strB)->second.find(strA)->second = true;
    }
    else if(table.find(strA) != table.end()){
        table.find(strA)->second.find(strB)->second = true;
    }
}
void DFA::fill_table(FillingTable &table) const {
    bool new_distinguishable;
    do {
        new_distinguishable = false;
        for(const auto &currentRow : table) {
            for(const auto &currentCol : currentRow.second) {
                if(!currentCol.second) {
                    for(const char current_symbol : alphabet) {
                        const std::string strA = transition_map.at(get_state_by_name(currentRow.first)).
                                at(current_symbol)->get_naam();
                        const std::string strB = transition_map.at(get_state_by_name(currentCol.first)).
                                at(current_symbol)->get_naam();
                        if(strA != strB) {
                            const bool distinguishable = get_table_element(table, strA, strB);
                            if (distinguishable) {
                                new_distinguishable = true;
                                set_table_element(table, currentRow.first, currentCol.first);
                                break;
                            }
                        }
                    }
                }
            }
        }
    } while(new_distinguishable);
}
FillingTable DFA::get_table() const {
    FillingTable table;
    std::vector<std::string> state_names;
    for(const State* const &current_state : states) {
        state_names.push_back(current_state->get_naam());
    }
    std::sort(state_names.begin(), state_names.end(), std::less<>());
   for(auto str_iter = next(state_names.begin()); str_iter != state_names.end(); str_iter++) {
        const std::string &current_naam = *str_iter;
        State* current_state = get_state_by_name(current_naam);
        assert(current_state != nullptr);
        for(auto iter = state_names.begin(); *iter != current_naam; iter++) {
            State* iter_state = get_state_by_name(*iter);
            assert(iter_state != nullptr);
            const bool distinguishable = current_state->get_isAccepting() ^ iter_state->get_isAccepting();
            table[current_naam][*iter] = distinguishable;
        }
    }
    fill_table(table);
    return table;
}
void DFA::printTable() const {
    const FillingTable &table = get_table();
    for(const auto &currentRow : table) {
        std::cout << currentRow.first;
        for(const auto &currentCol : currentRow.second) {
            std::cout << "\t" << ((currentCol.second) ? 'X' : '-');
        }
        std::cout << "\n";
    }
    for(const auto &currentCol : prev(table.end())->second) {
        std::cout << "\t" << currentCol.first;
    }
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

DFA::~DFA() {
    for(State* current_state : states) {
        delete current_state;
    }
}
std::string get_product_name(const std::string &name1, const std::string &name2) {
    return "("+name1+","+name2+")";
}
State* DFA::make_product_state(const State* const state1, const State* const state2, const bool &intersection) {
    const bool is_starting = state1->get_isStarting() && state2->get_isStarting();
    const bool is_accepting = (intersection) ?
            state1->get_isAccepting() && state2->get_isAccepting() :
            state1->get_isAccepting() || state2->get_isAccepting();
    auto result = new State(get_product_name(state1->get_naam(), state2->get_naam()),
                 is_starting, is_accepting);
    states.insert(result);
    if (result->get_isStarting()) start_state = result;
    if (result->get_isAccepting()) end_states.insert(result);
    return result;
}
State* DFA::find_or_make_product(const State *const state1, const State *const state2, const bool &intersection) {
    State* search_result = get_state_by_name(get_product_name(state1->get_naam(), state2->get_naam()));
    State* product_state = (search_result != nullptr) ? search_result : make_product_state(state1, state2, intersection);
    return product_state;
}
void DFA::make_product_transition_map(State *const &dfa1_state, State *const &dfa2_state,
                                      const TransitionMap &dfa1_tm, const TransitionMap &dfa2_tm,
                                      const bool &intersection) {
    State* product_state = find_or_make_product(dfa1_state, dfa2_state, intersection);
    if(transition_map.find(product_state) == transition_map.end()) {
        transition_map.insert({product_state, {}});
        for(const char currentSymbol : alphabet) {
            State* dfa1_next_state = dfa1_tm.at(dfa1_state).at(currentSymbol);
            State* dfa2_next_state = dfa2_tm.at(dfa2_state).at(currentSymbol);
            State* next_product_state = find_or_make_product(dfa1_next_state, dfa2_next_state, intersection);

            transition_map.at(product_state).insert({currentSymbol, next_product_state});
            make_product_transition_map(dfa1_next_state, dfa2_next_state,
                                        dfa1_tm, dfa2_tm, intersection);
        }
    }
}
DFA::DFA(const DFA &dfa1, const DFA &dfa2, const bool &intersection) {
    if(dfa1.get_alphabet() != dfa2.get_alphabet())
        throw std::invalid_argument("The DFA's don't have the same alphabet!");
    alphabet = dfa1.alphabet;

    State* const dfa1_currentState = dfa1.get_start_state();
    State* const dfa2_currentState = dfa2.get_start_state();
    const TransitionMap & dfa1_transition_map = dfa1.get_transition_map();
    const TransitionMap & dfa2_transition_map = dfa2.get_transition_map();

    start_state = make_product_state(dfa1_currentState, dfa2_currentState, intersection);

    make_product_transition_map(dfa1_currentState, dfa2_currentState,
                                dfa1_transition_map, dfa2_transition_map, intersection);
    validate();
}

DFA::DFA(const std::string &relativeFile) {
    std::ifstream input(relativeFile);
    nlohmann::json data = nlohmann::json::parse(input);
    for(auto &elem : data["alphabet"]) {
        alphabet.insert(elem.get<std::string>()[0]);
    }
    for(auto &elem : data["states"]) {
        auto new_state = new State(elem["name"].get<std::string>());
        if (elem["starting"].get<bool>()) {
            new_state->set_isStarting(true);
            start_state=new_state;
        }
        if (elem["accepting"].get<bool>()) {
            new_state->set_isAccepting(true);
            end_states.insert(new_state);
        }
        states.insert(new_state);
    }
    for(auto &elem : data["transitions"]) {
        auto from_state = get_state_by_name(elem["from"].get<std::string>());
        auto to_state = get_state_by_name(elem["to"].get<std::string>());
        char curr_input = elem["input"].get<std::string>()[0];

        assert(from_state != nullptr);
        assert(to_state != nullptr);

        if(transition_map.find(from_state) == transition_map.end()) {
            std::map<char, State*> new_map = {{curr_input, to_state}};
            transition_map.insert({from_state, new_map});
        }
        else {
            transition_map.at(from_state).insert({curr_input, to_state});
        }
    }
    validate();
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
State* DFA::get_start_state() const {
    return start_state;
}
TransitionMap DFA::get_transition_map() const {
    return transition_map;
}
State* DFA::get_state_by_name(const std::string &name) const {
    for(auto &current_state : states) {
        if(current_state->get_naam() == name) return current_state;
    }
    return nullptr;
}

// other
bool DFA::accepts(const std::string &str) const {
    State* current_state = start_state;
    for(char current_char : str) {
        current_state = transition_map.at(current_state).at(current_char);
    }
    return is_element_of(current_state, end_states);
}
void DFA::print() const {
    nlohmann::json j;
    j["type"] = "DFA";
    j["alphabet"] = {};
    j["states"] = {};
    j["transitions"] = {};
    for(const char currentChar: alphabet) {
        j["alphabet"].push_back(std::string(1,currentChar));
    }
    for(State* currentState : states) {
        j["states"].push_back({{"name", currentState->get_naam()}, {"starting", currentState->get_isStarting()},
                               {"accepting", currentState->get_isAccepting()}});
    }
    for(Transitions currentTransitions : transition_map) {
        for(auto currentTransition : currentTransitions.second) {
            j["transitions"].push_back({{"from", currentTransitions.first->get_naam()},
                                        {"to", currentTransition.second->get_naam()},
                                        {"input", std::string(1, currentTransition.first)}});
        }
    }
    std::cout << std::setw(4) << j << std::endl;
}