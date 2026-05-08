// Copyright 2021 GHA Test Team
#ifndef INCLUDE_TEXTGEN_H_
#define INCLUDE_TEXTGEN_H_

#include <deque>
#include <map>
#include <vector>
#include <string>
#include <random>

class MarkovTextGenerator {
 public:
    using Prefix = std::deque<std::string>;
    using StateTable = std::map<Prefix, std::vector<std::string>>;

    explicit MarkovTextGenerator(int npref = 2, int maxgen = 100);

    bool loadText(const std::string& filename);
    std::string generate();
    bool saveResult(const std::string& filename);

    // Для тестов
    void addSuffix(const Prefix& prefix, const std::string& suffix);
    const StateTable& getStateTable() const { return statetab; }
    void setSeed(unsigned seed) { rng.seed(seed); }

 private:
    int NPREF;
    int MAXGEN;
    StateTable statetab;
    std::mt19937 rng;
    Prefix firstPrefix;

    std::string getRandomSuffix(const std::vector<std::string>& suffixes);
};

#endif // INCLUDE_TEXTGEN_H_
