#pragma once
#include "player.h"
#include "config.h"
#include <vector>

class Game
{
    Player* player = nullptr;
    // 1 = εμπόδιο, 0 = ελεύθερο κελί
    std::vector<unsigned char> blocked;
    // το κελί-στόχος (σπίτι)
    Cell home{ GRID_COLS - 2, GRID_ROWS - 2 };
    //για την αρχή με το κλικ
    bool prevL = false;
    bool prevR = false;

public:
    Game();
    ~Game();

    void init();
    void update(float ms);
    void draw();

    Player* getPlayer() { return player; }

private:
    float cellW() const { return CANVAS_WIDTH / (float)GRID_COLS; }
    float cellH() const { return CANVAS_HEIGHT / (float)GRID_ROWS; }

    int idx(int x, int y) const { return y * GRID_COLS + x; }
    bool inBounds(int x, int y) const { return x >= 0 && x < GRID_COLS && y >= 0 && y < GRID_ROWS; }

    Cell worldToCell(float x, float y) const;
    // Γεμίζει το grid με τυχαία εμπόδια(κρατά start / home ελεύθερα)
    void randomObstacles();
    // A* pathfinding: επιστρέφει λίστα κελιών από start προς goal
    std::vector<Cell> aStar(Cell start, Cell goal) const;
};
