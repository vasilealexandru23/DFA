#include <bits/stdc++.h>
#define ALPHABET_SIZE 26
#define INITIAL_STATE 0

std::unordered_map<int, std::string> nodeName;
std::unordered_map<std::string, int> nodeIndex;
const int MAX_LEN = 1e6;
std::vector<std::vector<int>> trie(MAX_LEN, std::vector<int>(ALPHABET_SIZE, 0));
std::unordered_set<int> finalState;
std::vector<std::string> accepted, fail;

struct DFA {
    int numStates;
    int numFinal;
    int initialState;
    std::vector<int> finalStates;
    std::vector<std::vector<int>> transitions;
};

/* Helper function to get the new state number after minimization. */
int getNewState(int state, const std::vector<std::vector<int>>& partition) {
    for(size_t i = 0; i < partition.size(); i++) {
        if(std::find(partition[i].begin(), partition[i].end(), state) != partition[i].end()) {
            return i;
        }
    }
    return -1;
}

DFA minimizeDFA(int numStates, const std::unordered_set<int>& finalStates, const std::vector<std::vector<int>>& transitions) {
    std::vector<std::vector<int>> partition;
    
    /* Initial partition: final and non-final states. */
    std::vector<int> finalStateVec, nonFinalStates;
    for(int i = 0; i < numStates; i++) {
        if(finalStates.count(i)) {
            finalStateVec.push_back(i);
        } else {
            nonFinalStates.push_back(i);
        }
    }
    
    if(!finalStateVec.empty()) partition.push_back(finalStateVec);
    if(!nonFinalStates.empty()) partition.push_back(nonFinalStates);
    
    /* Refine partition until no more refinement is possible. */
    bool changed;
    do {
        changed = false;
        std::vector<std::vector<int>> newPartition = partition;
        
        for(size_t i = 0; i < partition.size(); i++) {
            std::vector<std::vector<int>> splits;
            
            for(int state : partition[i]) {
                bool found = false;
                
                for(auto& split : splits) {
                    bool equivalent = true;
                    int representativeState = split[0];
                    
                    for(int c = 0; c < ALPHABET_SIZE; c++) {
                        int dest1 = transitions[state][c];
                        int dest2 = transitions[representativeState][c];
                        
                        int partition1 = getNewState(dest1, partition);
                        int partition2 = getNewState(dest2, partition);
                        
                        if(partition1 != partition2) {
                            equivalent = false;
                            break;
                        }
                    }
                    
                    if(equivalent) {
                        split.push_back(state);
                        found = true;
                        break;
                    }
                }
                
                if(!found) {
                    splits.push_back({state});
                }
            }
            
            if(splits.size() > 1) {
                changed = true;
                newPartition.erase(newPartition.begin() + i);
                newPartition.insert(newPartition.end(), splits.begin(), splits.end());
                break;
            }
        }
        
        if(changed) {
            partition = newPartition;
        }
    } while(changed);
    
    /* Construct minimized DFA. */
    DFA result;
    result.numStates = partition.size();
    result.initialState = getNewState(INITIAL_STATE, partition);
    
    /* Set transitions for minimized DFA. */
    result.transitions.resize(result.numStates, std::vector<int>(ALPHABET_SIZE));
    for(int i = 0; i < result.numStates; i++) {
        int representativeState = partition[i][0];
        for(int c = 0; c < ALPHABET_SIZE; c++) {
            int dest = transitions[representativeState][c];
            result.transitions[i][c] = getNewState(dest, partition);
        }
    }
    
    /* Set final states for minimized DFA. */
    std::set<int> newFinalStates;
    for(int oldState : finalStates) {
        newFinalStates.insert(getNewState(oldState, partition));
    }
    result.finalStates.assign(newFinalStates.begin(), newFinalStates.end());
    result.numFinal = newFinalStates.size();
    
    return result;
}

std::string find_first_prefix(std::string word) {
    for (int len = 1; len <= word.size(); ++len) {
        std::string prefix = word.substr(0, len);
        bool works = true;
        for (int i = 0; i < fail.size(); ++i) {
            if (fail[i].compare(0, prefix.size(), prefix) == 0) {
                works = false;
                break;
            }
        }
        if (works) {
            return prefix;
        }
    }
    return "";
}

int main() {
    std::ifstream fin("input.txt");
    int cnt_accept, cnt_fail, len_string; 
    fin >> cnt_accept >> cnt_fail >> len_string;
    int indexNode = 0;
    
    for (int i = 0; i < cnt_accept; ++i) {
        std::string word;
        fin >> word;
        accepted.push_back(word);
    }
    for (int i = 0; i < cnt_fail; ++i) {
        std::string word;
        fin >> word;
        fail.push_back(word);
    }
    fin.close();
    
    /* Build my DFA. */
    for (int i = 0; i < cnt_accept; ++i) {
        std::string word = find_first_prefix(accepted[i]);
        int lastNode = INITIAL_STATE;
        for (int j = 0; j < word.size(); ++j) {
            std::string prefix = word.substr(0, j + 1);
            if (nodeIndex[prefix] == 0) {
                nodeIndex[prefix] = ++indexNode;
                nodeName[indexNode] = prefix;
                trie[lastNode][word[j] - 'a'] = indexNode;
                lastNode = indexNode;
            } else {
                trie[lastNode][word[j] - 'a'] = nodeIndex[prefix];
                lastNode = nodeIndex[prefix];
            }
            if (j == word.size() - 1) {
                finalState.insert(nodeIndex[prefix]);
            }
        }
    }
    
    for (auto it : finalState) {
        for (int j = 0; j < ALPHABET_SIZE; ++j) {
            trie[it][j] = it;
        }
    }
    
    /* Make the remaining edges in the graph to FAIL state. */
    int FAILState = ++indexNode;
    for (int i = 0; i <= FAILState; ++i) {
        for (int j = 0; j < ALPHABET_SIZE; ++j) {
            if (trie[i][j] == 0) {
                trie[i][j] = FAILState;
            }
        }
    }

    /* Minimize the DFA. */
    DFA minimizedDFA = minimizeDFA(indexNode + 1, finalState, trie);
    
    /* Write the final DFA to file. */
    std::ofstream fout("output.txt");
    fout << "dfa\n" << minimizedDFA.numStates << " " << minimizedDFA.numFinal << " " << minimizedDFA.initialState << "\n";
    
    for(int finalState : minimizedDFA.finalStates) {
        fout << finalState << " ";
    }
    fout << "\n";
    
    for(int i = 0; i < minimizedDFA.numStates; i++) {
        for(int j = 0; j < ALPHABET_SIZE; j++) {
            fout << minimizedDFA.transitions[i][j] << " ";
        }
        fout << "\n";
    }
    
    fout.close();
    return 0;
}
