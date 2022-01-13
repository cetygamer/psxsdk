/**
 * PSXSDK
 *
 * Sound Processing Unit Functions
 * Based on code from James Higgs's PSX lib and on code by bitmaster
 */

#include <stdio.h>
#include <string.h>
#include <psx.h>

#define SPU_ADDR				*((unsigned short*)0x1f801da6)
#define SPU_DATA				*((unsigned short*)0x1f801da8)
#define SPU_CONTROL 			*((unsigned short*)0x1f801daa)
#define SPU_STATUS			*((unsigned short*)0x1f801dac)
#define SPU_STATUS2			*((unsigned short*)0x1f801dae)
#define SPU_MVOL_L			*((unsigned short*)0x1f801d80)
#define SPU_MVOL_R			*((unsigned short*)0x1f801d82)
#define SPU_REVERB_L			*((unsigned short*)0x1f801d84)
#define SPU_REVERB_R			*((unsigned short*)0x1f801d86)
#define SPU_KEY_ON1           		*((unsigned short*)0x1f801d88)
#define SPU_KEY_ON2	            	*((unsigned short*)0x1f801d8a)
#define SPU_KEY_OFF1           		*((unsigned short*)0x1f801d8c)
#define SPU_KEY_OFF2            	*((unsigned short*)0x1f801d8e)
#define SPU_KEY_FM_MODE1		*((unsigned short*)0x1f801d90)
#define SPU_KEY_FM_MODE2		*((unsigned short*)0x1f801d92)
#define SPU_KEY_NOISE_MODE1		*((unsigned short*)0x1f801d94)
#define SPU_KEY_NOISE_MODE2		*((unsigned short*)0x1f801d96)
#define SPU_KEY_REVERB_MODE1		*((unsigned short*)0x1f801d98)
#define SPU_KEY_REVERB_MODE2		*((unsigned short*)0x1f801d9a)
#define SPU_CD_MVOL_L			*((unsigned short*)0x1f801db0)
#define SPU_CD_MVOL_R			*((unsigned short*)0x1f801db2)
#define SPU_EXT_VOL_L			*((unsigned short*)0x1f801db4)
#define SPU_EXT_VOL_R			*((unsigned short*)0x1f801db6)
#define SPU_REVERB_WORK_ADDR		*((unsigned short*)0x1f801da2)
#define SPU_VOICE_BASE_ADDR(x)		(0x1f801c00 + (x<<4))

// DPCR and other DMA defines will be eventually shared between GPU and SPU

#define DPCR				*((unsigned int*)0x1f8010f0)

static unsigned int ss_vag_addr;

/**
 * Set voice volume.
 * @param voice Voice number (0-23)
 * @param left Left channel volume
 * @param right Right channel volume
 */

void SsVoiceVol(int voice, unsigned short left, unsigned short right)
{
	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);

	a[0] = left;
	a[1] = right;
}

/**
 * Set voice pitch.
 * @param pitch Pitch.
 */

void SsVoicePitch(int voice, unsigned short pitch)
{
	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);
	
	a[2] = pitch;
}

/**
 * Set start Sound RAM address for voice.
 * @param voice Voice
 * @param addr Start address in Sound RAM (multiplier of 8)
 */

void SsVoiceStartAddr(int voice, unsigned int addr)
{
// address given is real address, then it is divided by eight when written to the register	
// example: SSVoiceStartAddr(0, 0x1008) , writes 0x201 on the register which means 0x1008
	
	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);
	
	a[3] = (addr >> 3);
}

/**
 * Set ADSR level for voice
 * @param voice Voice
 * @param level ADSR level
 * @param rate ADSR rate
 */

void SsVoiceADSRRaw(int voice, unsigned short level, unsigned short rate)
{
	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);

	a[4] = level;
	a[5] = rate;
}

/**
 * Set repeat address for voice
 * @param voice Voice
 * @param addr Address in Sound RAM (multiplier of 8)
 */

void SsVoiceRepeatAddr(int voice, unsigned int addr)
{
// only valid after KeyOn
// the explanation for SSVoiceStartAddr() is valid for this function as well

	unsigned short *a = (unsigned short*)SPU_VOICE_BASE_ADDR(voice);
	
	a[7] = (addr >> 3);
}

/**
 * Set a voice to 'on'. This has the effect of playing the sound specified for the voice.
 * @param voice Voice
 */

void SsKeyOn(int voice)
{
	unsigned int i = 1 << voice;
	
	SPU_KEY_ON1 = i & 0xffff;
	SPU_KEY_ON2 = i >> 16;
	
/*	while(SPU_KEY_ON1 != (i & 0xffff));
	while(SPU_KEY_ON2 != (i >> 16));
*/
}

/**
 * Set a voice to 'off'. This stops the sound specified for the voice.
 * @param voice Voice
 */

void SsKeyOff(int voice)
{
	unsigned int i = 1 << voice;
	
	SPU_KEY_OFF1 = i & 0xffff;
	SPU_KEY_OFF2 = i >> 16;
}

/**
 * Set the voices specified by the bitmask to 'on'. Like SsKeyOn()
 * @param mask Bitmask
 */

void SsKeyOnMask(int mask)
{
	SPU_KEY_ON1 = mask & 0xffff;
	SPU_KEY_ON2 = mask >> 16;
}

/**
 * Set the voices specified by the bitmask to 'off'. Like SsKeyOff()
 * @param mask Bitmask
 */

void SsKeyOffMask(int mask)
{
	SPU_KEY_OFF1 = mask & 0xffff;
	SPU_KEY_OFF2 = mask >> 16;
}

/**
 * Wait for the SPU to be ready.
 */

void SsWait()
{
	while(SPU_STATUS2 & 0x7ff);
}

/**
 * Intialize the SPU.
 */

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

// SsUpload is originally based on code by bitmaster

/**
 * Uploads sound data in PSX ADPCM format to Sound RAM.
 * @param addr Pointer to PSX ADPCM sound data in main RAM
 * @param size Sound data size
 * @param spu_addr Destination address in Sound RAM (multiplier of 8).
 */

void SsUpload(void *addr, int size, int spu_addr)
{
	short spu_status; 
	int block_size;
	short *ptr;
	short d;
	int i;

	spu_status = SPU_STATUS2 & 0x7ff;

	SPU_ADDR = spu_addr >> 3;

	for(i=0;i<100;i++); // Waste time...

	ptr = (short *) addr;

	while(size > 0) 
	{
		block_size = ( size > 64 ) ? 64 : size; 

		for( i = 0; i < block_size; i += 2 )
			SPU_DATA = *ptr++;     

		d = SPU_CONTROL;
		d = ( d & 0xffcf ) | 0x10;
		SPU_CONTROL = d;	// write Block to SPU-Memory

		for(i=0;i<100;i++) // Waste time

		while(SPU_STATUS2 & 0x400);

		for(i=0;i<200;i++); // Waste time

		size -= block_size;
	}     

	SPU_CONTROL &= 0xffcf;

	while( ( SPU_STATUS2 & 0x7ff ) != spu_status ); 
}

/**
 * Converts a sampling rate in hertz to PlayStation pitch rate used by the SPU.
 * @param hz Sampling rate in hertz.
 * @return PlayStation pitch rate
 */

unsigned short SsFreqToPitch(int hz)
{
// Converts a normal samples per second frequency value in Hz
// in a pitch value

// i.e. 44100 -> 0x1000, 22050 -> 0x800	
	
	return (hz << 12) / 44100;
}

/**
 * Reads information from a buffer containg a VAG file and stores it inside a SsVag structure.
 * @param vag Pointer to structure in which to store information.
 * @param data Pointer to VAG file data
 */

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

/**
 * Uploads the sound data specified by a SsVag structure to the specified address in Sound RAM.
 * The SsVag structure can then be used for playing with SsPlayVag()
 * @param vag Pointer to SsVag structure
 * @param spu_addr Destination address in Sound RAM (multiplier of 8)
 */

void SsUploadVagEx(SsVag *vag, int spu_addr)
{
	vag->spu_addr = spu_addr;
	SsUpload(vag->data, vag->data_size, vag->spu_addr);
	//spu_addr += vag->data_size;
}

/**
 * Uploads the sound data specified by a SsVag structure to Sound RAM, beginning from the
 * base of usable Sound RAM and continuing from there, in an automatic fashion.
 * @param vag Pointer to SsVag structure
 */

void SsUploadVag(SsVag *vag)
{
	vag->spu_addr = ss_vag_addr;
	SsUploadVagEx(vag, ss_vag_addr);
	ss_vag_addr += vag->data_size;
}

/**
 * Plays the sound specified by the SsVag structure at specified voice and volume.
 * @param vag Pointer to SsVag structure
 * @param voice Voice
 * @param vl Left channel volume
 * @param vr Right channel volume
 */

void SsPlayVag(SsVag *vag, unsigned char voice, unsigned short vl, 
	unsigned short vr)
{
	SsVoicePitch(voice, SsFreqToPitch(vag->sample_rate));
	SsVoiceStartAddr(voice, vag->spu_addr);
	SsVoiceVol(voice, vl, vr);
	SsKeyOn(voice);
	
	vag->cur_voice = voice;
}

/**
 * Stops the sound specified by the SsVag structure
 * @param vag Pointer to SsVag structure
 */

void SsStopVag(SsVag *vag)
{
	SsKeyOff(vag->cur_voice);
	vag->cur_voice = -1;
}

/**
 * Tell SsUploadVag() to start uploading from the base of usable Sound RAM again.
 */

void SsResetVagAddr()
{
	ss_vag_addr = SPU_DATA_BASE_ADDR;
}

/**
 * Enable CD Audio.
 */

void SsEnableCd()
{
	SPU_CONTROL |= 1;
	CdSendCommand(CdlDemute, 0);
}

/**
 * Enable External audio. (???)
 */

void SsEnableExt()
{
	SPU_CONTROL |= 2;
}

/**
 * Set CD Audio volume.
 * @param left Left channel volume
 * @param right Right channel volume
 */

void SsCdVol(unsigned short left, unsigned short right)
{
	SPU_CD_MVOL_L = left;
	SPU_CD_MVOL_R = right;
}
