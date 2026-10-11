#include <CTRPluginFramework.hpp>

using namespace CTRPluginFramework;

void RAHashLookup(void) {
    u64 titleID = Process::GetTitleID();
    OSD::Notify("Title ID: " + Utils::Format("%016llX", titleID));
}

int main(void) {
    PluginMenu *menu = new PluginMenu("RA VC Plugin", 1, 0, 0,
        "RetroAchievements for 3DS Virtual Console");

    menu->SynchronizeWithFrame(true);
    menu->ShowWelcomeMessage(false);

    *menu += new MenuEntry("RA Hash Lookup", RAHashLookup);

    menu->Run();
    delete menu;
    return 0;
}