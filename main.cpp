#include <3ds.h>
#include <CTRPluginFramework.hpp>
#include <string>

using namespace CTRPluginFramework;

void RAHashLookup(void) {
    u64 titleID = Process::GetTitleID();
    OSD::Notify("Title ID: " + Utils::Format("%016llX", titleID));

    std::string hash = "placeholderhash";

    httpcInit(0);

    httpcContext context;
    Result res = httpcOpenContext(&context, HTTPC_METHOD_GET,
        ("http://retroachievements.org/dorequest.php?r=gameid&m=" + hash).c_str(), 1);

    if (R_SUCCEEDED(res)) {
        httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
        httpcAddRequestHeaderField(&context, "User-Agent", "RA-VC-Plugin");

        res = httpcBeginRequest(&context);
        if (R_SUCCEEDED(res)) {
            u32 statusCode = 0;
            httpcGetResponseStatusCode(&context, &statusCode);

            if (statusCode == 200) {
                u8 buffer[4096] = {0};
                res = httpcReceiveData(&context, buffer, sizeof(buffer));

                if (R_SUCCEEDED(res)) {
                    std::string response((char*)buffer);
                    OSD::Notify("RA: " + response.substr(0, 60));
                }
            }
        }
        httpcCloseContext(&context);
    }

    httpcExit();
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
