// MultiTracker module file support for MODPlay

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "modplay.h"
extern const unsigned short modplay_pitch_per_tbl[118];

ModMusic *MODLoad_MTM(void *d)
{
	unsigned char *c = d;
	ModMusic *m;
	int x;
	int mp=0;
	int y;
	
// Allocate memory for mod structure	
	m = (ModMusic*)malloc(sizeof(ModMusic));

	m->channel_num = 32;

// Get title	
	memcpy(m->title, &c[4], 20);
	m->title[20]=0;

	m->saved_track_num = c[0x18] | (c[0x19] << 8);
	m->pattern_num = c[0x1a] | (c[0x1b] << 8);
	m->pattern_num++;
	m->xsz_len = c[0x1c] | (c[0x1d] << 8);
	m->sample_num = c[0x1e];
	m->beats_per_track = c[0x20];
	m->track_num = c[0x21];

// Pattern row sizes are always 64
	for(x = 0; x < m->pattern_num; x++)
		m->pattern_row_num[x] = 64;
	
	m->sample = malloc(sizeof(ModSample) * m->sample_num);
	
	for(x = 0; x < m->sample_num; x++)
	{
		y = 0x42 + (x*37);
		memcpy(m->sample[x].name, &c[y], 22);
		m->sample[x].name[22] = 0;
		y+=22;
		m->sample[x].length        = c[y] | (c[y+1]<<8) | (c[y+2]<<16) | (c[y+3]<<24);
		m->sample[x].repeat_off  = c[y+4] | (c[y+5]<<8) | (c[y+6]<<16) | (c[y+7]<<24);
		m->sample[x].repeat_len  = c[y+8] | (c[y+9]<<8) | (c[y+10]<<16) | (c[y+11]<<24);
		m->sample[x].finetune = c[y+12];
		m->sample[x].volume = c[y+13];
		
		if(c[y+14] & 1)
			m->sample[x].bits = 16;
		else
			m->sample[x].bits = 8;

		m->sample[x].data = malloc(m->sample[x].length);
		y+=15;
	}
	
	memcpy(m->pattern_tbl, &c[y], 128);
	y+=128;

	m->track_data = malloc(192 * m->track_num);

	memcpy(m->track_data, &c[y], 192 * m->track_num);

	y += 192 * m->track_num;

	m->track_seq_data = malloc(m->pattern_num * 32 * sizeof(short));

	for(x = 0; x < m->pattern_num; x++)
	{
		m->track_seq_data[x] = c[y] | (c[y+1]<<8);
		y+=2;
	}

	y += m->xsz_len;

	for(x = 0; x < m->sample_num; x++)
	{
		memcpy(m->sample[x].data, &c[y], m->sample[x].length);
		y += m->sample[x].length;
	}

	m->song_pos = 0;
	m->pat_pos = 0;
	m->divisions_sec = 7;
	m->beats_minute = 125;
	m->ticks_division = 6;
	
	for(x = 0; x<8;x++)
	{
		m->old_samples[x] = 1;
		m->old_periods[x] = 0;
	}
	
	m->cur_tick = 0;
	m->fmt = MOD_FMT_MTM;
	
// MTM has no instruments!
	m->instrument_num = 0;
	
	return m;
}

void MODPlay_MTM(ModMusic *m,int *t)
{
	int cur_pat = m->pattern_tbl[m->song_pos];
	int cur_pat_pos = m->pat_pos;
	unsigned char b[4];
	int s, p, e,x,y, w;
	int do_not_increase_pat = 0;
	int v1, v2,v3,f;
	
	if(*t == 0)
		return;

		
	modplay_int_cnt++;

	if(modplay_int_cnt == (50 / m->divisions_sec))
	{
		m->cur_tick = 0;
	}

	if((modplay_int_cnt % (50 / (m->divisions_sec*m->ticks_division))==0) &&
		modplay_int_cnt>0)
	{
		m->cur_tick++;
	}

	if(modplay_int_cnt != (50 / m->divisions_sec))
		return;
	
	for(x = 0; x < m->channel_num; x++)
	{
	//	printf("there!\n");
	//	memcpy(b, &m->pattern_data[(cur_pat * ((4*m->channel_num)*64)) + (cur_pat_pos * (4*m->channel_num)) + (x*4)], 4);

		y = m->track_seq_data[(cur_pat*32)+x];

		if(y == 0)
			b[0] = b[1] = b[2] = b[3] = 0;
		else
		{
			y =((y-1)*192)+(cur_pat_pos*3);
			b[0] = m->track_data[y];
			b[1]= m->track_data[y+1];
			b[2] = m->track_data[y+2];
		}

	// Get sample
		s = (b[2] & 0xf0)>>4;
		s |= b[0] & 0xf0;
		
	// Get period
		p = b[1];
		p|= (b[0] & 0xf)<<8;
		p&=~(2048|1024);
	
	// Get effect
		e = b[3];
		e|= (b[2] & 0xf)<<8;

		if(s != 0 && p==0)
			p = m->old_periods[x];	
		
		if(s == 0 && p != 0)
			s=m->old_samples[x];

		v1 = m->sample[s-1].volume;
		
		switch(e & 0xf00)
		{
			case 0xc00: // Set volume
				v1 = e & 0xff;
			break;
		}
				
		f = -1;
		
		for(y = 0; y < sizeof(modplay_pitch_per_tbl) / 4; y++)
		{
			if(modplay_pitch_per_tbl[y<<1] == p)
			{
				f = modplay_pitch_per_tbl[(y<<1)+1];
				break;
			}
		}
		
		if(f==-1 && p!=0)
		{
			printf("Couldn't find period %d in table. Calculating it.\n", p);
			f = SsFreqToPitch(7159090/(p*2));
		}

		f+=m->transpose;

		if(f<0)f=0;
		else if(f>0x3fff)f=0x3fff;

		v1 <<= 8;
		
		if(v1 >= 0x4000)
			v1 = 0x3fff;
		

		if(s && p!=0)
		{
			if(!(x&1))
				MODPlay_func(m, x, s-1, f, v1, 0);
			else
				MODPlay_func(m, x, s-1, f, 0, v1);
		}
			
		switch(e & 0xf00)
		{
			case 0xb00: // Position Jump
				m->song_pos = e & 0xff;
				m->pat_pos = 0;
				// printf("Jump to song pos %d\n", m->song_pos);
			
				// this fixes some mods which jump over the mod itself
			
				if(m->song_pos >= m->song_pos_num)
					m->song_pos = 0;
			
				do_not_increase_pat = 1;
			break;
			case 0xd00: // Pattern break
				m->song_pos++;
				m->pat_pos = (((e&0xf0)>>4)*10)+(e&0xf);
				// printf("Pattern break, newpatpos=%d\n", m->pat_pos);
			
				// this fixes some mods which jump over the mod itself
			
				if(m->song_pos >= m->song_pos_num)
					m->song_pos = 0;
			
				do_not_increase_pat = 1;
			break;
			case 0xf00: // Tempo
				/*v1 = (e & 0xf0) >> 4;
				v2 = e & 0xf;
				v3 = (v1*16)+v2;*/
			
				if((e & 0xff) <= 32)
				{
					if((e&0xff) == 0)e++;
					m->ticks_division = e & 0xff;
				}
				else
					m->beats_minute = e & 0xff;
			
				m->divisions_sec = 24 * m->beats_minute;
				m->divisions_sec /= m->ticks_division;
				m->divisions_sec /= 60;
			break;
		}
		
		if(s) m->old_samples[x] = s;
		if(p) m->old_periods[x] = p;

		modplay_int_cnt = 0;
	}
	
	if(!do_not_increase_pat)m->pat_pos++;
	
	if(m->pat_pos == 64)
	{
		m->song_pos++;
		if(m->song_pos >= m->song_pos_num)
		{
			*t-=1;

			MODRewind(m);
		}
			
		m->pat_pos = 0;
	}
}
