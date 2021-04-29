﻿#include "player-info/alignment.h"
#include "artifact/fixed-art-types.h"
#include "game-option/text-display-options.h"
#include "inventory/inventory-slot-types.h"
#include "monster/monster-info.h"
#include "monster-race/monster-race.h"
#include "monster-race/race-flags3.h"
#include "monster/monster-status.h"
#include "player-info/avatar.h"
#include "player/player-race.h"
#include "player/player-status.h" // todo has_melee_weapon()が相互依存している。後で消す
#include "system/floor-type-definition.h"
#include "system/monster-race-definition.h"
#include "system/monster-type-definition.h"
#include "system/object-type-definition.h"
#include "system/player-type-definition.h"
#include "util/bit-flags-calculator.h"

PlayerAlignment::PlayerAlignment(player_type *creature_ptr)
{
    this->creature_ptr = creature_ptr;
}

/*!
 * @brief クリーチャーの抽象的善悪アライメントの表記を返す。 / Return alignment title
 * @param with_value 徳の情報と一緒に表示する時だけtrue
 * @return アライメントの表記を返す。
 */
concptr PlayerAlignment::get_alignment_description(bool with_value)
{
    auto s = alignment_label();
    if (with_value || show_actual_value)
        return format(_("%s(%ld)", "%s (%ld)"), s, static_cast<long>(this->creature_ptr->alignment));

    return s;
}

/*
 * @brief アライメント情報の更新
 * @param なし
 * @return なし
 */
void PlayerAlignment::update_alignment()
{
    this->reset_alignment();
    floor_type *floor_ptr = creature_ptr->current_floor_ptr;
    for (MONSTER_IDX m_idx = floor_ptr->m_max - 1; m_idx >= 1; m_idx--) {
        auto *m_ptr = &floor_ptr->m_list[m_idx];
        if (!monster_is_valid(m_ptr))
            continue;
        auto *r_ptr = &r_info[m_ptr->r_idx];

        if (!is_pet(m_ptr))
            continue;

        if (any_bits(r_ptr->flags3, RF3_GOOD)) {
            this->set_alignment(r_ptr->level, update_alignment_type::ALIGNMENT_ADDITION);
        }

        if (any_bits(r_ptr->flags3, RF3_EVIL)) {
            this->set_alignment(r_ptr->level, update_alignment_type::ALIGNMENT_SUBTRACTION);
        }
    }

    if (creature_ptr->mimic_form) {
        switch (creature_ptr->mimic_form) {
        case MIMIC_DEMON:
            this->set_alignment(200, update_alignment_type::ALIGNMENT_SUBTRACTION);
            break;
        case MIMIC_DEMON_LORD:
            this->set_alignment(200, update_alignment_type::ALIGNMENT_SUBTRACTION);
            break;
        }
    } else {
        switch (creature_ptr->prace) {
        case RACE_ARCHON:
            this->set_alignment(200, update_alignment_type::ALIGNMENT_ADDITION);
            break;
        case RACE_BALROG:
            this->set_alignment(200, update_alignment_type::ALIGNMENT_SUBTRACTION);
            break;

        default:
            break;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (!has_melee_weapon(creature_ptr, INVEN_MAIN_HAND + i) || (creature_ptr->inventory_list[INVEN_MAIN_HAND + i].name1 != ART_IRON_BALL))
            continue;

        this->set_alignment(1000, update_alignment_type::ALIGNMENT_SUBTRACTION);
    }

    int j = 0;
    int neutral[2];
    for (int i = 0; i < 8; i++) {
        switch (creature_ptr->vir_types[i]) {
        case V_JUSTICE:
            this->set_alignment(creature_ptr->virtues[i] * 2, update_alignment_type::ALIGNMENT_ADDITION);
            break;
        case V_CHANCE:
            break;
        case V_NATURE:
        case V_HARMONY:
            neutral[j++] = i;
            break;
        case V_UNLIFE:
            this->set_alignment(creature_ptr->virtues[i], update_alignment_type::ALIGNMENT_SUBTRACTION);
            break;
        default:
            this->set_alignment(creature_ptr->virtues[i], update_alignment_type::ALIGNMENT_ADDITION);
            break;
        }
    }

    for (int i = 0; i < j; i++) {
        if (creature_ptr->alignment > 0) {
            this->set_alignment(creature_ptr->virtues[neutral[i]] / 2, update_alignment_type::ALIGNMENT_SUBTRACTION);
            if (creature_ptr->alignment < 0)
                this->reset_alignment();
        } else if (creature_ptr->alignment < 0) {
            this->set_alignment(creature_ptr->virtues[neutral[i]] / 2, update_alignment_type::ALIGNMENT_ADDITION);
            if (creature_ptr->alignment > 0)
                this->reset_alignment();
        }
    }
}

void PlayerAlignment::set_alignment(int value, update_alignment_type ua_type)
{
    switch (ua_type) {
    case update_alignment_type::ALIGNMENT_SUBSTITUTION:
        this->creature_ptr->alignment = value;
        return;
    case update_alignment_type::ALIGNMENT_ADDITION:
        this->creature_ptr->alignment += value;
        return;
    case update_alignment_type::ALIGNMENT_SUBTRACTION:
        this->creature_ptr->alignment -= value;
        return;
    default:
        return;
    }
}

void PlayerAlignment::reset_alignment()
{
    this->set_alignment(0, update_alignment_type::ALIGNMENT_SUBSTITUTION);
}

/*!
 * @brief クリーチャーの抽象的善悪アライメントの表記名のみを返す。 / Return only alignment title
 * @param creature_ptr 算出するクリーチャーの参照ポインタ。
 * @return アライメントの表記名
 */
concptr PlayerAlignment::alignment_label()
{
    if (this->creature_ptr->alignment > 150)
        return _("大善", "Lawful");
    else if (this->creature_ptr->alignment > 50)
        return _("中善", "Good");
    else if (this->creature_ptr->alignment > 10)
        return _("小善", "Neutral Good");
    else if (this->creature_ptr->alignment > -11)
        return _("中立", "Neutral");
    else if (this->creature_ptr->alignment > -51)
        return _("小悪", "Neutral Evil");
    else if (this->creature_ptr->alignment > -151)
        return _("中悪", "Evil");
    else
        return _("大悪", "Chaotic");
}
