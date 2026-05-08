// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <fstream>
#include <algorithm>
#include <string>
#include "../include/textgen.h"

class MarkovTextGeneratorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        gen = new MarkovTextGenerator(2, 100);
    }

    void TearDown() override {
        delete gen;
    }

    MarkovTextGenerator* gen;
};

// Тест 1: Формирование префикса из заданного числа слов
TEST_F(MarkovTextGeneratorTest, PrefixFormation) {
    MarkovTextGenerator::Prefix prefix;
    prefix.push_back("hello");
    prefix.push_back("world");

    EXPECT_EQ(prefix.size(), 2);
    EXPECT_EQ(prefix[0], "hello");
    EXPECT_EQ(prefix[1], "world");
}

// Тест 2: Формирование записи "префикс-суффикс"
TEST_F(MarkovTextGeneratorTest, PrefixSuffixEntry) {
    MarkovTextGenerator::Prefix prefix = {"the", "quick"};
    gen->addSuffix(prefix, "brown");

    const auto& table = gen->getStateTable();
    EXPECT_EQ(table.size(), 1);
    EXPECT_TRUE(table.find(prefix) != table.end());
    EXPECT_EQ(table.at(prefix)[0], "brown");
}

// Тест 3: Выбор единственного суффикса
TEST_F(MarkovTextGeneratorTest, SingleSuffixSelection) {
    gen->setSeed(12345);
    MarkovTextGenerator::Prefix prefix = {"test", "word"};
    gen->addSuffix(prefix, "only");

    std::string result = gen->generate();
    EXPECT_TRUE(result.find("only") != std::string::npos);
}

// Тест 4: Выбор из нескольких суффиксов
TEST_F(MarkovTextGeneratorTest, MultipleSuffixesSelection) {
    MarkovTextGenerator::Prefix prefix = {"color"};
    gen->addSuffix(prefix, "red");
    gen->addSuffix(prefix, "green");
    gen->addSuffix(prefix, "blue");

    const auto& table = gen->getStateTable();
    EXPECT_EQ(table.at(prefix).size(), 3);

    // Проверяем, что все три суффикса есть
    bool hasRed = false, hasGreen = false, hasBlue = false;
    const auto& suffixes = table.at(prefix);
    for (const auto& s : suffixes) {
        if (s == "red") hasRed = true;
        if (s == "green") hasGreen = true;
        if (s == "blue") hasBlue = true;
    }
    EXPECT_TRUE(hasRed && hasGreen && hasBlue);
}

// Тест 5: Формирование текста заданной длины
TEST_F(MarkovTextGeneratorTest, GenerationLength) {
    MarkovTextGenerator smallGen(2, 20);
    smallGen.setSeed(1);

    smallGen.addSuffix({"a", "b"}, "c");
    smallGen.addSuffix({"b", "c"}, "d");
    smallGen.addSuffix({"c", "d"}, "e");

    std::string result = smallGen.generate();
    int wordCount = std::count(result.begin(), result.end(), ' ') + 1;

    EXPECT_LE(wordCount, 20);
}

// Тест 6: Обработка пустой таблицы
TEST_F(MarkovTextGeneratorTest, EmptyTable) {
    MarkovTextGenerator emptyGen(2, 100);
    std::string result = emptyGen.generate();
    EXPECT_EQ(result, "Таблица пуста");
}

// Тест 7: Загрузка текста из файла
TEST_F(MarkovTextGeneratorTest, LoadFromFile) {
    // Создаём тестовый файл
    std::ofstream testFile("test_input.txt");
    testFile << "one two three four five six seven eight nine ten";
    testFile.close();

    bool loaded = gen->loadText("test_input.txt");
    remove("test_input.txt");

    EXPECT_TRUE(loaded);
    EXPECT_GT(gen->getStateTable().size(), 0);
}

// Тест 8: Префикс из 3 слов
TEST_F(MarkovTextGeneratorTest, PrefixSizeThree) {
    MarkovTextGenerator gen3(3, 100);
    MarkovTextGenerator::Prefix prefix = {"one", "two", "three"};
    gen3.addSuffix(prefix, "four");

    const auto& table = gen3.getStateTable();
    EXPECT_EQ(table.size(), 1);
    EXPECT_TRUE(table.find(prefix) != table.end());
}

// Тест 9: Детерминированность с одинаковым seed
TEST_F(MarkovTextGeneratorTest, DeterministicGeneration) {
    MarkovTextGenerator gen1(1, 10);
    MarkovTextGenerator gen2(1, 10);

    gen1.setSeed(999);
    gen2.setSeed(999);

    gen1.addSuffix({"hello"}, "world");
    gen2.addSuffix({"hello"}, "world");

    EXPECT_EQ(gen1.generate(), gen2.generate());
}

// Тест 10: Обработка конца текста
TEST_F(MarkovTextGeneratorTest, EndOfTextHandling) {
    MarkovTextGenerator chainGen(2, 10);

    // Создаём цепочку, которая обрывается
    chainGen.addSuffix({"x", "y"}, "z");
    chainGen.addSuffix({"y", "z"}, "");  // пустой суффикс - конец

    std::string result = chainGen.generate();
    EXPECT_TRUE(result.find("x y z") != std::string::npos);
}

// Тест 11: Сохранение результата в файл
TEST_F(MarkovTextGeneratorTest, SaveToFile) {
    gen->addSuffix({"save"}, "test");
    gen->setSeed(1);

    bool saved = gen->saveResult("test_output.txt");
    std::ifstream check("test_output.txt");
    bool exists = check.good();
    check.close();
    std::remove("test_output.txt");

    EXPECT_TRUE(saved);
    EXPECT_TRUE(exists);
}

// Тест 12: Несколько суффиксов для одного префикса
TEST_F(MarkovTextGeneratorTest, MultipleSuffixesSamePrefix) {
    MarkovTextGenerator::Prefix prefix = {"same", "prefix"};

    gen->addSuffix(prefix, "suffix1");
    gen->addSuffix(prefix, "suffix2");
    gen->addSuffix(prefix, "suffix3");

    const auto& table = gen->getStateTable();
    EXPECT_EQ(table.at(prefix).size(), 3);
}
