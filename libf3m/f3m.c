// made in 2015 by GreaseMonkey - Public Domain
// modified in 2016 by nextvolume for inclusion in PSXSDK
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <psx.h>
#include "f3m.h"

typedef struct ins
{
	uint8_t typ;
	uint8_t fname[12];
	uint8_t dat_para_h;
	uint16_t dat_para;
	uint32_t len, lpbeg, lpend;
	uint8_t vol, rsv1, pack, flags;
	uint32_t c4freq;
	uint8_t rsv2[12];
	uint8_t name[28];
	uint8_t magic[4];
} __attribute__((__packed__)) ins_s;

extern mod_s fsys_s3m_test[];

const uint32_t period_amiclk = 8363*1712-400;
const uint16_t period_table[12] = {1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907};

// from ITTECH.TXT
static const int8_t f3m_sintab[64] = {
	  0,  6, 12, 19, 24, 30, 36, 41,
	 45, 49, 53, 56, 59, 61, 63, 64,
	 64, 64, 63, 61, 59, 56, 53, 49,
	 45, 41, 36, 30, 24, 19, 12,  6,
};

mod_s *f3m_mod_load(uint32_t *d)
{
	return (mod_s*)d;
}

mod_s *f3m_mod_load_filename(const char *fname)
{
	FILE *fp;
	int filesize;
	char *buf;
	mod_s *m;
	
	fp = fopen(fname, "rb");
	
	if(!fp)
		return NULL;

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buf = malloc(filesize);
	fread(buf, sizeof(char), filesize, fp);

	fclose(fp);
	
	m = f3m_mod_load((uint32_t*)buf);
	
	if(!m)
		free(buf);
	
	return m;
}

void f3m_mod_free(mod_s *mod)
{
	free(mod);
}

static uint16_t f3m_get_para(const uint16_t *p)
{
	const uint8_t *p2 = (const uint8_t *)p;
	uint16_t v0 = p2[0];
	uint16_t v1 = p2[1];

	return (v1<<8)|v0;
}

static int32_t f3m_calc_tempo_samples(int32_t tempo)
{
	return (F3M_FREQ*10)/(tempo*4);
}

static int32_t f3m_calc_freq(int32_t freq)
{
#if F3M_FREQ == 32768
	freq <<= 1;
#else
#if F3M_FREQ == 16384
	freq <<= 2;
#else
	freq = (freq << 10) / (F3M_FREQ >> 6);
#endif
#endif
	return freq;

}

static int32_t f3m_calc_period_freq(int32_t period)
{
	int32_t freq = period_amiclk / period;
	return f3m_calc_freq(freq);
}

int f3m_set_mono_mode(player_s *player, int onoff)
{
	int old = player->monomode;

	player->monomode = onoff;
	
	return old;
}

uint16_t f3m_set_max_volume(player_s *player, uint16_t maxvolume)
{
	uint16_t old = player->maxvolume;
	
	if(maxvolume > SPU_MAXVOL)
		player->maxvolume = SPU_MAXVOL;
	else
		player->maxvolume = maxvolume;
	
	return old;
}

unsigned int f3m_player_init(player_s *player, mod_s *mod)
{
	return f3m_player_init_ex(player, mod, 0, SPU_DATA_BASE_ADDR);
}

unsigned int f3m_player_init_ex(player_s *player, mod_s *mod, int basevoice, unsigned int baseaddr)
{
	int i;

	// Optional callback
	//update_music_status(0, mod->ins_num);

	player->mod = mod;
	player->modbase = (const void *)mod;
	player->ord_list = (const uint8_t *)(((const char *)(mod+1)));
	player->ins_para = (const uint16_t *)(((const char *)(mod+1)) + mod->ord_num);
	player->pat_para = (const uint16_t *)(((const char *)(mod+1)) + mod->ord_num + mod->ins_num*2);

	player->speed = mod->ispeed;
	player->tempo = mod->itempo;
	player->gvol = mod->gvol;
	player->ctick = player->speed;
	player->tempo_samples = f3m_calc_tempo_samples(player->tempo);
	player->tempo_wait = 0;

	player->cord = 0-1;
	player->cpat = 0;
	player->crow = 64;
	player->patptr = NULL;
	player->patptr_next = NULL;
	player->sfxoffs = 0;
	player->ccount = 16;
	player->repeat_row = 0;
	player->repeat_count = 0;
	
	player->baseaddr = baseaddr;
	player->basevoice = basevoice;
	player->monomode = 0;
	player->maxvolume = SPU_MAXVOL;

	for(i = 0; i < F3M_VCHNS; i++)
	{
		vchn_s *vchn = &(player->vchn[i]);

		vchn->spu_data = 0;

		vchn->len = 0;
		vchn->len_loop = 0;

		vchn->freq = 0;
		vchn->offs = 0;
		vchn->suboffs = 0;
		vchn->priority = (i < player->ccount ? F3M_PRIO_MUSIC_OFF : 0);

		vchn->gxx_period = 0;
		vchn->period = 0;
		vchn->insvol = 0;
		vchn->midvol = 0;
		vchn->outvol = 0;
		vchn->pan = ((mod->mvol&0x80)==0)
			? 0x8
			: (mod->defpanFC == 0xFC
				? ((const uint8_t *)(player->pat_para + mod->pat_num))[i] & 0xF
				: ((i&1)?0xC:0x3));

		vchn->vib_offs = 0;
		vchn->tre_offs = 0;
		vchn->rtg_count = 0;

		vchn->eft = 0;
		vchn->efp = 0;
		vchn->lefp = 0;
		vchn->last_note = 0;
		vchn->lins = 0;

		vchn->mem_gxx = 0;
		vchn->mem_hxx = 0;
		vchn->mem_oxx = 0;
	}

	int j, k;

	// load samples
	for(i = 0; i < 99; i++)
	{
		player->psx_spu_offset[i] = 0;
		player->psx_spu_offset_lpbeg[i] = 0;
	}

	uint16_t spu_offs = baseaddr >> 3;
	
	static uint16_t smp_data_buf[8];

	static int smp_src_buf[28];
	int smp_data_last = 0;

	for(i = 0; i < 99 && i < mod->ins_num; i++)
	{
		//update_music_status(i, mod->ins_num);
		// TODO: subtly adjust samples so loops work properly

		// Get instrument + check if valid
		const ins_s *ins = ((void *)mod) + (((uint32_t)(f3m_get_para(&player->ins_para[i])))*16);
		uint32_t para = (((uint32_t)(ins->dat_para_h))<<16)|((uint32_t)(ins->dat_para));
		if(ins->len == 0 || para == 0)
			continue;

		int lpbeg = (((ins->flags & 0x01) != 0) ? ins->lpbeg : ins->len + 64);
		int lpend = (((ins->flags & 0x01) != 0) ? ins->lpend+1 : ins->len + 64);
		// Ensure the loop actually fires
		// Not sure if the assurance is really that good here!
		if((ins->flags & 0x01) != 0 && lpend > ((int)ins->len)-14)
			lpend = ins->len-14;

		const uint8_t *data = ((void *)mod) + (para*16);
		player->psx_spu_offset[i] = spu_offs;
		player->psx_spu_offset_lpbeg[i] = spu_offs;
		for(j = 0; j < 64000 && j < (int)ins->len; j += 28, data += 28, spu_offs += (0x10>>3))
		{
			// Load data
			int src_min = smp_data_last;
			int src_max = smp_data_last;

			for(k = 0; k < 28; k++)
			{
				int v = (j+k >= (int)ins->len ? 0 : (((int)(data[k]))-0x80)<<8);
				if(v < src_min) src_min = v;
				if(v > src_max) src_max = v;
				smp_src_buf[k] = v;
			}

			// Calculate shift
			int src_range = src_max - src_min;
			int shift = 0;
			while(src_range >= 16 && shift < 12)
			{
				shift++;
				src_range >>= 1;
			}

			// Clear old buffer
			for(k = 0; k < 8; k++)
				smp_data_buf[k] = 0;

			// Set header
			// applying filter 1 so we get a delta + soft LPF
			smp_data_buf[0] = (12-shift) | (1<<4) | ((0x00)<<8);
			if(j+14 >= lpbeg && j-14 < lpbeg)
			{
				smp_data_buf[0] |= 0x0400;
				player->psx_spu_offset_lpbeg[i] = spu_offs;
			}
			if(j+14 >= lpend && j-14 < lpend)
				smp_data_buf[0] |= 0x0300;

			// Add data
			for(k = 0; k < 28; k++)
			{
				int v = smp_src_buf[k];
				v -= smp_data_last;
				v = (v + (1<<(shift-1)))>>shift;
				if(v < -8) v = -8;
				if(v >  7) v =  7;

				smp_data_buf[1+(k>>2)] |= ((v&15)<<((k&3)<<2));
				smp_data_last += (v<<shift);
			}

			// Upload data
			SsUpload(smp_data_buf, 16, spu_offs << 3);
		}

		// Upload silence
		smp_data_buf[0] = 0x0500;
		
		for(k = 1; k < 8; k++)
			smp_data_buf[k] = 0x0000;
		
		SsUpload(smp_data_buf, 16, spu_offs << 3);
		
		spu_offs += 0x10>>3;
	}
	
	return spu_offs << 3;
}

static void f3m_update_outvol(player_s *player, vchn_s *vchn)
{
	vchn->outvol = (vchn->midvol * player->gvol) >> 6;
}

static void f3m_player_eff_slide_vol(player_s *player, vchn_s *vchn, int isfirst)
{
	(void)player; // "player" is only there to check the fast slide flag (TODO!)

	uint8_t lefp = vchn->lefp;
	int samt = 0;

	if((lefp & 0xF0) == 0x00)
	{
		if((!isfirst) || lefp == 0x0F) samt = -(lefp & 0x0F);
	} else if((lefp & 0x0F) == 0x00) {
		if((!isfirst) || lefp == 0xF0) samt = lefp >> 4;
	} else if((lefp & 0x0F) == 0x0F) {
		if(isfirst) samt = lefp >> 4;
	} else if((lefp & 0xF0) == 0xF0) {
		if(isfirst) samt = -(lefp & 0x0F);
	} else {
		// default: slide down on nonzero ticks
		// SATELL.s3m relies on this
		if(!isfirst) samt = -(lefp & 0x0F);
	}

	if(samt > 0)
	{
		vchn->midvol += samt;
		if(vchn->midvol > 63) vchn->midvol = 63;
	} else if(samt < 0) {
		if(vchn->midvol < (uint8_t)-samt) vchn->midvol = 0;
		else vchn->midvol += samt;
	}
	
	f3m_update_outvol(player, vchn);
}

static void f3m_player_eff_slide_period(vchn_s *vchn, int amt)
{
	vchn->period += amt;
	vchn->freq = f3m_calc_period_freq(vchn->period);
}

static void f3m_player_eff_vibrato(vchn_s *vchn, int lefp, int shift)
{
	vchn->freq = f3m_calc_period_freq(vchn->period);

	int vspeed = (lefp>>4);
	int vdepth = (lefp&15)<<shift;

	// TODO: support other waveforms

	// TODO: find rounding + direction
	int vval = f3m_sintab[vchn->vib_offs&31];
	if(vchn->vib_offs & 32) vval = -vval;
	vval *= vdepth;
	vval += (1<<(5-1));
	vval >>= 5;

	vchn->freq = f3m_calc_period_freq(vchn->period + vval);
	vchn->vib_offs += vspeed;
}

static void f3m_note_retrig(player_s *player, vchn_s *vchn)
{
	int iidx = vchn->lins;
	const ins_s *ins = player->modbase + (((uint32_t)(f3m_get_para(&player->ins_para[iidx-1])))*16);

	int note = vchn->last_note;
	vchn->gxx_period = ((8363 * 16 * period_table[note&15]) / ins->c4freq)
		>> (note>>4);

	vchn->spu_data = player->psx_spu_offset[iidx-1];
	vchn->spu_data_lpbeg = player->psx_spu_offset_lpbeg[iidx-1];

	vchn->priority = F3M_PRIO_MUSIC;
	vchn->len = (((ins->flags & 0x01) != 0) && ins->lpend < ins->len
		? ins->lpend
		: ins->len);
	vchn->len_loop = (((ins->flags & 0x01) != 0) && ins->lpbeg < ins->len
		? vchn->len - ins->lpbeg
		: 0);

	// TODO: verify if this is the case wrt note-end
	if(vchn->spu_data == 0 || (vchn->eft != ('G'-'A'+1) && vchn->eft != ('L'-'A'+1)))
	{
		vchn->period = vchn->gxx_period;
		vchn->freq = f3m_calc_period_freq(vchn->period);
		vchn->offs = 0;
		if(vchn->eft == ('O'-'A'+1))
		{
			vchn->eft = 0;
			int lefp = (vchn->efp != 0 ? vchn->efp : vchn->mem_oxx);
			vchn->mem_oxx = lefp;
			lefp <<= 8;
			if(lefp < vchn->len)
				vchn->offs = lefp;
		}
		vchn->vib_offs = 0; // TODO: find correct retrig point
		vchn->tre_offs = 0; // TODO: find correct retrig point
	}
}

static void f3m_jump_to_row(player_s *player, int nrow)
{
	int i;

	// Ensure in range
	// TODO: work out correct behaviour of out of range values
	if(nrow < 0 || nrow >= 64)
	{
		return;
	}

	// Get patptr
	const uint8_t *p = player->modbase + (((uint32_t)(f3m_get_para(&player->pat_para[player->cpat])))*16);
	p += 2;

	// Walk some number of rows
	for(i = 0; i < nrow; i++)
	{
		for(;;)
		{
			uint8_t v = *(p++);
			if(v == 0) break;
			if((v & 0x80) != 0) p += 2;
			if((v & 0x40) != 0) p += 1;
			if((v & 0x20) != 0) p += 2;
		}
 	}

	// Update next patptr + values
	player->patptr_next = p;
	player->crow = nrow-1;
}

void f3m_effect_nop(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;
}

void f3m_effect_Axx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick == 0 && pefp >= 1)
		player->speed = pefp;
}

void f3m_effect_Bxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick == 0)
	{
		// TODO: handle Bxx/Cxx combined
		player->cord = pefp-1;
		player->crow = 64; 
	}
}

void f3m_effect_Cxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick == 0)
	{
		if(player->patptr_next == NULL)
		{
			// TODO: actually look up the jump value
			player->crow = 64; 
		}
	}
}

void f3m_effect_Dxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	f3m_player_eff_slide_vol(player, vchn, tick == 0);
}

void f3m_effect_Exx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick == 0)
	{
		if(lefp >= 0xF0)
		{
			f3m_player_eff_slide_period(vchn, ((lefp & 0x0F)<<2));
		} else if(lefp >= 0xE0) {
			f3m_player_eff_slide_period(vchn, (lefp & 0x0F));
		}
	} else {
		if(lefp < 0xE0)
		{
			f3m_player_eff_slide_period(vchn, (lefp<<2));
		}
	}
}

void f3m_effect_Fxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick == 0)
	{
		if(lefp >= 0xF0)
		{
			f3m_player_eff_slide_period(vchn, -((lefp & 0x0F)<<2));
		} else if(lefp >= 0xE0) {
			f3m_player_eff_slide_period(vchn, -(lefp & 0x0F));
		}
	} else {
		if(lefp < 0xE0)
		{
			f3m_player_eff_slide_period(vchn, -(lefp<<2));
		}
	}
}

void f3m_effect_Gxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick == 0)
	{
		lefp = (pefp != 0 ? pefp : vchn->mem_gxx);
		vchn->mem_gxx = lefp;
	} else {
		lefp = vchn->mem_gxx;

		if(vchn->period < vchn->gxx_period)
		{
			vchn->period += lefp<<2;
			if(vchn->period > vchn->gxx_period)
				vchn->period = vchn->gxx_period;
			vchn->freq = f3m_calc_period_freq(vchn->period);

		} else if(vchn->period > vchn->gxx_period) {
			vchn->period -= lefp<<2;
			if(vchn->period < vchn->gxx_period)
				vchn->period = vchn->gxx_period;
			vchn->freq = f3m_calc_period_freq(vchn->period);
		}
	}
}

void f3m_effect_Hxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick == 0)
	{
		lefp = pefp;
		if((lefp&0x0F) == 0) lefp |= vchn->mem_hxx&0x0F;
		if((lefp&0xF0) == 0) lefp |= vchn->mem_hxx&0xF0;
		vchn->mem_hxx = lefp;
	} else {
		lefp = vchn->mem_hxx;

		f3m_player_eff_vibrato(vchn, lefp, 2);
	}
}

void f3m_effect_Kxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick != 0)
	{
		f3m_effect_Hxx(player, vchn, tick, 0, 0);
		f3m_effect_Dxx(player, vchn, tick, pefp, lefp);
	}
}

void f3m_effect_Lxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick != 0)
	{
		f3m_effect_Gxx(player, vchn, tick, 0, 0);
		f3m_effect_Dxx(player, vchn, tick, pefp, lefp);
	}
}

void f3m_effect_Qxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	// Notes:
	// 1. When effect is not Qxy, rtg_count is reset.
	// 2. Current y (from lefp, not special mem) is used as a threshold.
	// 3. When y is exceeded, change volume according to current x.

	int voldrop = (lefp>>4);
	int rtick = (lefp&15);

	if(rtick != 0 && vchn->rtg_count >= rtick)
	{
		// Retrigger
		// TODO: work out what happens when we've already done a period or volume slide
		// TODO: 
		f3m_note_retrig(player, vchn);

		if(voldrop < 8)
		{
			if(voldrop < 6)
			{
				vchn->midvol -= (1<<voldrop);
				if(vchn->midvol < 0) vchn->midvol = 0;
			} else if(voldrop == 6) {
				// *2/3, which according to FC is exactly the same as 5/8
 				vchn->midvol = (vchn->midvol*5)>>3; 				
			} else {
				// *1/2
				vchn->midvol = vchn->midvol>>1;
			}

		} else {
			voldrop -= 8;
			if(voldrop < 6)
			{
				vchn->midvol += (1<<voldrop);
			} else if(voldrop == 6) {
				// *3/2
				vchn->midvol = (vchn->midvol*3)>>1;
			} else {
				// *2
				vchn->midvol = vchn->midvol<<1;
			}

			// XXX: do we deal with the case where volume > 63 before doubling?
			if(vchn->midvol > 63) vchn->midvol = 63;
		}

		vchn->rtg_count = 0;
		f3m_update_outvol(player, vchn);
	}

	vchn->rtg_count++;
}

void f3m_effect_Rxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	// TODO: actual tremolo

	if(tick != 0 && vchn->insvol != 0)
	{
		int vspeed = (lefp>>4);
		int vdepth = (lefp&15);

		// TODO: support other waveforms
		// TODO: find rounding + direction
		int vval = f3m_sintab[vchn->tre_offs&31];
		if(vchn->tre_offs & 32) vval = -vval;
		vval *= vdepth;
		vval += (1<<(5-1));
		vval >>= 5;

		// TODO: get clamp range
		vchn->midvol = vchn->insvol + vval;
		if(vchn->midvol <  0) vchn->midvol = 0;
		if(vchn->midvol > 63) vchn->midvol = 63;
		f3m_update_outvol(player, vchn);

		vchn->tre_offs += vspeed;
	}
}

void f3m_effect_Sxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	switch(lefp>>4)
	{
		case 0x8:
			if(tick == 0)
			if((player->mod->mvol&0x80)!=0)
			{
				vchn->pan = lefp & 0x0F;
			}
			break;

		case 0xB:
			// TODO confirm SBx behaviour
			if(tick == 0)
			{
				if((lefp & 0x0F) == 0)
				{
					player->repeat_row = player->crow;
				} else {
					if(player->repeat_count == 0)
					{
						player->repeat_count = lefp & 0x0F;
					} else {
						player->repeat_count--;
					}

					if(player->repeat_count != 0)
					{
						f3m_jump_to_row(player, player->repeat_row);
					} else {
						player->repeat_row = player->crow + 1;
					}
				}
			}
			break;
			
		case 0xC:
			// SC0 is ignored
			// TODO: Make this work:
			// "Playback is temporarily frozen and may be resumed by EFGHJKLU"
			if(tick != 0 && (lefp&0x0F) == tick)
			{
				vchn->spu_data = 0;
				vchn->priority = F3M_PRIO_MUSIC_OFF;
				vchn->midvol = 0;
				f3m_update_outvol(player, vchn);
			}
			break;

		case 0xD:
			// TODO confirm SD0 behaviour
			if(tick != 0 && (lefp&0x0F) == tick)
			{
				f3m_note_retrig(player, vchn);
			}
			break;
	}
}

void f3m_effect_Txx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(pefp >= 33)
	{
		player->tempo = pefp;
		player->tempo_samples = f3m_calc_tempo_samples(player->tempo);
	}

}

void f3m_effect_Uxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick == 0)
	{
		lefp = pefp;
		if((lefp&0x0F) == 0) lefp |= vchn->mem_hxx&0x0F;
		if((lefp&0xF0) == 0) lefp |= vchn->mem_hxx&0xF0;
		vchn->mem_hxx = lefp;
	} else {
		lefp = vchn->mem_hxx;

		f3m_player_eff_vibrato(vchn, lefp, 0);
	}
}

void f3m_effect_Vxx(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp)
{
	(void)player; (void)vchn; (void)tick; (void)pefp; (void)lefp;

	if(tick != 0)
	{
		if(pefp >= 0x00 && pefp <= 0x40)
		{
			player->gvol = lefp;
		}
	}
}


void (*(f3m_effect_tab[32]))(player_s *player, vchn_s *vchn, int tick, int pefp, int lefp) = {
	f3m_effect_nop, f3m_effect_Axx, f3m_effect_Bxx, f3m_effect_Cxx,
	f3m_effect_Dxx, f3m_effect_Exx, f3m_effect_Fxx, f3m_effect_Gxx,
	f3m_effect_Hxx, f3m_effect_nop, f3m_effect_nop, f3m_effect_Kxx,
	f3m_effect_Lxx, f3m_effect_nop, f3m_effect_nop, f3m_effect_nop,

	f3m_effect_nop, f3m_effect_Qxx, f3m_effect_Rxx, f3m_effect_Sxx,
	f3m_effect_Txx, f3m_effect_Uxx, f3m_effect_Vxx, f3m_effect_nop,
	f3m_effect_nop, f3m_effect_nop, f3m_effect_nop, f3m_effect_nop,
	f3m_effect_nop, f3m_effect_nop, f3m_effect_nop, f3m_effect_nop,
};

static void f3m_player_play_newnote(player_s *player)
{
	int i;

	// Advance row
	player->crow++;
	if(player->crow >= 64)
	{
		player->crow = 0;

		// Advance order
		player->cord++;
		while(player->cord < player->mod->ord_num && player->ord_list[player->cord] == 0xFE)
			player->cord++;
		if(player->cord >= player->mod->ord_num || player->ord_list[player->cord] == 0xFF)
			player->cord = 0;
		while(player->cord < player->mod->ord_num && player->ord_list[player->cord] == 0xFE)
			player->cord++;

		player->cpat = player->ord_list[player->cord];
	//	assert(player->cpat < 200);
	//	assert(player->cpat < player->mod->pat_num);

		// Get new pattern pointer
		player->patptr = player->modbase + (((uint32_t)(f3m_get_para(&player->pat_para[player->cpat])))*16);
		player->patptr += 2;
	}

	// Clear vchn pattern data
	for(i = 0; i < F3M_VCHNS; i++)
	{
		vchn_s *vchn = &(player->vchn[i]);
		vchn->eft = 0x00;
		vchn->efp = 0x00;
	}

	// Read pattern data
	if(player->patptr_next != NULL)
	{
		player->patptr = player->patptr_next;
		player->patptr_next = NULL;
	}
	
	if(player->patptr == NULL)
		return;

	const uint8_t *p = player->patptr;

	for(;;)
	{
		uint8_t cv = *(p++);
		if(cv == 0) break;
		vchn_s *vchn = &(player->vchn[cv&15]); // TODO proper channel map check?

		uint8_t pnote = 0xFF;
		uint8_t pins = 0x00;
		uint8_t pvol = 0xFF;
		uint8_t peft = 0x00;
		uint8_t pefp = 0x00;

		if((cv & 0x20) != 0)
		{
			pnote = *(p++);
			pins = *(p++);
		}

		if((cv & 0x40) != 0)
		{
			pvol = *(p++);
		}

		if((cv & 0x80) != 0)
		{
			peft = *(p++);
			pefp = *(p++);
			peft &= 0x1F;
		}

		vchn->eft = peft;
		vchn->efp = pefp;
		if(pefp != 0) vchn->lefp = pefp;
		uint8_t lefp = vchn->lefp;

		// TODO: DO THIS PROPERLY
		if(pnote == 0xFE)
		{
			vchn->spu_data = 0;
			vchn->priority = F3M_PRIO_MUSIC_OFF;

		} else if((pnote < 0x80 && (pins != 0 || vchn->lins != 0))
				|| (pnote == 0xFF && pins != 0)) {
			int iidx = (pins == 0 ? vchn->lins : pins);
			vchn->lins = iidx;
			const ins_s *ins = player->modbase + (((uint32_t)(f3m_get_para(&player->ins_para[iidx-1])))*16);

			// TODO: work out correct rounding
			if(pvol != 0xFF || pins != 0)
			{
				vchn->insvol = (pvol != 0xFF ? pvol
					: pins != 0 ? ins->vol
					: vchn->insvol);
				if(vchn->insvol > 63) vchn->insvol = 63; // lesser-known quirk
				vchn->midvol = vchn->insvol;
			}

			// TODO: work out what happens on note end when ins but no note

			if(vchn->spu_data == 0 || pnote < 0x80)
			{
				int note = (pnote < 0x80 ? pnote : vchn->last_note);
				vchn->last_note = note;

				if(peft != ('S'-'A'+1) || (lefp&0xF0) != 0xD0)
					f3m_note_retrig(player, vchn);
			}
			
			f3m_update_outvol(player, vchn);
		}

		if((peft == ('S'-'A'+1) && (lefp&0xF0) == 0xD0) && pnote >= 0x80)
		{
			// Cancel effect if no note to trigger (e.g. CLICK.S3M)
			// TODO: Check if volume column has any effect
			vchn->eft = 0;
		}

		if(pvol < 0x80)
		{
			vchn->insvol = pvol;
			if(vchn->insvol > 63) vchn->insvol = 63;
			vchn->midvol = vchn->insvol;
			f3m_update_outvol(player, vchn);
		}

		if(peft != ('Q'-'A'+1))
		{
			vchn->rtg_count = 0;
		}

		f3m_effect_tab[peft](player, vchn, 0, pefp, lefp);
	}

	player->patptr = p;
}

void f3m_player_play_newtick(player_s *player)
{
	int i;

	player->ctick++;
	if(player->ctick >= player->speed)
	{
		player->ctick = 0;
		f3m_player_play_newnote(player);
	} else {
		for(i = 0; i < F3M_VCHNS; i++)
		{
			vchn_s *vchn = &(player->vchn[i]);

			uint8_t peft = vchn->eft;
			uint8_t pefp = vchn->efp;
			uint8_t lefp = vchn->lefp;

			f3m_effect_tab[peft&31](player, vchn, player->ctick, pefp, lefp);
		}

	}
}

void f3m_player_play(player_s *player)
{
	int i;
	const int blen = F3M_BUFLEN;

	// Check if we have a new tick
	while(player->tempo_wait < 0)
	{
		f3m_player_play_newtick(player);
		player->tempo_wait += player->tempo_samples;
	}

	player->tempo_wait -= blen;

	// TARGET_PSX.
	// We need to use hardware channels for this.
	uint32_t kon_mask = 0;

	for(i = 0; i < F3M_VCHNS; i++)
	{
		vchn_s *vchn = &(player->vchn[i]);

		// Channel enabled?
		// TODO: handle note offs properly
		if(vchn->spu_data == 0)
		{
			if((vchn->offs & 1) != 0)
			{
				vchn->offs &= ~1;
				SsKeyOff(i+player->basevoice);
			}

			continue;
		}

		// Output sample
		uint16_t spu_offs = vchn->spu_data;
		uint16_t spu_offs_lpbeg = vchn->spu_data_lpbeg;
		int32_t offs = vchn->offs;
		int32_t lvol = vchn->outvol<<7;
		int32_t rvol = vchn->outvol<<7;
		if(vchn->pan < 0x8)
		{
			lvol = (lvol*(vchn->pan*2+1))/15;
		} else {
			rvol = (rvol*((15-vchn->pan)*2+1))/15;
		}
		const int32_t freq = vchn->freq;

		if((vchn->offs & 1) == 0)
		{
			SsVoiceStartAddr(i+player->basevoice, (spu_offs + (((offs+14)/28)<<1)) << 3);
			SsVoiceRepeatAddr(i+player->basevoice, spu_offs_lpbeg << 3);
			SsVoiceADSRRaw(i+player->basevoice, 0x83FF, 0x9FC0);
			
			kon_mask |= (1<<(i+player->basevoice));
			vchn->offs |= 1;
		}

		if(player->maxvolume != SPU_MAXVOL)
		{
			lvol = (lvol * player->maxvolume) / SPU_MAXVOL;
			rvol = (rvol * player->maxvolume) / SPU_MAXVOL;
		}
		
		if(player->monomode)
		{
			if(lvol > rvol)
				rvol = lvol;
			else
				lvol = rvol;
		}
		
		SsVoiceVol(i+player->basevoice, lvol, rvol);
		SsVoicePitch(i+player->basevoice, freq>>4);
	}

	SsKeyOnMask(kon_mask);
}

void f3m_player_stop(player_s *player)
{
	int i;
	
	for(i = 0; i < F3M_VCHNS; i++)
	{
		SsVoiceVol(i+player->basevoice, 0, 0);
		SsVoiceADSRRaw(i+player->basevoice, 0, 0);
	}
}
