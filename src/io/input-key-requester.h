﻿#pragma once

#include "system/angband.h"
#include "game-option/keymap-directory-getter.h"

extern concptr keymap_act[KEYMAP_MODES][256];

extern bool use_menu;

extern COMMAND_CODE command_cmd;
extern COMMAND_ARG command_arg;
extern short command_rep;
extern DIRECTION command_dir;
extern int16_t command_see;
extern TERM_LEN command_gap;
extern int16_t command_wrk;
extern int16_t command_new;

class PlayerType;
class InputKeyRequestor {
public:
    InputKeyRequestor(PlayerType *player_ptr, bool shopping);
    void request_command();

private:
    PlayerType *player_ptr;
    bool shopping;
    char request_command_buffer[256]{}; /*!< Special buffer to hold the action of the current keymap */
    keymap_mode mode;

    void input_command();
    short get_command();
    char inkey_from_menu();
    bool process_repeat_num(short *cmd);
    char input_repeat_num();
    void process_command_command(short *cmd);
    void process_control_command(short *cmd);
};
