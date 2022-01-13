#ifndef _MODPLAY_H
#define _MODPLAY_H

enum
{
	MOD_FMT_MOD, // Ultimate SoundTracker / NoiseTracker / ProTracker
	MOD_FMT_669, // Composer 669
	MOD_FMT_MTM, // MultiTracker
	MOD_FMT_XM, // FastTracker XM
};

typedef struct
{
	char name[23];
	unsigned int length; // Length in bytes
	char finetune;
	unsigned char volume;
	unsigned short repeat_off;
	unsigned short repeat_len;
	unsigned char bits;
	unsigned char data_type;
	unsigned char *data;
	
	/**** XM STUFF ****/
	unsigned char relative_note;
}ModSample;

typedef struct
{
	char name[64];
	int sample_num;
	unsigned char sample_ids[8];
}ModInstrument;

typedef struct
{
	char title[21];
	int sample_num;
	int channel_num;
	int instrument_num;
	ModSample *sample; // 32 for MOD/669, arbitrary for XM
	ModInstrument *instrument;
	unsigned char song_pos_num;
	unsigned char pattern_tbl[256]; // 128 for MOD/669, 256 for XM
	unsigned char pattern_row_num[256];
	unsigned char id[4];
	unsigned char saved_track_num; // Number of saved tracks
	unsigned char track_num; // Number of tracks
	unsigned char pattern_num; // Number of patterns
	unsigned char *pattern_data;
	unsigned short xsz_len; // MultiTracker Extra comment field length
	//unsigned char pattern_order_data[128];
	unsigned char *track_data;
	unsigned short *track_seq_data;
	
	unsigned char tempo_list[128]; // For 669
	unsigned char break_loc_list[128]; // For 669
	
	unsigned char xm_flags; // for XM
	
// Runtime data
	unsigned char song_pos;
	unsigned char pat_pos; // 0-63
	int divisions_sec;
	unsigned char beats_minute;
	unsigned char beats_per_track;
	unsigned char ticks_division;
	unsigned char cur_tick;
	unsigned short old_periods[8];
	unsigned char old_samples[8];
	short transpose; // In PlayStation pitch, this is added to the original sample pitch
				   // and can be used to change the pitch of the music for special effects
	
	unsigned char cur_tempo; // For 699
	unsigned short cur_ticks; // For 669, timing
	
	int fmt;

}ModMusic;

// Allocate a ModMusic structure and copy data to it from 
// data in memory containing a ProTracker module file

ModMusic *MODLoad(void *d);

// Play a tick of a music 
// This has to be called 60 / 50 times per second
// t is a pointer to an int which contains how many times
// the mod file has to be played
// t = 1, play once
// t = 2, play twice
// ...
// t = -1, loop endlessly
//
// MODPlay decreases the value referenced by t every time
// the music finishes. Then if t != 0, the music is restarted
// from the beginning. If t == 0, MODPlay does nothing.
// Set the variable pointed by t when you want to set the number of times again!

void MODPlay(ModMusic *m,int *t);

// Stop MOD
void MODStop(ModMusic *m);

// Rewind MOD (back to beginning)
void MODRewind(ModMusic *m);

// Upload the samples of the module music to Sound RAM
// base_addr is the Sound RAM address to start from when uploading to Sound RAM
// Specifying base_addr as -1 sets the base address to the start of the section for sound data in Sound RAM
// (SPU_DATA_BASE_ADDR). base_addr must be a multiply of 8.
// This function returns the sound address after all the uploaded samples

int MODUploadSamples(ModMusic *m, int base_addr);

// Sets the SPU voice to use as the first channel when playing music.
// The voice for the second channel will then be this (value+1), and so on...
// Usually the base voice is 0. A MOD file can have up to eight channels, so take care of that.
// A 669 file can always have 8 channels.

void MODSetBaseVoice(int base_voice);

// Sets transpose
void MODSetTranspose(ModMusic *m, short transpose);

// Internal function...

void MODPlay_func(ModMusic *m, int c, int s, int p, int vl, int vr);

// MOD4PSX Upload - Upload ADPCM Samples

int MOD4PSX_Upload(void *d, int base_addr);

extern int modplay_int_cnt;

// Free memory allocated for music module

void MODUnload(ModMusic *m);

// Set maximum volume

void MODSetMaxVolume(unsigned short max_volume);

// Set mono mode (0 = stereo, 1 = mono mode)

void MODSetMono(int value);

#endif
