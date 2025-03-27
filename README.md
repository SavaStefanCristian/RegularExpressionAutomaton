# Regular Expression Automaton Engine (C++)

## Overview
This project implements a Regular Expression (Regex) engine in C++ using Non-deterministic Finite Automata (NFA) and Deterministic Finite Automata (DFA). The engine supports evaluating regex patterns, converting them into finite automata, and checking string matches efficiently.

## Features
- Supports basic regex operations:
  - `|` (Union/Choice)
  - `*` (Kleene Star for repetition)
  - `.` (Concatenation)
  - `+` (One or more repetitions)
  - `?` (Zero or one occurrence)
  - `()` (Grouping)
- Converts regular expressions to NFA and then DFA
- Validates automaton correctness
- Matches input strings against the generated DFA
- Includes a testing environment

