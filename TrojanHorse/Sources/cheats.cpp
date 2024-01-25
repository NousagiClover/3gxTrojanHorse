#include "cheats.hpp"

namespace CTRPluginFramework
{
    /* オリジナル関数 */

    void cheatSelecter(MenuEntry* entry)
    {
        std::vector<std::string> option = {"空中で止まれる", "壁貫通"};
        Keyboard optionkb("チートを選択してください", option);
        bool flag = false;

        if (Controller::IsKeysDown(Key::L + Key::DPadRight))
            flag = true;
        else if (Controller::IsKeysDown(Key::L + Key::DPadLeft))
            flag = false;
        else
            return;
        
        const int optionMenu = optionkb.Open();
        switch (optionMenu)
        {
        case 0:
            if (flag)
                Process::Write32(0x001531FC, 0xE2411008);
            else
                Process::Write32(0x001531FC, 0xE2811008);
        break;
        
        case 1:
            if (flag)
                Process::Write32(0x00236B74, 0x40800000);
            else
                Process::Write32(0x00236B74, 0x3F000000);
        break;
        }
    }


    /* プレイヤー */

    void hp(MenuEntry* entry)
    {
        if (entry->WasJustActivated())
            Process::Write32(0x00249584, 0xE1A01001);
        else if (!entry->IsActivated())
            Process::Write32(0x00249584, 0xE0411005);
    }

    void moonjump(MenuEntry* entry)
    {
        Process::Write32(0x00236488, 0x0A0000AC);
        Process::Write32(0x00236704, 0x1A00000D);
        if (Controller::IsKeyDown(DPadLeft))
        {
            Process::Write32(0x00236488, 0x1A0000AC);
            Process::Write32(0x00236704, 0x0A00000D);
        }
    }

    void speedHack(MenuEntry* entry)
    {
        if (entry->WasJustActivated())
            Process::Write32(0x00235CD0, 0x40A00000);
        else if (!entry->IsActivated())
            Process::Write32(0x00235CD0, 0x3F800000);
    }



    /* アイテム */

    void item(MenuEntry* entry)
    {
        if (entry->WasJustActivated())
            Process::Write32(0x0022B22C, 0xE3A02063);
        else if (!entry->IsActivated())
            Process::Write32(0x0022B22C, 0xE2422001);
    }

    void endurance(MenuEntry* entry)
    {
        if (entry->WasJustActivated())
            Process::Write32(0x002238A0, 0xE1A00000);
        else if (!entry->IsActivated())
            Process::Write32(0x002238A0, 0xE2411001);
    }

    void craft(MenuEntry* entry)
    {
        if (entry->WasJustActivated())
            Process::Write32(0x002443AC, 0xE3A040FF);
        else if (!entry->IsActivated())
            Process::Write32(0x002443AC, 0xE3A04000);
    }



    /* チャット */

    // メッセージ連射するメッセージ
    std::string msg;

    void autoChat(MenuEntry* entry)
    {
        if (Controller::IsKeysDown(Key::A + Key::DPadDown))
        {
            Process::Write32(0x00440AE8, 0x00000002);
            Process::Write32(0x0043B8A0, 0x00080001);
        }
    }

    void message(MenuEntry* entry)
    {
        std::vector<std::string> option = {"入力", "チャットからコピー"};
        Keyboard optionkb(option);
        Keyboard messagekb("メッセージを入力してください");
        static bool chatMode = false;

        const int optionMenu = optionkb.Open();
        switch (optionMenu)
        {
        case 0:
            // 入力
            if (messagekb.Open(msg) != 0) return;
            OSD::Notify("set message -> " + msg);
        break;
    
        case 1:
            // チャットからコピー
            chatMode = true;
        break;
        }

        entry->SetGameFunc([](MenuEntry*){
            if (chatMode) {
                u32 offset;
                u8 buf;
                u8 size;
                Process::Read8(0x00440ACC, size);

                if (size == 0)
                {
                    MessageBox(Color::Red << "エラー", "チャットに文字を入力して実行")();
                    return;
                }

                for (int i = 0; i < size; i++)
                {
                    Process::Read8(0x00440AD0 + offset, buf);
                    Process::Write8(0x00440AD0 + offset, buf);
                    offset++;
                }
            } else {
                Process::WriteString(0x00440AD0, msg);
            }
        });
    }

    void customKeyboard(MenuEntry* entry)
    {
        // カスタムキーボード
    }
}
