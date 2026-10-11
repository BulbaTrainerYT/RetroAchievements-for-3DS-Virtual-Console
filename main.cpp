#include <3ds.h>
#include <CTRPluginFramework.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

using namespace CTRPluginFramework;

// ---------------------------------------------------------------------------
// Placeholders / helpers (implement properly)
// ---------------------------------------------------------------------------

// Simple placeholder MD5. Replace with a real implementation.
// Expected to return a 32-character lowercase hex string.
std::string ComputeMD5(const u8* data, size_t len)
{
    // TODO: Implement or include a real MD5 (e.g. from public domain sources).
    // For now return a dummy so the skeleton compiles and shows the flow.
    return "00000000000000000000000000000000";
}

// Detect system type from Title ID or from ROM signature.
// Return one of: "NES", "GB", "GBC", "SNES", "GBA", "unknown"
std::string DetectSystem(u64 titleId)
{
    // TODO: Build a small lookup or range check for known VC Title IDs.
    // For now just return unknown; you can hardcode a few for testing.
    return "unknown";
}

// Apply RA hashing rules and return the hash string.
std::string ComputeRAHash(const u8* rom, size_t size, const std::string& system)
{
    if (system == "NES")
    {
        // If starts with "NES\x1a", skip 16-byte header
        if (size >= 16 && rom[0] == 'N' && rom[1] == 'E' && rom[2] == 'S' && rom[3] == 0x1A)
            return ComputeMD5(rom + 16, size - 16);
        return ComputeMD5(rom, size);
    }
    else if (system == "SNES")
    {
        // If size is 512 bytes more than a multiple of 8KB, strip header
        if (size > 512 && ((size - 512) % 8192) == 0)
            return ComputeMD5(rom + 512, size - 512);
        return ComputeMD5(rom, size);
    }
    else if (system == "GB" || system == "GBC" || system == "GBA")
    {
        return ComputeMD5(rom, size);
    }
    return "";
}

// Minimal HTTP GET using libctru httpc.
// Returns the response body (truncated if huge).
std::string HttpGet(const std::string& url)
{
    httpcInit(0);
    httpcContext context;
    Result res = httpcOpenContext(&context, HTTPC_METHOD_GET, url.c_str(), 0);
    if (R_FAILED(res))
    {
        httpcExit();
        return "httpcOpen failed";
    }

    httpcBeginRequest(&context);

    u8 buffer[4096];
    u32 size = 0;
    res = httpcReceiveData(&context, buffer, sizeof(buffer), &size);
    httpcCloseContext(&context);
    httpcExit();

    if (R_FAILED(res) && res != 0xd8a0a046) // download pending is ok in some cases
        return "receive failed";

    return std::string(reinterpret_cast<char*>(buffer), size);
}

// ---------------------------------------------------------------------------
// Menu action
// ---------------------------------------------------------------------------

void DoHashLookup(MenuEntry* entry)
{
    u64 tid = Process::GetTitleID();
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << tid;
    std::string tidStr = ss.str();

    std::string system = DetectSystem(tid);

    OSD::Notify("TitleID: " + tidStr);
    OSD::Notify("System guess: " + system);

    // TODO: Obtain ROM data.
    // Option A: Read from RomFS (preferred).
    // Option B: Scan memory for signature and copy a reasonable size.
    // For skeleton we pretend we have a small dummy buffer.
    std::vector<u8> dummyRom(64, 0);
    dummyRom[0] = 'N'; dummyRom[1] = 'E'; dummyRom[2] = 'S'; dummyRom[3] = 0x1A;

    std::string hash = ComputeRAHash(dummyRom.data(), dummyRom.size(), "NES");
    OSD::Notify("Hash: " + hash);

    if (!hash.empty())
    {
        std::string url = "http://retroachievements.org/dorequest.php?r=gameid&m=" + hash;
        // Optionally add &u=username&p=password or use a token after login.
        std::string response = HttpGet(url);
        OSD::Notify("RA response: " + response.substr(0, 60));
    }
    else
    {
        OSD::Notify("Could not compute hash");
    }
}

// ---------------------------------------------------------------------------
// Plugin entry points
// ---------------------------------------------------------------------------

void PatchProcess(FwkSettings& settings)
{
    // Called early, before game fully starts. Safe place for patches.
}

void OnProcessExit(void)
{
    // Cleanup if needed.
}

void InitMenu(PluginMenu& menu)
{
    menu += new MenuEntry("RA Hash Lookup", DoHashLookup);
}

int main(void)
{
    PluginMenu* menu = new PluginMenu("VC RA Hash", 0, 1, 0,
        "Skeleton for RetroAchievements hash lookup\non 3DS Virtual Console titles.");

    menu->SynchronizeWithFrame(true);
    InitMenu(*menu);
    menu->Run();
    delete menu;
    return 0;
}