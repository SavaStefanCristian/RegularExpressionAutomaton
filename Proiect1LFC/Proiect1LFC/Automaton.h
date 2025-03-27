#pragma once


#include <unordered_set>
#include <unordered_map>
#include <string>
#include <stack>
#include <queue>
#include <iostream>
#include <utility>


namespace RegularExpression {



	class Automaton
	{


	public:
		//Constructors
		Automaton() = default;
		Automaton(const Automaton&) = default;
		Automaton(Automaton&&) = default;
		Automaton& operator=(const Automaton&) = default;
		Automaton& operator=(Automaton&&) = default;
		~Automaton() = default;
		Automaton(const char& operand, size_t currentStatesCount);
		Automaton(const std::string& word);

		//Methods
	public:
		bool verifyAutomaton() const;
		bool CheckWord(const std::string& word);
		friend std::ostream& operator<<(std::ostream& os, const Automaton& automaton);

		Automaton GetDFA();


		//Building lambdaNFA
		friend Automaton OrStructure(const Automaton& lhs, const Automaton& rhs, size_t currentStatesCount);
		friend Automaton AndStructure(const Automaton& lhs, const Automaton& rhs);
		friend Automaton KleeneStructure(const Automaton& automaton, size_t currentStatesCount);

	private:
		//Building DFA
		std::unordered_set<std::string> GetLambdaClosures(const std::unordered_set<std::string>& states);
		std::unordered_set<std::string> GetTransitionStates(const std::unordered_set<std::string>& states, const char& operand);

		//Constants
	private:
		char kLambda = '\0';

	private:
		struct SetHash {
		public:
			size_t operator()(const std::unordered_set<std::string>& value) const {
				size_t hash = 0;
				for (const auto& str : value) {
					hash ^= std::hash<std::string>{}(str) + (hash<<1);
				}
				return hash;
			}
		};


		//Atributes
	private:
		using transition = std::unordered_map<char, std::unordered_set<std::string>>;

		std::unordered_set<std::string> m_states; //multimea starilor
		std::unordered_set<char> m_alphabet; //alfabetul
		std::string m_initialState; //starea initiala
		std::unordered_set<std::string> m_finalStates; //multimea starilor finale
		std::unordered_map<std::string, transition> m_transitionFunction; //functia de tranzitie


	}; //END OF AUTOMATON


	//Functions


	Automaton buildAutomaton(const std::string& inputExpression);

	bool isOperand(const char& c);
	int getPriority(const char& c);
	std::string polishPostfixNotation(const std::string& inputExpression);

	Automaton getLambdaNFA(const std::string& polish);


	//Template Functions

	template<typename T>
	std::unordered_set<T> joinSets(const std::unordered_set<T>& lhs, const std::unordered_set<T>& rhs)
	{
		if (lhs == rhs) return lhs;
		std::unordered_set<T> result;
		if (lhs.size() > rhs.size()) {
			result = lhs;
			for (const T& curr : rhs) {
				result.insert(curr);
			}
		}
		else {
			result = rhs;
			for (const T& curr : lhs) {
				result.insert(curr);
			}
		}
		return result;
	}

	template<typename T1, typename T2, typename T3>
	std::unordered_map<T1, T2, T3> joinMaps(const std::unordered_map<T1, T2, T3>& lhs, const std::unordered_map<T1, T2, T3>& rhs)
	{
		if (lhs == rhs) return lhs;
		std::unordered_map<T1, T2, T3> result;
		if (lhs.size() > rhs.size()) {
			result = lhs;
			for (const std::pair<T1, T2>& curr : rhs) {
				result.insert(curr);
			}
		}
		else {
			result = rhs;
			for (const std::pair<T1, T2>& curr : lhs) {
				result.insert(curr);
			}
		}
		return result;
	}

}




