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
    auto find_result_1 = table.find(strA);
    if(find_result_1 != table.end()) {
        auto find_result_1b = find_result_1->second.find(strB);
        if(find_result_1b != find_result_1->second.end()) return find_result_1b->second;
    }
    auto find_result_2 = table.find(strB);
    if(find_result_2 != table.end()) {
        auto find_result_2b = find_result_2->second.find(strA);
        assert(find_result_2b != find_result_2->second.end());
        return find_result_2b->second;
    }
    return false;
}
void set_table_element(FillingTable &table, const std::string &strA, const std::string &strB) {
    if(table.find(strA) == table.end()) {
        table.find(strB)->second.find(strA)->second = true;
    }
    else if(table.find(strA) != table.end()){
        table.find(strA)->second.find(strB)->second = true;
    }
}
std::set<std::string> get_corresponding_equivalence_class(const std::string &name,
                                                          const std::set<std::set<std::string>> &equivalence_classes) {
    for(auto &equivalence_class : equivalence_classes) {
        if(equivalence_class.find(name) != equivalence_class.end()) return equivalence_class;
    }
    return {name};
}
std::string equivalence_class_to_string(const std::set<std::string> &equivalence_class) {
    std::string result = "{";
    for(const std::string &name : equivalence_class) {
        result += name + ", ";
    }
    return result.substr(0, result.size()-2)+"}";
}
std::set<std::set<std::string>> get_equivalence_classes(const FillingTable &table) {
    std::vector<std::vector<std::string>> equivalence_classes;
    std::set<std::set<std::string>> equivalence_classes_set;
    for(auto &currentRow : table) {
        for(auto &currentCol : currentRow.second) {
            if(!currentCol.second) {
                bool found = false;
                for(auto& equivalence_class : equivalence_classes) {
                    auto search_result = std::find(equivalence_class.begin(),
                                                   equivalence_class.end(), currentCol.first);
                    if(search_result != equivalence_class.end()) {
                        found = true;
                        equivalence_class.push_back(currentRow.first);
                    }
                    else {
                        search_result = std::find(equivalence_class.begin(), equivalence_class.end(), currentRow.first);
                        if(search_result != equivalence_class.end()) {
                            found = true;
                            equivalence_class.push_back(currentCol.first);
                        }
                    }
                }
                if(!found) {
                    equivalence_classes.push_back({currentRow.first, currentCol.first});
                }
            }
        }
    }
    for(auto vect : equivalence_classes){
        std::set<std::string> equivalence_class_set(vect.begin(), vect.end());
        equivalence_classes_set.insert(equivalence_class_set);
    }
    return equivalence_classes_set;
}
DFA DFA::minimize() const {
    const FillingTable & table = get_table();
    std::set<std::set<std::string>> equivalence_classes = get_equivalence_classes(table);
    TransitionMap transition_map_copy = get_transition_map();
    DFA minimized_dfa = DFA(*this, equivalence_classes);
    return minimized_dfa;
}
void print_table(const FillingTable &table) {
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
    std::cout << "\n";
}
bool operator==(const DFA &dfa1, const DFA &dfa2) {
    FillingTable table = dfa1.get_table(dfa2);
    print_table(table);
    return !get_table_element(table, dfa1.get_start_state()->get_naam(), dfa2.get_start_state()->get_naam());
}
State* DFA::get_output(State* const state, const char &symbol) const {
    return transition_map.at(state).at(symbol);
}
void DFA::fill_table(FillingTable &table, const DFA &dfa) const {
    bool new_distinguishable;
    do {
        new_distinguishable = false;
        for(const auto &currentRow : table) {
            for(const auto &currentCol : currentRow.second) {
                if(!currentCol.second) {
                    for(const char current_symbol : alphabet) {
                        bool row = get_state_by_name(currentRow.first) != nullptr;
                        bool col = get_state_by_name(currentCol.first) != nullptr;
                        State* rowState = (row)
                                ? get_state_by_name(currentRow.first) :
                                dfa.get_state_by_name(currentRow.first);
                        State* colState = (col)
                                          ? get_state_by_name(currentCol.first) :
                                          dfa.get_state_by_name(currentCol.first);

                        const std::string strA = (row) ?
                                get_output(rowState, current_symbol)->get_naam()
                                : dfa.get_output(rowState, current_symbol)->get_naam();

                        const std::string strB = (col) ?
                                get_output(colState, current_symbol)->get_naam()
                                : dfa.get_output(colState, current_symbol)->get_naam();

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
FillingTable DFA::get_table(const DFA &dfa) const {
    FillingTable table;
    std::vector<std::string> state_names;
    for(const State* const &current_state : get_states()) {
        state_names.push_back(current_state->get_naam());
    }
    for(const State* const &current_state : dfa.get_states()) {
        state_names.push_back(current_state->get_naam());
    }
    std::sort(state_names.begin(), state_names.end(), std::less<std::string>());

    for(auto str_iter = next(state_names.begin()); str_iter != state_names.end(); str_iter++) {
        const std::string &current_naam = *str_iter;
        State* current_state = (get_state_by_name(current_naam) != nullptr )
                ? get_state_by_name(current_naam) :
                dfa.get_state_by_name(current_naam);

        assert(current_state != nullptr);
        for(auto iter = state_names.begin(); *iter != current_naam; iter++) {
            State* iter_state = (get_state_by_name(*iter) != nullptr )
                    ? get_state_by_name(*iter) :
                    dfa.get_state_by_name(*iter);
            assert(iter_state != nullptr);
            const bool distinguishable = current_state->get_isAccepting() ^ iter_state->get_isAccepting();
            table[current_naam][*iter] = distinguishable;
        }
    }
    fill_table(table, dfa);
    return table;
}
FillingTable DFA::get_table() const {
    FillingTable table;
    std::vector<std::string> state_names;
    for(const State* const &current_state : get_states()) {
        state_names.push_back(current_state->get_naam());
    }
    std::sort(state_names.begin(), state_names.end(), std::less<std::string>());
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
    print_table(get_table());
}
void DFA::validate() const {
    assert(is_subset_of(get_end_states(), get_states()));
    assert(is_element_of(get_start_state(), get_states()));

    TransitionMap transition_map_copy = get_transition_map();

    for(State* current_state : get_states()) {
        auto found_state = transition_map_copy.find(current_state);
        for(char character : get_alphabet()) {
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
    for(State* current_state : get_states()) {
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
DFA::DFA(const DFA &dfa) {
    std::set<State*> new_states = dfa.get_states();
    std::set<State*> new_end_states = dfa.get_end_states();
    const State* const new_start_state = dfa.get_start_state();
    std::set<char> new_alphabet = dfa.get_alphabet();
    TransitionMap new_transition_map = dfa.get_transition_map();

    alphabet = new_alphabet;

    for(const State* current_state : new_states) {
        auto new_state = new State(current_state->get_naam(), current_state->get_isStarting(),
                                     current_state->get_isAccepting());
        states.insert(new_state);
        if(current_state->get_isAccepting()) end_states.insert(new_state);
        if(current_state->get_isStarting()) start_state = new_state;
    }
    for(const auto &transitions : new_transition_map) {
        std::map<char, State*> new_transitions_map;
        for(const auto &transition : transitions.second) {
            new_transitions_map.insert(
                    {transition.first, get_state_by_name(transition.second->get_naam())});
        }
        Transitions new_transitions =
                {get_state_by_name(transitions.first->get_naam()), new_transitions_map};
        transition_map.insert(new_transitions);
    }
    validate();
}
DFA::DFA(const DFA &dfa, const std::set<std::set<std::string>> &equivalence_classes) {
    std::set<State*> new_states = dfa.get_states();
    std::set<State*> new_end_states = dfa.get_end_states();
    State* new_start_state = dfa.get_start_state();
    std::set<char> new_alphabet = dfa.get_alphabet();
    TransitionMap new_transition_map = dfa.get_transition_map();

    alphabet = new_alphabet;

    for(const State* current_state : new_states) {
        std::set<std::string> equivalence_class =
                get_corresponding_equivalence_class(current_state->get_naam(), equivalence_classes);
        const std::string equivalence_class_str = equivalence_class_to_string(equivalence_class);
        State* search_result = get_state_by_name(equivalence_class_str);

        if(search_result == nullptr) {
            auto new_state = new State(equivalence_class_str, false,
                                       current_state->get_isAccepting());
            if(current_state->get_isAccepting()) end_states.insert(new_state);
            states.insert(new_state);
        }
    }
    start_state = get_state_by_name(equivalence_class_to_string(
            get_corresponding_equivalence_class(new_start_state->get_naam(), equivalence_classes)));
    start_state->set_isStarting(true);

    for(const auto &transitions : new_transition_map) {
        std::map<char, State*> new_transitions_map;
        for(const auto &transition : transitions.second) {
            std::set<std::string> equivalence_class =
                    get_corresponding_equivalence_class(transition.second->get_naam(), equivalence_classes);
            const std::string state_name =
                    equivalence_class_to_string(equivalence_class);

            new_transitions_map.insert(
                    {transition.first, get_state_by_name(state_name)});

        }
        std::set<std::string> equivalence_class =
                get_corresponding_equivalence_class(transitions.first->get_naam(), equivalence_classes);
        const std::string state_name =
                equivalence_class_to_string(equivalence_class);

        Transitions new_transitions =
                {get_state_by_name(state_name), new_transitions_map};

        transition_map.insert(new_transitions);
    }
    validate();
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
        current_state = get_output(current_state, current_char);
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