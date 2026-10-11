#include <3ds.h>
#include <CTRPluginFramework.hpp>
#include <string>
#include <sstream>

using namespace CTRPluginFramework;

void RAHashLookup(void) {
    u64 titleID = Process::GetTitleID();

    // Stub: detect if this is a Virtual Console title
    OSD::Notify("Title ID: " + Utils::Format("%016llX", titleID));

    // Placeholder for ROM location + system-specific hashing
    // (NES header strip, SNES, GB/GBC plain MD5, etc.)
    std::string hash = "placeholderhash";

    // Minimal httpc example to query RetroAchievements gameid
    httpcInit(0);

    httpcContext context;
    Result res = httpcOpenContext(&context, HTTPC_METHOD_GET,
        "http://retroachievements.org/dorequest.php?r=gameid&m=" + hash, 1);

    if (R_SUCCEEDED(res)) {
        httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
        httpcAddRequestHeaderField(&context, "User-Agent", "RA-VC-Plugin");

        res = httpcBeginRequest(&context);
        if (R_SUCCEEDED(res)) {
            u32 statusCode = 0;
            httpcGetResponseStatusCode(&context, &statusCode);

            if (statusCode == 200) {
                u8 buffer[4096];
                u32 size = 0;

                // Fixed: new libctru signature
                res = httpcReceiveData(&context, buffer, sizeof(buffer));
                size = sizeof(buffer);

                if (R_SUCCEEDED(res)) {
                    std::string response((char*)buffer, size);
                    OSD::Notify("RA Response: " + response.substr(0, 80));
                }
            }
        }
        httpcCloseContext(&context);
    }

    httpcExit();
}

int main(void) {
    PluginMenu *menu = new PluginMenu("RA VC Plugin", 1, 0, 0,
        "RetroAchievements lookup for 3DS Virtual Console");

    menu->SynchronizeWithFrame(true);
    menu->ShowWelcomeMessage(false);

    *menu += new MenuEntry("RA Hash Lookup", RAHashLookup);

    menu->Run();
    delete menu;
    return 0;
}