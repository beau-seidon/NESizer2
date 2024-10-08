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



  Patch handling

  Handles storing and reading patches to/from memory.
*/


#include "patch/patch.h"
#include "io/memory.h"
#include "parameter/parameter.h"

// First 256 bytes of SRAM not used.
#define PATCH_START 0x100

#define PATCH_SIZE NUM_PARAMETERS

const uint16_t PATCH_MEMORY_END;

void patch_initialize(uint8_t num)
/* Initializes patch memory by writing initial values to the patch memory */
{
    uint16_t address = PATCH_START + PATCH_SIZE * num;

    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) {
        struct parameter data = parameter_get(i);
        memory_write(address++, data.initial_value);
    }
}

void patch_save(uint8_t num)
{
    uint16_t address = PATCH_START + PATCH_SIZE * num;

    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) {
        struct parameter data = parameter_get(i);
        memory_write(address++, *data.target);
    }
}

void patch_load(uint8_t num)
{
    uint16_t address = PATCH_START + PATCH_SIZE * num;

    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) {
        struct parameter data = parameter_get(i);
        *data.target = memory_read(address++);
    }
}

uint8_t patch_pc_limit(int8_t* patch_num, int8_t min, int8_t max, int8_t pc_num)
{
    if (min <= pc_num && pc_num <= max) {
        *patch_num = pc_num;
        return 1;
    }
    return 0;
}
