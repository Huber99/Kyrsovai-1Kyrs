#pragma once
#ifndef GAME_H
#define GAME_H

#include <vector>
#include <utility>

enum class Direction {
    UP, DOWN, LEFT, RIGHT
};

class GameObject {
public:
    GameObject(int x, int y) : x(x), y(y) {}
    virtual ~GameObject() = default;
    int getX() const { return x; }
    int getY() const { return y; }
    virtual char getSymbol() const = 0;

protected:
    int x, y;
};

class Target : public GameObject {
public:
    Target(int x, int y) : GameObject(x, y), destroyed(false) {}
    char getSymbol() const override { return destroyed ? ' ' : 'T'; }
    bool isDestroyed() const { return destroyed; }
    void destroy() { destroyed = true; }

private:
    bool destroyed;
};

class Mirror : public GameObject {
public:
    Mirror(int x, int y) : GameObject(x, y), angle(0) {}
    char getSymbol() const override;
    void rotate(int degrees);
    Direction reflect(Direction incoming) const;

private:
    int angle; // 0, 30, 60, ..., 330 градусов
};

class LaserSource : public GameObject {
public:
    LaserSource(int x, int y, Direction dir) : GameObject(x, y), direction(dir) {}
    char getSymbol() const override;
    Direction getDirection() const { return direction; }

private:
    Direction direction;
};

class Game {
public:
    Game(int width, int height, int targetCount, int mirrorCount);
    void run();
    void printField() const;
    bool shootLaser();

private:
    void initializeField();
    void placeObjects(int targetCount, int mirrorCount);
    bool allTargetsDestroyed() const;

    int width, height;
    std::vector<std::vector<char>> field;
    LaserSource* laser;
    std::vector<Target*> targets;
    std::vector<Mirror*> mirrors;
};

#endif // GAME_H