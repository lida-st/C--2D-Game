#include "graphics.h"
#include "config.h"
#include "globalstate.h"

static void draw()
{
    GlobalState::get().draw();
}

static void update(float ms)
{
    GlobalState::get().update(ms);
}

int main()
{
    graphics::createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Smurf Goes Home (A*) LEFT CLICK ON THE SMURF ON THE TOP LEFT");

    graphics::setDrawFunction(draw);
    graphics::setUpdateFunction(update);

    graphics::setCanvasSize(CANVAS_WIDTH, CANVAS_HEIGHT);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

    GlobalState::get().init();

    graphics::startMessageLoop();
    graphics::destroyWindow();

    GlobalState::destroy();
    return 0;
    
}

