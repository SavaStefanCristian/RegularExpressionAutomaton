#include "Automaton.h"
#include <iostream>
#include <string>
#include <fstream>
#include <string>

void printRegexExplanation(const std::string& regex) {
    std::cout << "Regular expression: " << regex << "\n";
    std::cout << "Interpretation:\n";

    if (regex.find('|') != std::string::npos) {
        std::cout << "- The '|' symbol represents a choice between two options.\n";
    }
    if (regex.find('*') != std::string::npos) {
        std::cout << "- The '*' symbol indicates the repetition of a sequence 0 or more times.\n";
    }
    if (regex.find('.') != std::string::npos) {
        std::cout << "- The '.' symbol marks the concatenation of sequences.\n";
    }
}

int main() {

    std::ifstream file("input.txt");
    std::string expression;
    file >> expression;
    file.close();
    RegularExpression::Automaton automaton(expression);

    if (automaton.verifyAutomaton()) {
        std::cout << "Automaton is valid!" << std::endl;
    }
    else {
        std::cout << "Automaton is invalid!" << std::endl;
    }

    std::ofstream outputFile("output.txt");
    outputFile << automaton;
    outputFile.close();

    std::cout << automaton;

    std::cout << std::endl;
    printRegexExplanation(expression);

    std::cout << "\nInsert words for testing the automaton:\n";
    std::string test = "";
    while (test != "0") {
        std::cin >> test;
        if (automaton.CheckWord(test)) {
            std::cout << "ACCEPTED\n";
        }
        else {
            std::cout << "NOT ACCEPTED\n";
        }
    }

    return 0;
}
