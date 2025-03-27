#include "Automaton.h"

using namespace RegularExpression;

//AUTOMATON



RegularExpression::Automaton::Automaton(const char& operand, size_t currentStatesCount)
{
	m_states.insert("q" + std::to_string(currentStatesCount));
	m_states.insert("q" + std::to_string(currentStatesCount + 1));
	m_alphabet.insert(operand);
	m_initialState = "q" + std::to_string(currentStatesCount);
	m_finalStates.insert("q" + std::to_string(currentStatesCount + 1));

	m_transitionFunction["q" + std::to_string(currentStatesCount)][operand].insert("q" + std::to_string(currentStatesCount + 1));
}

RegularExpression::Automaton::Automaton(const std::string& word)
{
	*this = buildAutomaton(word);
}


// Methods

bool RegularExpression::Automaton::verifyAutomaton() const {
	// 1. Check if the initial state is defined
	if (m_initialState.empty()) {
		std::cerr << "Error: The automaton does not have an initial state defined." << std::endl;
		return false;
	}

	// 2. Verify that the initial state belongs to the set of states
	if (m_states.find(m_initialState) == m_states.end()) {
		std::cerr << "Error: The initial state '" << m_initialState << "' is not part of the state set." << std::endl;
		return false;
	}

	// 3. Check if there are any final states defined
	if (m_finalStates.empty()) {
		std::cerr << "Error: The automaton does not have any final states defined." << std::endl;
		return false;
	}

	// 4. Verify that all final states are valid (belong to the state set)
	for (const auto& finalState : m_finalStates) {
		if (m_states.find(finalState) == m_states.end()) {
			std::cerr << "Error: The final state '" << finalState << "' is not part of the state set." << std::endl;
			return false;
		}
	}

	// 5. Verify the transition function
	for (const auto& [state, transitions] : m_transitionFunction) {
		// 5.1 Check if the current state exists in the set of states
		if (m_states.find(state) == m_states.end()) {
			std::cerr << "Error: The state '" << state
				<< "' in the transition function is not part of the state set." << std::endl;
			return false;
		}

		for (const auto& [symbol, targetStates] : transitions) {
			// 5.2 Check if the symbol exists in the alphabet
			if (m_alphabet.find(symbol) == m_alphabet.end()) {
				std::cerr << "Error: The symbol '" << symbol
					<< "' in the transition function is not part of the alphabet." << std::endl;
				return false;
			}

			// 5.3 Check if the automaton is deterministic
			if (targetStates.size() != 1) {
				std::cerr << "Error: The transition for state '" << state << "' and symbol '" << symbol
					<< "' is not deterministic (targetStates.size() = " << targetStates.size() << ")." << std::endl;
				return false;
			}

			// 5.4 Check if all target states are valid
			for (const auto& targetState : targetStates) {
				if (m_states.find(targetState) == m_states.end()) {
					std::cerr << "Error: The target state '" << targetState << "' in the transition function is not part of the state set." << std::endl;
					return false;
				}
			}
		}
	}


	// 6. Verify that all states are accessible from the initial state
	std::unordered_set<std::string> visited;
	std::queue<std::string> toVisit;
	toVisit.push(m_initialState);

	while (!toVisit.empty()) {
		std::string current = toVisit.front();
		toVisit.pop();
		visited.insert(current);

		if (m_transitionFunction.find(current) != m_transitionFunction.end()) {
			for (const auto& [symbol, targetStates] : m_transitionFunction.at(current)) {
				for (const auto& targetState : targetStates) {
					if (visited.find(targetState) == visited.end()) {
						toVisit.push(targetState);
					}
				}
			}
		}
	}
	for (const auto& state : m_states) {
		if (visited.find(state) == visited.end()) {
			std::cerr << "Error: The state '" << state << "' is not accessible from the initial state." << std::endl;
			return false;
		}
	}

	std::cout << "The automaton is deterministic and well-defined." << std::endl;
	return true;
}


bool RegularExpression::Automaton::CheckWord(const std::string& word)
{
	std::string currState = m_initialState;
	for (const char& currCh : word) {
		if (m_transitionFunction.find(currState) == m_transitionFunction.end()) return false;
		if (m_transitionFunction[currState].find(currCh) == m_transitionFunction[currState].end()) return false;
		if (m_transitionFunction[currState][currCh].size() == 0 || m_transitionFunction[currState][currCh].size() > 1) {
			std::cerr << "NOT DFA!";
			return false;
		}
		currState = *(m_transitionFunction[currState][currCh].begin());
	}
	if (m_finalStates.find(currState) != m_finalStates.end()) return true;
	return false;
}

Automaton RegularExpression::Automaton::GetDFA()
{
	Automaton DFAAutomaton;

	std::unordered_map<std::unordered_set<std::string>, std::string, SetHash> newStates;

	std::queue<std::unordered_set<std::string>> newStatesQueue; //for iterating

	std::unordered_set<std::string> initialClosure = GetLambdaClosures({ m_initialState });
	DFAAutomaton.m_initialState = "q" + std::to_string(newStates.size());
	DFAAutomaton.m_states.insert(DFAAutomaton.m_initialState);
	newStates[initialClosure] = DFAAutomaton.m_initialState;
	newStatesQueue.push(initialClosure);
	while (!newStatesQueue.empty()) {
		std::unordered_set<std::string> newState = newStatesQueue.front();
		newStatesQueue.pop();
		for (const char& operand : m_alphabet) {
			if (operand == kLambda) continue; // ignore lambda-transitions for DFA
			std::unordered_set<std::string> transitionStates = GetTransitionStates(newState, operand);
			std::unordered_set<std::string> lambdaClosures = GetLambdaClosures(transitionStates);
			if (lambdaClosures.empty()) continue;
			if (newStates.find(lambdaClosures) == newStates.end()) {
				DFAAutomaton.m_alphabet.insert(operand);
				newStates[lambdaClosures] = "q" + std::to_string(newStates.size());
				newStatesQueue.push(lambdaClosures);
				DFAAutomaton.m_states.insert(newStates[lambdaClosures]);

				for (const std::string& NFAState : lambdaClosures) {
					if (m_finalStates.find(NFAState) != m_finalStates.end()) {
						DFAAutomaton.m_finalStates.insert(newStates[lambdaClosures]);
						break;
					}
				}
			}
			DFAAutomaton.m_transitionFunction[newStates[newState]][operand].insert(newStates[lambdaClosures]);

		}
	}
	return DFAAutomaton;
}


std::unordered_set<std::string> RegularExpression::Automaton::GetLambdaClosures(const std::unordered_set<std::string>& states)
{
	std::unordered_set<std::string> lambdaClosures = states;
	std::stack<std::string> stack; //Holds states reachable with lambda
	//Initialize the stack with the states
	for (const std::string& state : states) {
		stack.push(state);
	}

	// DFS
	while (!stack.empty()) {
		std::string currState = stack.top(); stack.pop();

		if (m_transitionFunction.find(currState) == m_transitionFunction.end()) continue;
		if (m_transitionFunction[currState].find(kLambda) == m_transitionFunction[currState].end()) continue;

		for (const std::string& nextState : m_transitionFunction[currState][kLambda]) {
			if (lambdaClosures.find(nextState) == lambdaClosures.end()) {
				lambdaClosures.insert(nextState); 
				stack.push(nextState);
			}
		}
	}


	return lambdaClosures;
}

std::unordered_set<std::string> RegularExpression::Automaton::GetTransitionStates(const std::unordered_set<std::string>& states, const char& operand)
{
	std::unordered_set<std::string> transitionStates;
	for (const std::string& state : states) {
		if (m_transitionFunction.find(state) == m_transitionFunction.end()) continue;
		if (m_transitionFunction[state].find(operand) == m_transitionFunction[state].end()) continue;
		for (const std::string& nextState : m_transitionFunction[state][operand])
			transitionStates.insert(nextState);
	}
	return transitionStates;
}




//Friend methods


std::ostream& RegularExpression::operator<<(std::ostream& os, const Automaton& automaton)
{
	os << "STATES:\n";
	for (const std::string& state : automaton.m_states) {
		os << state << " ";
	}
	os << "\nALPHABET:\n";
	for (const char& ch : automaton.m_alphabet) {
		if (ch == automaton.kLambda) os << "lambda ";
		else os << ch << " ";
	}
	os << "\nINITIAL STATE: " << automaton.m_initialState << "\n";
	os << "FINAL STATES:\n";
	for (const std::string& finalState : automaton.m_finalStates) {
		os << finalState << " ";
	}
	os << "\nTRANSITION FUNCTION:\n";

	std::unordered_set<std::string> closedList;
	std::queue<std::string> queue;
	queue.push(automaton.m_initialState);
	closedList.insert(automaton.m_initialState);
	while (!queue.empty()) {
		std::string currentState = queue.front(); queue.pop();
		if (automaton.m_transitionFunction.find(currentState) == automaton.m_transitionFunction.end()) continue;
		for (const auto& [operand,nextState] : automaton.m_transitionFunction.at(currentState)) {
			for (const std::string& next : nextState) {
				if (operand == automaton.kLambda) os << "function(" << currentState << ",lambda) = " << next << "\n";
				else os << "function(" << currentState << "," << operand << ") = " << next << "\n";
				if (closedList.find(next) == closedList.end()) {
					queue.push(next);
					closedList.insert(next);
				}
			}
		}
	}

	return os;
}

Automaton RegularExpression::OrStructure(const Automaton& rhs, const Automaton& lhs, size_t currentStatesCount)
{
	Automaton result;
	// Reunion of alphabet
	result.m_alphabet = RegularExpression::joinSets(lhs.m_alphabet, rhs.m_alphabet);
	// Add lambda if it isn't already
	result.m_alphabet.insert(lhs.kLambda);
	// Reunion of states
	result.m_states = RegularExpression::joinSets(lhs.m_states, rhs.m_states);
	// Add new initial and final states
	std::string newInitial = "q" + std::to_string(currentStatesCount);
	std::string newFinal = "q" + std::to_string(currentStatesCount + 1);
	result.m_states.insert(newInitial);
	result.m_states.insert(newFinal);
	result.m_initialState = newInitial;
	result.m_finalStates.insert(newFinal);
	// Reunion of transition functions
	result.m_transitionFunction = RegularExpression::joinMaps(lhs.m_transitionFunction, rhs.m_transitionFunction);
	// Adjust transition function to match new initial state
	result.m_transitionFunction[newInitial][lhs.kLambda].insert(lhs.m_initialState);
	result.m_transitionFunction[newInitial][lhs.kLambda].insert(rhs.m_initialState);
	// Adjust transition function to match new final state
	for (const std::string& lhsState : lhs.m_finalStates) {
		result.m_transitionFunction[lhsState][lhs.kLambda].insert(newFinal);
	}
	for (const std::string& rhsState : rhs.m_finalStates) {
		result.m_transitionFunction[rhsState][rhs.kLambda].insert(newFinal);
	}


	return result;
}

Automaton RegularExpression::AndStructure(const Automaton& rhs, const Automaton& lhs)
{
	Automaton result;
	// Reunion of alphabet
	result.m_alphabet = RegularExpression::joinSets(lhs.m_alphabet, rhs.m_alphabet);
	// Reunion of states
	result.m_states = RegularExpression::joinSets(lhs.m_states, rhs.m_states);
	result.m_initialState = lhs.m_initialState;
	result.m_finalStates = rhs.m_finalStates;
	// Reunion of transition functions
	result.m_transitionFunction = RegularExpression::joinMaps(lhs.m_transitionFunction, rhs.m_transitionFunction);
	// Adjust transition function to adapt the connection between the two
	for (const std::string& lhsFinalState : lhs.m_finalStates) {
		if (result.m_transitionFunction.find(rhs.m_initialState) == result.m_transitionFunction.end()) continue;
		for (auto& [operand, nextStatesFromRhsInitialState] : result.m_transitionFunction.at(rhs.m_initialState))
			for(const std::string& next : nextStatesFromRhsInitialState)
			result.m_transitionFunction[lhsFinalState][operand].insert(next);
	}
	result.m_states.erase(rhs.m_initialState);
	result.m_transitionFunction.erase(rhs.m_initialState);
	return result;
}

Automaton RegularExpression::KleeneStructure(const Automaton& automaton, size_t currentStatesCount)
{
	// Copy automaton in result
	Automaton result;
	result.m_alphabet = automaton.m_alphabet;
	// Add labda if it isn't already
	result.m_alphabet.insert(automaton.kLambda);
	// Add new initial and final states
	std::string newInitial = "q" + std::to_string(currentStatesCount);
	std::string newFinal = "q" + std::to_string(currentStatesCount + 1);
	result.m_states = automaton.m_states;
	result.m_states.insert(newInitial);
	result.m_states.insert(newFinal);
	result.m_initialState = newInitial;
	result.m_finalStates.insert(newFinal);
	// Adjust transition function to adapt the connection between the two
	result.m_transitionFunction = automaton.m_transitionFunction;
	result.m_transitionFunction[newInitial][automaton.kLambda].insert(automaton.m_initialState);
	for (const std::string& finalState : automaton.m_finalStates) {
		result.m_transitionFunction[finalState][automaton.kLambda].insert(automaton.m_initialState);
		result.m_transitionFunction[finalState][automaton.kLambda].insert(newFinal);
	}
	result.m_transitionFunction[newInitial][automaton.kLambda].insert(newFinal);
	return result;
}

//Functions


Automaton RegularExpression::buildAutomaton(const std::string& inputExpression)
{
	Automaton lambdaNFAAutomaton;
	std::string polish = polishPostfixNotation(inputExpression);
	if (polish == "") {
		std::cerr << "Could not build automaton! Expression error!\n";
		return lambdaNFAAutomaton;
	}
	lambdaNFAAutomaton = getLambdaNFA(polish);

	Automaton DFAAutomaton = lambdaNFAAutomaton.GetDFA();

	return DFAAutomaton;
}







//////////////LAMBDA NONDETERMINISTIC FINITE AUTOMATON


Automaton RegularExpression::getLambdaNFA(const std::string& polish)
{
	std::stack<Automaton> stack;
	size_t statesCount = 0;
	for (size_t index = 0; index < polish.size(); ++index) {

		const char& current = polish[index];

		// For operand, build a new automaton
		if (isOperand(current)) {
			stack.push(Automaton(current, statesCount));
			statesCount += 2;
			continue;
		}
		if (current == '|') {
			Automaton first = stack.top(); stack.pop();
			Automaton second = stack.top(); stack.pop();
			stack.push(OrStructure(first, second, statesCount));
			statesCount += 2;
		}
		if (current == '.') {
			Automaton first = stack.top(); stack.pop();
			Automaton second = stack.top(); stack.pop();
			stack.push(AndStructure(first, second));
		}
		if (current == '*') {
			Automaton first = stack.top(); stack.pop();
			stack.push(KleeneStructure(first, statesCount));
			statesCount += 2;
		}
		if (current == '+') {
			Automaton first = stack.top(); stack.pop();
			stack.push(AndStructure(first, KleeneStructure(first, statesCount)));
			statesCount += 2;
		}
	}

	return stack.top();
}





/////////POLISH NOTATION


bool RegularExpression::isOperand(const char& c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

int RegularExpression::getPriority(const char& c) {
	if (c == '+') return 2;
	if (c == '*') return 2;
	if (c == '.') return 1;
	if (c == '|') return 0;
	return -1; // c == '('
}

std::string RegularExpression::polishPostfixNotation(const std::string& inputExpression) {
	std::stack<char> op_stack;
	std::string polishNotation;
	for (const char& current : inputExpression) {
		if (current == ' ') continue;
		if (isOperand(current)) {
			polishNotation.push_back(current);
			continue;
		}
		if (current == '(') {
			op_stack.push(current);
			continue;
		}
		if (current == ')') {
			while (!op_stack.empty() && op_stack.top() != '(') {
				polishNotation.push_back(op_stack.top());
				op_stack.pop();
			}
			if (!op_stack.empty()) op_stack.pop();
			else {
				std::cerr << "Invalid expression! Parenthesis error!\n";
				return "";
			}
			continue;
		}
		if (current == '+' || current == '*' || current == '.' || current == '|') {
			while (!op_stack.empty() && getPriority(op_stack.top()) >= getPriority(current)) {
				polishNotation.push_back(op_stack.top());
				op_stack.pop();
			}
			op_stack.push(current);
			continue;
		}
		std::cerr << "Invalid expression! Invalid characters!\n";
		return "";
	}
	while (!op_stack.empty()) {
		if (op_stack.top() == '(') {
			std::cerr << "Invalid expression! Parenthesis error!\n";
			return "";
		}
		polishNotation.push_back(op_stack.top());
		op_stack.pop();
	}
	return polishNotation;
}