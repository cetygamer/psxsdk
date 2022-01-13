#ifndef _PSXCDROM_H
#define _PSXCDROM_H

#define CDSTATUS_PLAY		0x80
#define CDSTATUS_SEEK		0x40

// Command names

enum
{
	CdlSync, CdlNop, CdlSetloc, CdlPlay,
	CdlForward, CdlBackward, CdlReadN, CdlStandby,
	CdlStop, CdlPause, CdlInit, CdlMute,
	CdlDemute, CdlSetfilter, CdlSetmode, CdlSetparam,
	CdlGetlocL, CdlGetlocP, CdlCmd18, CdlGetTN,
	CdlGetTD, CdlSeekL, CdlSeekP, CdlCmd23,
	CdlCmd24, CdlTest, CdlID, CdlReadS,
	CdlReset, CdlCmd29, CdlReadTOC
};

/*
 * Send a low-level CDROM command
 * cmd = command number
 * num = number of arguments
 * ... = arguments
 */

void CdSendCommand(int cmd, int num, ...);

/*
 * Reads the results of a low-level CDROM command
 *
 * out = pointer to array of chars where the output will be stored
 * max = maximum number of bytes to store
 *
 * Return value: number of results.
 */

int CdReadResults(unsigned char *out, int max);

/*
 * Gets CDROM drive status
 */

int CdGetStatus();

/*
 * To play an Audio CD track
 */

int CdPlayTrack(unsigned int track);

#endif
