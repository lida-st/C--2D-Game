#pragma once
#include "game.h"
#include "config.h"

#include "graphics.h"
#include "scancodes.h"
#include <cstdio>

class GlobalState
{
private:
    static GlobalState* s_instance;

    int m_level = 1;
    int m_score = 0;

    Game m_game;

    GlobalState() = default;

public:
    GlobalState(const GlobalState&) = delete;
    GlobalState& operator=(const GlobalState&) = delete;

    static GlobalState& get()
    {
        if (!s_instance) s_instance = new GlobalState();
        return *s_instance;
    }

    static void destroy()
    {
        delete s_instance;
        s_instance = nullptr;
    }

    void init()
    {
        m_level = 1;
        m_score = 0;
        m_game.init();
    }

    void update(float ms)
    {
        m_game.update(ms);
    }

    void draw()
    {
        m_game.draw();

        graphics::Brush t;
        t.fill_opacity = 1.0f;
        t.outline_opacity = 0.0f;
        t.fill_color[0] = 0.0f;
        t.fill_color[1] = 0.0f;
        t.fill_color[2] = 0.0f;

        char buf[128];
        std::snprintf(buf, sizeof(buf), "Level: %d  Score: %d", m_level, m_score);
        graphics::drawText(2.0f, 10.0f, 4.0f, buf, t);
    }

    // getters 
    Game& getGame() { return m_game; }
    Player* getPlayer() { return m_game.getPlayer(); }
};
