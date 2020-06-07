#ifndef GUI_H
#define GUI_H

#include "imgui.h"
#include "graph.h"

struct Gui
{
    void init(Graph *g);
    void cleanup();
    void frame();

    Graph *graph = nullptr;
};

#endif
