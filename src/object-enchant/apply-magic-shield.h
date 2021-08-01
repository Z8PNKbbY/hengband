﻿#pragma once

#include "object-enchant/abstract-protector-enchanter.h"
#include "object-enchant/enchanter-base.h"
#include "system/angband.h"

struct object_type;
struct player_type;
class ShieldEnchanter : AbstractProtectorEnchanter {
public:
    ShieldEnchanter(player_type *owner_ptr, object_type *o_ptr, DEPTH level, int power);
    ShieldEnchanter() = delete;
    virtual ~ShieldEnchanter() = default;
    void apply_magic() override;

protected:
    void enchant(){};
    void give_ego_index() override;
    void give_high_ego_index(){};
    void give_cursed(){};

private:
    player_type *owner_ptr;
};
