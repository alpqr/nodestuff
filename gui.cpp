#include "gui.h"

void Gui::frame()
{
    ImGui::Begin("Window");
    ImGui::Text("Hello world");
    ImGui::End();
}
