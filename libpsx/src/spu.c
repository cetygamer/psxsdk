/**
 * PSXSDK
 *
 * Sound Processing Unit Functions
 * Based on code from James Higgs's PSX lib and on code by bitmaster
 */

#include <stdio.h>
#include <string.h>
#include <psx.h>

static unsigned int ss_vag_addr;

void SsVoiceVol(int voice, unsigned short left, unsigned short right)
{
	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);

	a[0] = left;
	a[1] = right;
}

void SsVoicePitch(int voice, unsigned short pitch)
{
	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);
	
	a[2] = pitch;
}

void SsVoiceStartAddr(int voice, unsigned int addr)
{
// address given is real address, then it is divided by eight when written to the register	
// example: SSVoiceStartAddr(0, 0x1008) , writes 0x201 on the register which means 0x1008
	
	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);
	
	a[3] = (addr >> 3);
}

void SsVoiceADSRRaw(int voice, unsigned short level, unsigned short rate)
{
	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);

	a[4] = level;
	a[5] = rate;
}

void SsVoiceRepeatAddr(int voice, unsigned int addr)
{
// only valid after KeyOn
// the explanation for SSVoiceStartAddr() is valid for this function as well

	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);
	
	a[7] = (addr >> 3);
}

void SsKeyOn(int voice)
{
	unsigned int i = 1 << voice;
	
	SPU_KEY_ON1 = i & 0xffff;
	SPU_KEY_ON2 = i >> 16;
	
/*	while(SPU_KEY_ON1 != (i & 0xffff));
	while(SPU_KEY_ON2 != (i >> 16));
*/
}

void SsKeyOff(int voice)
{
	unsigned int i = 1 << voice;
	
	SPU_KEY_OFF1 = i & 0xffff;
	SPU_KEY_OFF2 = i >> 16;
}



void SsKeyOnMask(int mask)
{
	SPU_KEY_ON1 = mask & 0xffff;
	SPU_KEY_ON2 = mask >> 16;
}

void SsKeyOffMask(int mask)
{
	SPU_KEY_OFF1 = mask & 0xffff;
	SPU_KEY_OFF2 = mask >> 16;
}

void SsWait()
{
	while(SPU_STATUS2 & 0x7ff);
}

void SsInit()
{
	int x;

	printf("Initializing SPU (Sound Synthesizer)...\n");
	
	DPCR |= 0xB0000;
	
	SPU_MVOL_L = 0x3fff;
	SPU_MVOL_R = 0x3fff;	

	SPU_CONTROL = 0x0;
	SsWait();

	SPU_STATUS = 0x4; // Must be done, but not totally understood

	while(SPU_STATUS2 & 0x7ff);
	
	SPU_REVERB_L = 0x0;
	SPU_REVERB_R = 0x0;

	// All keys off

	SPU_KEY_OFF1 = 0xFFFF;
	SPU_KEY_OFF2 = 0xFFFF;

	// Switch FM, reverb and noise off
	SPU_KEY_FM_MODE1 = 0x0;
	SPU_KEY_FM_MODE2 = 0x0;
	SPU_KEY_NOISE_MODE1 = 0x0;
	SPU_KEY_NOISE_MODE2 = 0x0;
	SPU_KEY_REVERB_MODE1 = 0x0;
	SPU_KEY_REVERB_MODE2 = 0x0;

	// set CD master volume to 0 (mute it)
	SPU_CD_MVOL_L = 0x0;
	SPU_CD_MVOL_R = 0x0;

	// set external input volume to 0 (mute it)
	SPU_EXT_VOL_L = 0x0;
	SPU_EXT_VOL_R = 0x0;

	// set volume of all voices to 0 and adsr to 0,0
	
	for(x = 0; x < 24; x++)
	{
		SsVoiceVol(x, 0, 0);		
		SsVoiceADSRRaw(x, 0, 0);
	}

	SsWait();

	SPU_CONTROL = 0xC000; // SPU is on
	SPU_REVERB_WORK_ADDR = 0xFFFE; // Reverb work address in SPU memory, 0x1fff * 8 = 0xFFF8

	ss_vag_addr = SPU_DATA_BASE_ADDR;
	
	printf("SPU/SS Initialized.\n");
}

// This implementation of SsUpload() was contributed by Shendo 
// It waits either for a period of time or for the status flags to be raised, whichever comes first.
// This makes it work also on ePSXe, which never raises the status flags.

void SsUpload(void *addr, int size, int spu_addr)
{
	unsigned short *ptr = addr;
	int i;
	
	while(size > 0)
	{		
		SPU_STATUS = 4; // Sound RAM Data Transfer Control
		SPU_CONTROL = SPU_CONTROL & ~0x30; // SPUCNT.transfer_mode = 0 (STOP)
	
	
		for(i = 0; i < 100; i++)
		if(((SPU_STATUS2 >> 4) & 3) == 0)break; // wait until SPUSTAT.transfer is 0 (STOP)
	
		SPU_ADDR = spu_addr >> 3;

		for(i = 0; i < 32; i++)
			SPU_DATA = ptr[i];
		
		SPU_CONTROL = (SPU_CONTROL & ~0x30) | 16; // SPUCNT.transfer_mode = 1 (MANUAL)
	
		for(i = 0; i < 100; i++)
		if(((SPU_STATUS2 >> 4) & 3) == 1)break; // wait until SPUSTAT.transfer is 1 (MANUAL)
		
		while(SPU_STATUS2 & 0x400); // wait for transfer busy bit to be cleared
		
		spu_addr += 64;
		ptr += 32;
		size-=64;
	}
}

unsigned short SsFreqToPitch(int hz)
{
// Converts a normal samples per second frequency value in Hz
// in a pitch value

// i.e. 44100 -> 0x1000, 22050 -> 0x800	
	
	return (hz << 12) / 44100;
}

int SsReadVag(SsVag *vag, void *data)
{
	unsigned char *i = data;
	
	if(strncmp(data, "VAGp", 4) != 0)
		return 0;
	
	vag->version = (i[4]<<24)|(i[5]<<16)|(i[6]<<8)|i[7];
	vag->data_size = (i[12]<<24)|(i[13]<<16)|(i[14]<<8)|i[15];
	vag->sample_rate = (i[16]<<24)|(i[17]<<16)|(i[18]<<8)|i[19];
	memcpy(vag->name, &i[32], 16);
	vag->data = &i[48];
	
	return 1;
}

void SsUploadVagEx(SsVag *vag, int spu_addr)
{
	vag->spu_addr = spu_addr;
	SsUpload(vag->data, vag->data_size, vag->spu_addr);
	//spu_addr += vag->data_size;
}

void SsUploadVag(SsVag *vag)
{
	vag->spu_addr = ss_vag_addr;
	SsUploadVagEx(vag, ss_vag_addr);
	ss_vag_addr += vag->data_size;
}

void SsPlayVag(SsVag *vag, unsigned char voice, unsigned short vl, 
	unsigned short vr)
{
	SsVoicePitch(voice, SsFreqToPitch(vag->sample_rate));
	SsVoiceStartAddr(voice, vag->spu_addr);
	SsVoiceVol(voice, vl, vr);
	SsKeyOn(voice);
	
	vag->cur_voice = voice;
}

void SsStopVag(SsVag *vag)
{
	SsKeyOff(vag->cur_voice);
	vag->cur_voice = -1;
}

void SsResetVagAddr()
{
	ss_vag_addr = SPU_DATA_BASE_ADDR;
}

void SsEnableCd()
{
	SPU_CONTROL |= 1;
	CdSendCommand(CdlDemute, 0);
}

void SsEnableExt()
{
	SPU_CONTROL |= 2;
}

void SsCdVol(unsigned short left, unsigned short right)
{
	SPU_CD_MVOL_L = left;
	SPU_CD_MVOL_R = right;
}
