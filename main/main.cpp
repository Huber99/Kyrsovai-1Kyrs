#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "game.h"

int main() {
    setlocale(LC_ALL, "RU");
    std::srand(std::time(nullptr));

    int width, height;
    std::cout << "������� ������� �������� ���� (������ ������): ";
    std::cin >> width >> height;

    int targetCount, mirrorCount;
    std::cout << "������� ���������� ����� � ������: ";
    std::cin >> targetCount >> mirrorCount;

    Game game(width, height, targetCount, mirrorCount);
    game.run();

    return 0;
}