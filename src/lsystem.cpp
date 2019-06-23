#include "lsystem.hpp"
#include <cstdlib>

namespace lindenmaker {

using std::string;
using std::string_view;

LSystem::LSystem(string axiom, const RuleMap& rules)
    : axiom_(axiom), rules_(rules) {}

string LSystem::derive(unsigned int iter_count, string_view sentence) const
{
    if (iter_count == 0) {
        return string{ sentence };
    }

    string new_sentence = string{};
    for (auto symbol : sentence) {
        auto it = rules_.find(symbol);
        // no rewrite rule, leave as-is
        if (it == rules_.end()) {
            new_sentence += symbol;
            continue;
        }
        // if multiple rules, pick random
        auto symbol_rules = it->second;
        if (symbol_rules.size() == 1) {
            new_sentence += symbol_rules.front();
        } else {
            new_sentence += symbol_rules[std::rand() % symbol_rules.size()];
        }
    }
    return derive(iter_count - 1, new_sentence);
}
}
