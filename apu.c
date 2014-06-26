#include <avr/io.h>
#include <avr/pgmspace.h>
#include "2a03_io.h"
#include "apu.h"
#include "tools/deltacompress.h"

/* 
APU abstraction layer 

Contains functions for putting channel data in registers. 

*/

inline void register_update(uint8_t reg, uint8_t val)
{
    io_reg_buffer[reg] = val;
}

/* Square channels */

inline void sq_setup(uint8_t n)
{
    register_update(SQ1_VOL + n * 4, SQ_LENGTH_CNTR_DISABLE | SQ_CONSTANT_VOLUME);
    register_update(SQ1_HI + n * 4, 1 << LENGTH_CNTR_LOAD_p);
}

inline void sq_update(uint8_t n, Square sq)
{
    register_update(SQ1_VOL + n * 4, (io_reg_buffer[SQ1_VOL + n * 4] & ~(SQ_DUTY_m | VOLUME_m)) 
                                     | sq.volume << VOLUME_p
		                     | sq.duty << SQ_DUTY_p);

    register_update(SQ1_LO + n * 4, sq.period & 0xFF);

    register_update(SQ1_HI + n * 4, (io_reg_buffer[SQ1_HI + n * 4] & ~PERIOD_HI_m) 
		                    | (sq.period >> 8) << PERIOD_HI_p);
}

void sq1_setup()
{
    sq_setup(0);
}

void sq2_setup()
{
    sq_setup(1);
}

void sq1_update()
{
    register_update(SND_CHN, (io_reg_buffer[SND_CHN] & ~SQ1_ENABLE_m) 
		             | sq1.enabled << SQ1_ENABLE_p);

    sq_update(0, sq1);
}

void sq2_update()
{
    register_update(SND_CHN, (io_reg_buffer[SND_CHN] & ~SQ2_ENABLE_m) 
		             | sq2.enabled << SQ2_ENABLE_p);

    sq_update(1, sq2);
}


/* Triangle channel */

void tri_setup()
{
    register_update(TRI_LINEAR, TRI_LENGTH_CNTR_DISABLE | 1);
    register_update(TRI_HI, 0b1000);
}

void tri_update()
{
    register_update(SND_CHN, (io_reg_buffer[SND_CHN] & ~TRI_ENABLE_m) 
		             | tri.enabled << TRI_ENABLE_p);

    register_update(TRI_LO, tri.period & 0xFF);
     
    register_update(TRI_HI, (io_reg_buffer[TRI_HI] & ~PERIOD_HI_m) 
		            | (tri.period >> 8) << PERIOD_HI_p);
}


/* Noise channel */

void noise_setup(){

    register_update(NOISE_VOL, NOISE_LENGTH_CNTR_DISABLE | NOISE_CONSTANT_VOLUME);
    register_update(NOISE_HI, 0b1000);
}

void noise_update()
{
    register_update(SND_CHN, (io_reg_buffer[SND_CHN] & ~NOISE_ENABLE_m) 
		             | noise.enabled << NOISE_ENABLE_p);

    register_update(NOISE_VOL, (io_reg_buffer[NOISE_VOL] & ~VOLUME_m)
               		       | noise.volume << VOLUME_p);

    register_update(NOISE_LO, (io_reg_buffer[NOISE_VOL] & ~(NOISE_LOOP_m | NOISE_PERIOD_m))
	                      | noise.loop << NOISE_LOOP_p 
		              | noise.period << NOISE_PERIOD_p);
}

/* DMC channel */

void dmc_setup()
{
    register_update(DMC_FREQ, 0);
    register_update(DMC_START, 0);
    register_update(DMC_LEN, 0);
}

void dmc_update()
{
    register_update(SND_CHN, (io_reg_buffer[SND_CHN] & ~DMC_ENABLE_m) 
		             | dmc.enabled << DMC_ENABLE_p);

    register_update(DMC_RAW, dmc.data);
}

void dmc_update_sample_old()
{
    if (dmc.sample != 0) {
        dmc.data = pgm_read_byte(&dmc.sample[dmc.current++]) >> 1;

	register_update(DMC_RAW, dmc.data);
	io_register_write(DMC_RAW, dmc.data);

	if (dmc.current == dmc.sample_length) {
	    dmc.current = 0;
	    if (!dmc.sample_loop)
		dmc.sample_enabled = 0;
	}
	
    }
}

void dmc_update_sample() 
{
    static uint8_t data;
    static int8_t accumulator;
    static uint8_t flag;

    if (dmc.sample != 0) {
	if (dmc.current == 0) {
	    accumulator = pgm_read_byte(&dmc.sample[dmc.current++]); 
	    flag = 1;
	}
	else if (!flag) {
	    data = pgm_read_byte(&dmc.sample[dmc.current]);
	    accumulator += delta_table[data & 0x0F];
	}
	else {
	    accumulator += delta_table[(data >> 4) & 0x0F];
	    dmc.current++;
	}
	flag ^= 1;
	    
	dmc.data = accumulator >> 1;

	register_update(DMC_RAW, dmc.data);
	io_register_write(DMC_RAW, dmc.data);

	if (dmc.current == dmc.sample_length) {
	    dmc.current = 0;
	    if (!dmc.sample_loop)
		dmc.sample_enabled = 0;
	}
    }
}

void apu_refresh()
{
    static uint8_t group = 0;
    if (group == 0) { 
	io_write_changed(0x00);
	io_write_changed(0x01);
	io_write_changed(0x02);
	io_write_changed(0x03);
	io_write_changed(0x04);
	io_write_changed(0x05);
	group++;
    }
    else if (group == 1) {
	io_write_changed(0x06);
	io_write_changed(0x07);
	io_write_changed(0x08);
	io_write_changed(0x0A);
	io_write_changed(0x0B);
	io_write_changed(0x0C);
	group++;
    }
    else {
	io_write_changed(0x0E);
	io_write_changed(0x0F);
	io_write_changed(0x10);
	io_write_changed(0x12);
	io_write_changed(0x13);
	io_write_changed(0x15);
	group = 0;
    }
}

void apu_refresh_all()
{
    for (uint8_t i = 0; i < 3; i++)
	apu_refresh();
}
