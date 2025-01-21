# Hopcroft's algorithm for DFA Minimization

## `Structure of the directory:`
  * `inputs/` -> Set of input to test the automaton.
  * `main.cc` -> Implementation of the algorithm applied on a NPC problem.
  * `Makefile` -> Makefile with build, run and clean.

## NP-Complete Problem :
Given the alphabet consisting of the lowercase letters of the English alphabet (a-z) and two finite
languages, Accept and Fail, with the intersection empty. Build a deterministic finite automaton
that accepts words from the Accept-language and reject those from the Fail-language. If a word does not
belong to one of the languages, it does not matter how it is treated by regular expression or automaton.

The input will be read from the input.txt file.
The output will be written in the output.txt file.

### Note: Read more about DFA Minimization [here](https://eprints.soton.ac.uk/262042/1/jpms-ml01.pdf).

### Copyright 2025 Vasile Alexandru-Gabriel (vasilealexandru37@gmail.com)