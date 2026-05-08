// Copyright 2021 GHA Test Team
#include "../include/textgen.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <string>

MarkovTextGenerator::MarkovTextGenerator(int npref, int maxgen)
    : NPREF(npref), MAXGEN(maxgen), rng(std::random_device {}()) {
    }

bool MarkovTextGenerator::loadText(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::vector<std::string> words;
    std::string word;

    while (file >> word) {
        if (!word.empty()) {
            for (char& c : word) c = tolower(c);
            words.push_back(word);
        }
    }

    if (words.size() < static_cast<size_t>(NPREF) + 1) return false;

    // Построение таблицы префикс->суффикс
    statetab.clear();
    for (size_t i = 0; i + NPREF <= words.size(); i++) {
        Prefix prefix;
        for (int j = 0; j < NPREF; j++)
            prefix.push_back(words[i + j]);

        std::string suffix = (i + NPREF < words.size()) ? words[i + NPREF] : "";
        statetab[prefix].push_back(suffix);
    }

    // Сохраняем начальный префикс
    firstPrefix.clear();
    for (int i = 0; i < NPREF; i++)
        firstPrefix.push_back(words[i]);

    return true;
}

std::string MarkovTextGenerator::getRandomSuffix
(const std::vector<std::string>& suffixes) {
    if (suffixes.empty()) return "";
    std::uniform_int_distribution<size_t> dist(0, suffixes.size() - 1);
    return suffixes[dist(rng)];
}

void MarkovTextGenerator::addSuffix
(const Prefix& prefix, const std::string& suffix) {
    statetab[prefix].push_back(suffix);
    if (firstPrefix.empty()) firstPrefix = prefix;
}

std::string MarkovTextGenerator::generate() {
    if (statetab.empty() || firstPrefix.empty())
        return "Таблица пуста";

    Prefix current = firstPrefix;
    std::vector<std::string> result;

    // Добавляем начальный префикс
    for (const auto& w : current) result.push_back(w);

    for (int i = 0; i < MAXGEN - NPREF; i++) {
        auto it = statetab.find(current);
        if (it == statetab.end() || it->second.empty()) break;

        std::string next = getRandomSuffix(it->second);
        if (next.empty()) break;

        result.push_back(next);
        current.pop_front();
        current.push_back(next);
    }

    std::string output;
    for (size_t i = 0; i < result.size(); i++) {
        if (i > 0) output += " ";
        output += result[i];
    }
    return output;
}

bool MarkovTextGenerator::saveResult(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << generate();
    return true;
}
