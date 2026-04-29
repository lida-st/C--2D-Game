#include "player.h"
#include "graphics.h"
#include <cmath>
//αποθηκεύει το παθ και βρισκει τον καινουργιο δρομο
void Player::setPath(const std::vector<Cell>& p)
{
    path = p;
    path_index = 0;
}
//θεση του παιχτη
Cell Player::currentCell(float cellW, float cellH) const
{
    Cell c;
    c.x = (int)std::floor(pos_x / cellW);
    c.y = (int)std::floor(pos_y / cellH);
    return c;
}
//Κάνει το στρουμφάκι να περπατάει βήμα–βήμα πάνω στο μονοπάτι που βρήκε ο A*
void Player::updateWithDt(float dt, float cellW, float cellH)
{
    if (path.empty() || path_index >= (int)path.size())
        return;

    float tx = (path[path_index].x + 0.5f) * cellW;
    float ty = (path[path_index].y + 0.5f) * cellH;

    float dx = tx - pos_x;
    float dy = ty - pos_y;
    float d = std::sqrt(dx * dx + dy * dy);

    float step = speed * dt;

    if (d <= step || d < 1e-4f)
    {
        pos_x = tx;
        pos_y = ty;
        path_index++;
        return;
    }

    pos_x += (dx / d) * step;
    pos_y += (dy / d) * step;
}

void Player::drawWithSprite(float size, const char* texture_path)
{
    graphics::Brush br;
    br.outline_opacity = 0.0f;
    br.fill_opacity = 1.0f;
    br.texture = texture_path;

    graphics::drawRect(pos_x, pos_y, size, size, br);
}

