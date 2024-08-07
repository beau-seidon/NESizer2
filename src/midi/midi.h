/*
  Copyright 2014-2015 Johan Fjeldtvedt

  This file is part of NESIZER.

  NESIZER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  NESIZER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with NESIZER.  If not, see <http://www.gnu.org/licenses/>.



  MIDI interpreter

  Interprets MIDI messages and acts accordingly.
*/


#pragma once
#include <stdint.h>

#define MIDI_NOTE_LIST_MAX 8

enum sysex_cmd {
    SYSEX_CMD_SAMPLE_LOAD = 1,
    SYSEX_CMD_SETTINGS_LOAD,
    SYSEX_CMD_PATCH_LOAD,
    SYSEX_CMD_SEQUENCE_LOAD,
};

enum sysex_data_format {
    SYSEX_DATA_FORMAT_4BIT,
    SYSEX_DATA_FORMAT_7BIT_TRUNC,
};

struct midi_channel {
    uint8_t note_list[MIDI_NOTE_LIST_MAX];
    uint8_t note_list_length;
    uint8_t channel;
    uint8_t listeners_count;
    uint8_t listeners;
};

extern uint8_t midi_transfer_progress;
extern uint8_t midi_notes[5];

void midi_handler(void);

void push_note(uint8_t channel, uint8_t note);
void pop_note(uint8_t channel, uint8_t note);
