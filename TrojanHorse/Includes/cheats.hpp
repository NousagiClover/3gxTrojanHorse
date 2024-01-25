#ifndef CHEATS_H
#define CHEATS_H

#include <CTRPluginFramework.hpp>
#include "Helpers.hpp"
#include "Unicode.h"

namespace CTRPluginFramework
{
    using StringVector = std::vector<std::string>;

    // プレイヤー
    void hp(MenuEntry* entry);
    void moonjump(MenuEntry* entry);
    void speedHack(MenuEntry* entry);
    void cheatSelecter(MenuEntry* entry);

    // アイテム
    void item(MenuEntry* entry);
    void endurance(MenuEntry* entry);
    void craft(MenuEntry* entry);

    // チャット
    void autoChat(MenuEntry* entry);
    void message(MenuEntry* entry);
    void customKeyboard(MenuEntry* entry);
}
#endif
