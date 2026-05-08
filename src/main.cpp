// Copyright 2021 GHA Test Team
#include <iostream>
#include <string>
#include "../include/textgen.h"

int main() {
    MarkovTextGenerator generator(2, 1000);

    std::string inputFile = "input.txt";

    if (!generator.loadText(inputFile)) {
        std::cerr << "Не удалось загрузить " << inputFile <<
        ", создаю тестовую таблицу\n";

        MarkovTextGenerator::Prefix p1 = {"жил", "старик"};
        generator.addSuffix(p1, "со");
        generator.addSuffix({"старик", "со"}, "своею");
        generator.addSuffix({"со", "своею"}, "старухой");
        generator.addSuffix({"своею", "старухой"}, "у");
        generator.addSuffix({"старухой", "у"}, "самого");
        generator.addSuffix({"у", "самого"}, "синего");
        generator.addSuffix({"самого", "синего"}, "моря");

        generator.setSeed(42);
    }

    std::cout << "Generation text...\n";
    std::string text = generator.generate();

    generator.saveResult("result/gen.txt");
    std::cout << "Saved in result/gen.txt\n";

    return 0;
}
