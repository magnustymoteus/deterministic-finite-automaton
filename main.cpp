#include "DFA.h"

using namespace std;

int main() {
    DFA dfa("input-tfa1.json");
    dfa.get_table();
    //DFA mindfa = dfa.minimize();
    dfa.printTable();
    //mindfa.print();
    //cout << boolalpha << (dfa == mindfa) << endl;    // zijn ze equivalent? Zou hier zeker moeten. Dit wordt getest in de volgende vraag, maar hiermee kan je al eens proberen
    return 0;
}