#include "game.h"
#include "graphics.h"
#include "scancodes.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>


//ξεχωρίζει τα κελιά
static bool operator==(const Cell& a, const Cell& b) { return a.x == b.x && a.y == b.y; }


//δίνει εναν μοναδικό πολύ μεγάλο αριθμό σε κάθε κελί ώστε να γίννται πιο γρήγορα οι συγκρίσεις 
struct CellHash
{
    size_t operator()(const Cell& c) const noexcept
    {
        return (size_t)c.x * 73856093u ^ (size_t)c.y * 19349663u;
    }
};
//αποθηκεύει κάθε υποψήφιο κελί στον A*, μαζί με το κόστος g και f.”
struct OpenNode
{
    Cell c;
    float f = 0;
    float g = 0;
};

struct Cmp
{
    bool operator()(const OpenNode& a, const OpenNode& b) const { return a.f > b.f; }
};
//ευρετικη,απόσταση μεταξύ δύο κελιών
static float manhattan(const Cell& a, const Cell& b)
{
    return (float)(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

Game::Game()
{
    std::srand((unsigned)std::time(nullptr));
}
//οτι αρχιζει πρεπει να τελειωνει
Game::~Game()
{

    if (player)
    {
        delete player;
        player = nullptr;
    }
}
//αρχικοποιηση
void Game::init()
{
    //δημιουργεί τον πίνακα εμποδίων
    blocked.assign(GRID_COLS * GRID_ROWS, 0);
    randomObstacles();

    player = new Player();

    // θέση εκκίνησης (1,1)
    float x = (1 + 0.5f) * cellW();
    float y = (1 + 0.5f) * cellH();
    player->setPosition(x, y);

  
}
//Μετατροπή συντεταγμένων σε κελί
Cell Game::worldToCell(float x, float y) const
{
    Cell c;
    c.x = (int)std::floor(x / cellW());
    c.y = (int)std::floor(y / cellH());
    return c;
}

void Game::randomObstacles()
{
    std::fill(blocked.begin(), blocked.end(), 0);

    const Cell start{ 1,1 };

    for (int y = 0; y < GRID_ROWS; y++)
    {
        for (int x = 0; x < GRID_COLS; x++)
        {
            if ((x == start.x && y == start.y) || (x == home.x && y == home.y)) continue;
            if ((std::rand() % 100) < 18) blocked[idx(x, y)] = 1;
        }
    }
}
//υπολογίζει το μονοπάτι από το start στο goal χρησιμοποιώντας τον αλγόριθμο A*
std::vector<Cell> Game::aStar(Cell start, Cell goal) const
{
    //Αν το start ή το goal είναι εκτός grid → σταμάτα, δεν υπάρχει λύση.
     if (!inBounds(start.x, start.y) || !inBounds(goal.x, goal.y)) return {};
    //Αν το start ή το goal είναι πάνω σε εμπόδιο → σταμάτα, δεν υπάρχει λύση.
    if (blocked[idx(start.x, start.y)] || blocked[idx(goal.x, goal.y)]) return {};
    /*open = ουρά αναμονής για έλεγχο
    closed = “ήδη ελέγχθηκαν”
    gScore = πόσο δρόμο περπάτησα μέχρι τώρα
    came = “γονέας” κάθε κελιού*/
    std::priority_queue<OpenNode, std::vector<OpenNode>, Cmp> open;
    std::unordered_map<Cell, float, CellHash> gScore;
    std::unordered_map<Cell, Cell, CellHash> came;
    std::unordered_set<Cell, CellHash> closed;

    gScore[start] = 0.0f;
    open.push({ start, manhattan(start, goal), 0.0f });

    const Cell dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
    //Κύριος βρόχος A*
    while (!open.empty())
    {
        //το καλύτερο κελί,το κελί με το μικρότερο f
        OpenNode curN = open.top();
        open.pop();

        Cell cur = curN.c;
        //Αν το έχουμε ξαναδεί, το αγνοούμε
        if (closed.find(cur) != closed.end()) continue;
        closed.insert(cur);

        if (cur == goal)
        {
            /*Αν το τρέχον κελί είναι ο στόχος κοιτάμε στο came για να βρούμε τον “γονέα” του
                Συνεχίζουμε μέχρι να φτάσουμε στο start
                Αντιστρέφουμε τη λίστα(std::reverse)
                Επιστρέφουμε το path*/
            std::vector<Cell> path;
            Cell p = goal;
            path.push_back(p);

            while (!(p == start))
            {
                auto it = came.find(p);
                if (it == came.end()) break;
                p = it->second;
                path.push_back(p);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        float curG = gScore[cur];
        //εξεταζει τα διπλανά κελιά
        for (auto d : dirs)
        {
            Cell nb{ cur.x + d.x, cur.y + d.y };
            if (!inBounds(nb.x, nb.y)) continue;
            if (blocked[idx(nb.x, nb.y)]) continue;

            float tentative = curG + 1.0f;

            auto itg = gScore.find(nb);
            if (itg == gScore.end() || tentative < itg->second)
            {
                came[nb] = cur;
                gScore[nb] = tentative;
                float f = tentative + manhattan(nb, goal);
                open.push({ nb, f, tentative });
            }
        }
    }

    return {};
}

void Game::update(float ms)
{
    float dt = ms / 1000.0f;
    if (!player) return;

    graphics::MouseState m;
    graphics::getMouseState(m);

    bool L = m.button_left_pressed;
    bool R = m.button_right_pressed;

    bool clickL = L && !prevL;
    bool clickR = R && !prevR;

    prevL = L;
    prevR = R;

    
    if (clickR)
    {
        Cell c = worldToCell(m.cur_pos_x, m.cur_pos_y);
        if (inBounds(c.x, c.y))
        {
            Cell start = player->currentCell(cellW(), cellH());
            if (!(c == start) && !(c == home))
            {
                blocked[idx(c.x, c.y)] = blocked[idx(c.x, c.y)] ? 0 : 1;

                auto p = aStar(start, home);
                player->setPath(p);
            }   }
    }

    player->updateWithDt(dt, cellW(), cellH());
}

void Game::draw()
{
    // Background
    graphics::Brush bg;
    bg.texture = std::string(ASSET_PATH) + "backr.png";
    bg.outline_opacity = 0.0f;
    bg.fill_opacity = 1.0f;
    graphics::drawRect(CANVAS_WIDTH / 2.0f, CANVAS_HEIGHT / 2.0f, CANVAS_WIDTH, CANVAS_HEIGHT, bg);

    // Grid + obstacles
    graphics::Brush gridBr;
    gridBr.fill_opacity = 0.0f;
    gridBr.outline_opacity = 0.15f;
    gridBr.outline_color[0] = 0.0f;
    gridBr.outline_color[1] = 0.0f;
    gridBr.outline_color[2] = 0.0f;

    graphics::Brush obs;
    obs.fill_opacity = 1.0f;
    obs.outline_opacity = 0.0f;
    obs.fill_color[0] = 0.15f;
    obs.fill_color[1] = 0.15f;
    obs.fill_color[2] = 0.15f;

    float cw = cellW(), ch = cellH();

    for (int y = 0; y < GRID_ROWS; y++)
    {
        for (int x = 0; x < GRID_COLS; x++)
        {
            float cx = (x + 0.5f) * cw;
            float cy = (y + 0.5f) * ch;

            graphics::drawRect(cx, cy, cw, ch, gridBr);

            if (blocked[idx(x, y)])
                graphics::drawRect(cx, cy, cw * 0.95f, ch * 0.95f, obs);
        }
    }
    graphics::Brush homeBr;
    homeBr.fill_opacity = 1.0f;
    homeBr.outline_opacity = 0.0f;
    homeBr.texture = std::string(ASSET_PATH) + "HOME.png";

    float hx = (home.x + 0.5f) * cw;
    float hy = (home.y + 0.5f) * ch;
    graphics::drawRect(hx, hy, cw * 0.95f, ch * 0.95f, homeBr);

    player->drawWithSprite(std::min(cw, ch) * 0.95f, "assets\\SMERF1.png");
}
