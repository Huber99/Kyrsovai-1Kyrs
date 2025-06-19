#include "game.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

char Mirror::getSymbol() const {
    switch (angle) {
    case 0: case 180: return '|';
    case 90: case 270: return '-';
    case 30: case 210: return '/';
    case 60: case 240: return '\\';
    case 120: case 300: return '\\';
    case 150: case 330: return '/';
    default: return '?';
    }
}

void Mirror::rotate(int degrees) {
    angle = (angle + degrees) % 360;
    if (angle < 0) angle += 360;
}

Direction Mirror::reflect(Direction incoming) const {
    int normalAngle = (angle + 90) % 360; // Нормаль к поверхности зеркала

    // Упрощенная модель отражения (реальная физика сложнее)
    switch (incoming) {
    case Direction::UP:
        if (angle == 0 || angle == 180) return Direction::DOWN;
        if (angle == 90 || angle == 270) return Direction::LEFT;
        return (angle < 180) ? Direction::LEFT : Direction::RIGHT;
    case Direction::DOWN:
        if (angle == 0 || angle == 180) return Direction::UP;
        if (angle == 90 || angle == 270) return Direction::RIGHT;
        return (angle < 180) ? Direction::RIGHT : Direction::LEFT;
    case Direction::LEFT:
        if (angle == 0 || angle == 180) return Direction::RIGHT;
        if (angle == 90 || angle == 270) return Direction::DOWN;
        return (angle < 180) ? Direction::DOWN : Direction::UP;
    case Direction::RIGHT:
        if (angle == 0 || angle == 180) return Direction::LEFT;
        if (angle == 90 || angle == 270) return Direction::UP;
        return (angle < 180) ? Direction::UP : Direction::DOWN;
    }
    return incoming; // никогда не должно достигнуться
}

char LaserSource::getSymbol() const {
    switch (direction) {
    case Direction::UP: return '^';
    case Direction::DOWN: return 'v';
    case Direction::LEFT: return '<';
    case Direction::RIGHT: return '>';
    }
    return '?';
}

Game::Game(int width, int height, int targetCount, int mirrorCount)
    : width(width), height(height), laser(nullptr) {
    if (width < 3 || height < 3) {
        throw std::invalid_argument("Поле должно быть не менее 3x3");
    }
    if (targetCount < 1 || mirrorCount < 0) {
        throw std::invalid_argument("Неверное количество целей или зеркал");
    }
    initializeField();
    placeObjects(targetCount, mirrorCount);
}

void Game::initializeField() {
    field.resize(height, std::vector<char>(width, '.'));
}

void Game::placeObjects(int targetCount, int mirrorCount) {
    // Размещаем источник лазера в левом верхнем углу, направленный вправо
    laser = new LaserSource(0, 0, Direction::RIGHT);
    field[0][0] = laser->getSymbol();

    // Размещаем цели
    for (int i = 0; i < targetCount; ++i) {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while ((x == 0 && y == 0) || field[y][x] != '.');

        Target* target = new Target(x, y);
        targets.push_back(target);
        field[y][x] = target->getSymbol();
    }

    // Размещаем зеркала
    for (int i = 0; i < mirrorCount; ++i) {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while ((x == 0 && y == 0) || field[y][x] != '.');

        Mirror* mirror = new Mirror(x, y);
        mirror->rotate((rand() % 12) * 30); // случайный угол
        mirrors.push_back(mirror);
        field[y][x] = mirror->getSymbol();
    }
}

void Game::printField() const {
    for (const auto& row : field) {
        for (char cell : row) {
            std::cout << cell << ' ';
        }
        std::cout << '\n';
    }
}

bool Game::shootLaser() {
    std::vector<std::pair<int, int>> path;
    Direction dir = laser->getDirection();
    int x = laser->getX();
    int y = laser->getY();

    while (true) {
        // Двигаем луч в текущем направлении
        switch (dir) {
        case Direction::UP: y--; break;
        case Direction::DOWN: y++; break;
        case Direction::LEFT: x--; break;
        case Direction::RIGHT: x++; break;
        }

        // Проверяем выход за границы
        if (x < 0 || x >= width || y < 0 || y >= height) {
            break;
        }

        path.emplace_back(x, y);

        // Проверяем попадание в цель
        for (Target* target : targets) {
            if (!target->isDestroyed() && target->getX() == x && target->getY() == y) {
                target->destroy();
                field[y][x] = 'X'; // Помечаем попадание
                std::cout << "Цель уничтожена на позиции (" << x << ", " << y << ")!\n";
                return true;
            }
        }

        // Проверяем отражение от зеркала
        for (Mirror* mirror : mirrors) {
            if (mirror->getX() == x && mirror->getY() == y) {
                dir = mirror->reflect(dir);
                break;
            }
        }
    }

    std::cout << "Луч вышел за пределы поля без поражения целей.\n";
    return false;
}

bool Game::allTargetsDestroyed() const {
    return std::all_of(targets.begin(), targets.end(),
        [](const Target* t) { return t->isDestroyed(); });
}

void Game::run() {
    int remainingTargets = targets.size();

    while (!allTargetsDestroyed()) {
        system("clear"); // Для Linux/Mac. Для Windows: system("cls");
        printField();
        std::cout << "Осталось целей: " << remainingTargets << "\n\n";

        // Выбираем зеркало для поворота
        std::cout << "Выберите зеркало для поворота (индекс 1-" << mirrors.size() << "): ";
        int mirrorIndex;
        std::cin >> mirrorIndex;
        mirrorIndex--; // преобразуем в 0-based индекс

        if (mirrorIndex < 0 || mirrorIndex >= mirrors.size()) {
            std::cout << "Неверный индекс зеркала!\n";
            continue;
        }

        // Поворачиваем зеркало
        std::cout << "На сколько градусов повернуть (кратно 30)? ";
        int degrees;
        std::cin >> degrees;

        if (degrees % 30 != 0) {
            std::cout << "Угол должен быть кратен 30 градусам!\n";
            continue;
        }

        mirrors[mirrorIndex]->rotate(degrees);
        field[mirrors[mirrorIndex]->getY()][mirrors[mirrorIndex]->getX()] =
            mirrors[mirrorIndex]->getSymbol();

        // Стреляем лазером
        bool hit = shootLaser();
        if (hit) {
            remainingTargets--;
        }

        std::cout << "Нажмите Enter для продолжения...";
        std::cin.ignore();
        std::cin.get();
    }

    std::cout << "\nПоздравляем! Все цели уничтожены!\n";
    printField();
}