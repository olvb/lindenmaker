#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace lindenmaker {

class LSystem
{
public:
    using RuleMap = std::unordered_map<char, std::vector<std::string>>;
    LSystem(std::string axiom, const RuleMap& rules);

    std::string derive(unsigned int iter_count) const
    {
        return derive(iter_count, axiom_);
    }

    std::string derive(unsigned int iter_count, std::string_view sentence) const;

private:
    std::string axiom_;
    RuleMap rules_;
};
}
