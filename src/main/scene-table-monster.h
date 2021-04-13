﻿#pragma once

#include "main/scene-table.h"
#include "system/angband.h"
#include "system/monster-race-definition.h"

void clear_scene_target_monster();
void set_temp_mute_scene_monster(int sec);
int get_scene_monster_count();
void refresh_scene_monster(player_type *player_ptr, const std::vector<MONSTER_IDX> &monster_list, scene_type_list &list, int from_index);