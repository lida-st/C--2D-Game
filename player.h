#pragma once
#include "gameobjectr.h"
#include <vector>

struct Cell
{
    int x = 0, y = 0;
};

class Player : public Gameobject
{
    float pos_x = 0.0f, pos_y = 0.0f;  

   
    float speed = 10.0f; 

    std::vector<Cell> path;
    int path_index = 0;

public:
    void init() override {}
    void update() override {}
    void draw() override {}

    void setPosition(float x, float y) { pos_x = x; pos_y = y; }
    void getPosition(float& x, float& y) const { x = pos_x; y = pos_y; }

    void setPath(const std::vector<Cell>& p);
    void updateWithDt(float dt, float cellW, float cellH);

    Cell currentCell(float cellW, float cellH) const;

    bool isMoving() const { return path_index < (int)path.size(); }

   
    void drawWithSprite(float size, const char* texture_path);
};
