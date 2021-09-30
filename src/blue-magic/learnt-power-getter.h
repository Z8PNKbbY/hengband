﻿#pragma once
/*!
 * @file learnt-power-getter.h
 * @brief 青魔法の処理実行ヘッダ
 */

#include "system/angband.h"

#include <optional>

enum class RF_ABILITY;
struct player_type;
std::optional<RF_ABILITY> get_learned_power(player_type *player_ptr);
