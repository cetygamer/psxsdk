/**
 * MODPlay
 *
 * XM (FastTracker 2.00) file format support
 *
 * by Giuseppe Gatta, 2013
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "modplay.h"

// I love data alignment so much..

#define xmWord(addr)			__xmWord_(xmData, addr)
#define xmDWord(addr)			__xmDWord_(xmData, addr)

static unsigned short __xmWord_(unsigned char *c, unsigned int addr)
{
	return c[addr] | (c[addr+1]<<8);
}

static unsigned int __xmDWord_(unsigned char *c, unsigned int addr)
{
	return c[addr] | (c[addr+1]<<8) | (c[addr+2]<<16) | (c[addr+3]<<24);
}

//#define XMprintf	printf
#define XMprintf

static void *xmUpdateSpeed(ModMusic *m)
{
	m->divisions_sec = 24 * m->beats_minute;
	m->divisions_sec /= m->ticks_division;
	m->divisions_sec /= 60;
}

ModMusic *MODLoad_XM(void *d)
{
	unsigned char *xmData = d;
	ModMusic *music;
	int i, j, l, m;
	unsigned char c;
	unsigned short s;
	int totalSamples=0;

// Check ID text
	
	if(memcmp(&xmData[0], "Extended Module: ",
		17) != 0)
		return NULL;

// Tested only with version 0x0104 files.	
	
	if(xmWord(58) != 0x0104)
		return NULL;
	
// Allocate memory for ModPlay structure	
	music = malloc(sizeof(ModMusic));
	
	if(music == NULL)
		return NULL;

// Fill structure's title field
	
	memcpy(music->title, &xmData[17], 20);
	music->title[20] = 0;
	
// Ignore tracker name
	
/**** XM HEADER VARIABLES ****/
	unsigned int xmHeaderSize = xmDWord(60);
	unsigned short xmSongLength = xmWord(64);
	XMprintf("xmSongLength = %d\n", xmSongLength);
	unsigned short xmRestartPosition = xmWord(66);
	unsigned short xmNumOfChannels = xmWord(68);	
	unsigned short xmNumOfPatterns = xmWord(70);
	unsigned short xmNumOfInstruments = xmWord(72);
	unsigned short xmFlags = xmWord(74);
	unsigned short xmDefaultTempo = xmWord(76);
	unsigned short xmDefaultBPM = xmWord(78);
	unsigned char *xmPatternOrderTable = &xmData[80];
	
	music->song_pos_num = xmSongLength;
	music->channel_num = xmNumOfChannels;
	music->pattern_num = xmNumOfPatterns;
	music->beats_minute = xmDefaultBPM;
	music->ticks_division = xmDefaultTempo;
	music->xm_flags = xmFlags;
	
	xmUpdateSpeed(music);
	
	memcpy(music->pattern_tbl, xmPatternOrderTable, 256);
/***********************************/

	XMprintf("xmFlags = %d\n", xmFlags);
	
	// 336
	int curPtr = 336;
	
	int oldCurPtr = curPtr;
	int patternDataSize = 0;
		
// First get size of all pattern data
	for(i = 0; i < xmNumOfPatterns; i++)
	{
		unsigned int xmPatternHeaderLength = xmDWord(curPtr);
		unsigned short xmPatternNumOfRows = xmWord(curPtr+5);
		unsigned short xmPatternPackedSize = xmWord(curPtr+7);

		curPtr += xmPatternHeaderLength + xmPatternPackedSize;
		patternDataSize += (xmPatternNumOfRows * 5 * xmNumOfChannels); 
	}
		
	music->pattern_data = malloc(patternDataSize);
	
	curPtr = oldCurPtr;
	
	int patternDataPos = 0;
	
	for(i = 0; i < xmNumOfPatterns; i++)
	{
		XMprintf("Pattern %d\n", i);
		
		unsigned int xmPatternHeaderLength = xmDWord(curPtr);
		XMprintf("xmPatternHeaderLength = %d\n", xmPatternHeaderLength);
		unsigned char xmPackingType = xmData[curPtr+4];
		XMprintf("xmPackingType = %d\n", xmPackingType);
		unsigned short xmPatternNumOfRows = xmWord(curPtr+5);
		XMprintf("xmPatternNumOfRows = %d\n", xmPatternNumOfRows);
		unsigned short xmPatternPackedSize = xmWord(curPtr+7);
		XMprintf("xmPatternPackedSize = %d\n", xmPatternPackedSize);
		
		music->pattern_row_num[i] = xmPatternNumOfRows;
		
		curPtr += xmPatternHeaderLength;
		
		//printf("CURPTR= %d\n", curPtr);
		

				
		//curPtr = oldCurPtr;	
		oldCurPtr = curPtr;
				
		//printf("RESULT: %d\n", curPtr - oldCurPtr);	

				
		for(j = 0; j < xmPatternNumOfRows; j++)
		{
			for(l = 0; l < xmNumOfChannels; l++)
			{
				unsigned char rowNote = xmData[curPtr];
				unsigned char rowInstrument=0;
				unsigned char rowVolumeColumn=0;
				unsigned char rowEffectType=0;
				unsigned char rowEffectParameter=0;
			
				if(rowNote & 128) // Packing enabled
				{
					unsigned char packInfo = rowNote;
					//printf("Packing found.. %02x\n", packInfo);
				
					rowNote = 0;
					curPtr++;
				
					if(packInfo & 1) // Note follows
						rowNote = xmData[curPtr++];
					if(packInfo & 2) // Insturment follows
						rowInstrument = xmData[curPtr++];
					if(packInfo & 4)
						rowVolumeColumn = xmData[curPtr++];
					if(packInfo & 8)
						rowEffectType = xmData[curPtr++];
					if(packInfo & 16)
						rowEffectParameter = xmData[curPtr++];
				}
				else
				{
					rowInstrument = xmData[curPtr+1];
					rowVolumeColumn = xmData[curPtr+2];
					rowEffectType = xmData[curPtr+3];
					rowEffectParameter = xmData[curPtr+4];
					curPtr+=5;
				}

// The XM player code will always want unpacked pattern data.				
				music->pattern_data[patternDataPos++] = rowNote;
				music->pattern_data[patternDataPos++] = rowInstrument;
				music->pattern_data[patternDataPos++] = rowVolumeColumn;
				music->pattern_data[patternDataPos++] = rowEffectType;
				music->pattern_data[patternDataPos++] = rowEffectParameter;
			}
		}
			
			//printf("CURPTR=%d\n", curPtr);
		
		//curPtr += xmPatternHeaderLength + xmPatternPackedSize;
	}

	oldCurPtr = curPtr;
		
	for(l = 0; l < 2; l++)
	{
		// l = 0, PASS 1. Find number of samples in XM.
		// l = 1, PASS 2. Allocate & fill instrument and sample structures.
		
		
		curPtr = oldCurPtr;
		
		if(l == 1)
		{
			music->sample_num = totalSamples;
			music->instrument_num = xmNumOfInstruments;
			music->sample = malloc(sizeof(ModSample) * totalSamples);
			music->instrument = malloc(sizeof(ModInstrument) * xmNumOfInstruments);
		}
		
		int currentSample = 0;
		
		for(i = 0; i < xmNumOfInstruments; i++)
		{
			XMprintf("curPtrOff=%x\n", curPtr);

		//	printf("Instrument %d\n", i);
		
			unsigned int instrumentHeaderSize = xmDWord(curPtr);
			unsigned char *instrumentName = &xmData[curPtr+4];
		//	printf("instrumentName = %s\n", instrumentName);
			unsigned char instrumentType = xmData[curPtr+26];
			unsigned short instrumentNumOfSamples = xmWord(curPtr+27);
			
			XMprintf("instrumentNumOfSamples = %d\n", instrumentNumOfSamples);
			
			if(l == 0)
			{
				totalSamples += instrumentNumOfSamples;
				XMprintf("totalSamples=%d\n", totalSamples);
				XMprintf("%d: %s\n", i, instrumentName);
			}
			else
			{
				memcpy(music->instrument[i].name, instrumentName, 22);
				music->instrument[i].name[22] = 0;
				music->instrument[i].sample_num = instrumentNumOfSamples;
			}
			
		//printf("instrumentNumOfSamples = %d\n", instrumentNumOfSamples);
		
			unsigned int sampleHeaderSize = xmDWord(curPtr + 29);
			unsigned char *sampleNumberAllNotes = &xmData[curPtr + 33];
			unsigned char *samplePointsVolEnvelope = &xmData[curPtr + 129];
			unsigned char *samplePointsPanEnvelope = &xmData[curPtr + 177];
			unsigned char sampleNumOfVolumePoints = xmData[curPtr + 225];
			unsigned char sampleNumOfPanningPoints = xmData[curPtr + 226];
			unsigned char sampleVolumeSustainPoint = xmData[curPtr + 227];
			unsigned char sampleVolumeLoopStartPoint = xmData[curPtr + 228];
			unsigned char sampleVolumeLoopEndPoint = xmData[curPtr + 229];
			unsigned char samplePanningSustainPoint = xmData[curPtr + 230];
			unsigned char samplePanningLoopStartPoint = xmData[curPtr + 231];
			unsigned char samplePanningLoopEndPoint = xmData[curPtr + 232];
			unsigned char sampleVolumeType = xmData[curPtr + 233];
			unsigned char samplePanningType = xmData[curPtr + 234];
			unsigned char sampleVibratoType = xmData[curPtr + 235];
			unsigned char sampleVibratoSweep = xmData[curPtr + 236];
			unsigned char sampleVibratoDepth = xmData[curPtr + 237];
			unsigned char sampleVibratoRate = xmData[curPtr + 238];
			unsigned short sampleVolumeFadeout = xmDWord(curPtr+239);
		
			XMprintf("instrumentHeaderSize = %d\n", instrumentHeaderSize);
			curPtr += instrumentHeaderSize;
		
			for(j = 0; j < instrumentNumOfSamples; j++)
			{
				unsigned int sampleLength = xmDWord(curPtr);
				unsigned int sampleLoopStart = xmDWord(curPtr+4);
				unsigned int sampleLoopLength = xmDWord(curPtr+8);
				unsigned char sampleVolume = xmData[curPtr+12];
				unsigned char sampleFinetune = xmData[curPtr+13];
				unsigned char sampleType = xmData[curPtr+14];
				unsigned char samplePanning = xmData[curPtr+15];
				char sampleRelativeNoteNumber = xmData[curPtr+16];
				char sampleDataType = xmData[curPtr+17];
				
				XMprintf("sampleDataType = %02x, sampleLength=%d\n", sampleDataType,
					sampleLength);
				
				char *sampleName = &xmData[curPtr+18];
				XMprintf("sampleName[%d] = %s ( %s)\n", j, sampleName,
					(sampleType & 16)?"16-bit":"8-bit");
			
				char *sampleData = &xmData[curPtr + sampleHeaderSize];
			
				//curPtr+=sampleHeaderSize;
				
				if(l == 1)
				{
					memcpy(music->sample[currentSample].name, sampleName, 22);
					music->sample[currentSample].name[22] = 0;
					music->sample[currentSample].length = sampleLength;
					music->sample[currentSample].finetune = sampleFinetune;
					music->sample[currentSample].volume = sampleVolume;
					music->sample[currentSample].repeat_off = sampleLoopStart;
					music->sample[currentSample].repeat_len = sampleLoopLength;
					music->sample[currentSample].bits = (sampleType & 16) ? 16: 8;
					music->sample[currentSample].data_type = sampleDataType;
					music->sample[currentSample].data = malloc(sampleLength);
					
					/**** XM SPECIFIC ****/
					music->sample[currentSample].relative_note = sampleRelativeNoteNumber;
					
					switch(music->sample[currentSample].bits)
					{
						case 8:
							for(m = 0, c = 0; m < sampleLength; m++)
							{
								c += sampleData[m];
								music->sample[currentSample].data[m] = c ^ 0x80; 
							}
						break;
						case 16:
							for(m = 0, s = 0; m < sampleLength; m+=2)
							{
								s += (sampleData[m+1] << 8) | sampleData[m];
								//s ^= 0x8000;
								memcpy(&music->sample[currentSample].data[m], &s, sizeof(short));
							}
						break;
					}
					
					music->instrument[i].sample_ids[j] = currentSample;
					currentSample++;
				}
				
				curPtr += sampleHeaderSize + sampleLength;
			
				XMprintf("sampleHeaderSize = %d\n", sampleHeaderSize);
//			curPtr += sampleLength;
			
			/*unsigned char xmSampleNumber = xmData[curPtr + 33];
			unsigned char xmSamplePointsVolEnvelope = xmData[curPtr + 34];
			unsigned char xmSamplePointsPanEnvelope = xmData[cur*/
			//curPtr += sample
			}
		//curPtr += instrumentHeaderSize;
		}
	}
/***********************************/	
	
	music->fmt = MOD_FMT_XM;
	music->song_pos = 0;
	music->pat_pos = 0;
	
	return music;
}

void MODPlay_XM(ModMusic *m, int *t)
{
	int c, i;
	
	if(*t == 0)
		return;
	
	modplay_int_cnt++;

	if(modplay_int_cnt != (50 / m->divisions_sec))
		return;
	
	for(c = 0; c < m->channel_num; c++)
	{
		int curPat = m->pattern_tbl[m->song_pos];
		int curDataPos = 0;
		
// Find start of data for the current pattern		
		for(i = 0; i < curPat; i++)
			curDataPos += (m->pattern_row_num[i] * 5 * m->channel_num);
		
		

	}
}
