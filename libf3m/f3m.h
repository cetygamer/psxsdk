#ifndef _F3M_H
#define _F3M_H

#define F3M_FREQ 44100
#define F3M_BUFLEN 882
#define F3M_CHNS 2

#define F3M_VCHNS 20
#define F3M_PRIO_NORMAL 50
#define F3M_PRIO_MUSIC_OFF 100
#define F3M_PRIO_MUSIC 0x7FFF

typedef struct
{
	uint8_t name[28];
	uint8_t magic[4];
	uint16_t ord_num, ins_num, pat_num;
	uint16_t flags, ver, smptyp;
	uint8_t magic_scrm[4];
	uint8_t gvol, ispeed, itempo, mvol;
	uint8_t uclick, defpanFC;
	uint8_t unused1[8];
	uint16_t special;
	uint8_t cset[32];
	uint8_t extra[];
}__attribute__((__packed__)) mod_s;

typedef struct
{
	uint16_t spu_data;
	uint16_t spu_data_lpbeg;

	int32_t len;
	int32_t len_loop;

	int32_t period;
	int32_t gxx_period;

	int32_t freq;
	int32_t offs;
	uint16_t suboffs;
	int16_t priority;

	int8_t insvol; // assigned on note start
	int8_t midvol; // changed on slides
	int8_t outvol; // actual output	
	uint8_t pan;

	uint8_t vib_offs;
	uint8_t tre_offs;
	uint8_t rtg_count;

	uint8_t eft, efp, lefp, last_note;
	uint8_t lins;
	uint8_t mem_gxx, mem_hxx, mem_oxx;
} vchn_s;

typedef struct
{
	const mod_s *mod;
	const void *modbase;
	const uint16_t *ins_para;
	const uint16_t *pat_para;
	const uint8_t *ord_list;

	int32_t speed, tempo;
	int32_t gvol;
	int32_t ctick, tempo_samples, tempo_wait;
	int32_t cord, cpat, crow;
	const uint8_t *patptr;
	const uint8_t *patptr_next;
	int32_t repeat_row, repeat_count;	

	int sfxoffs;
	int ccount;

	uint16_t psx_spu_offset[99];
	uint16_t psx_spu_offset_lpbeg[99];
	
	uint32_t baseaddr;
	int basevoice;
	int monomode;
	uint16_t maxvolume;

	vchn_s vchn[F3M_VCHNS];
} player_s;

/**
 * Create a module file structure from a data buffer.
 * The data buffer must contain a valid S3M music module file.
 * @param d Pointer to buffer memory
 * @return Module file structure (NULL on error)
 */

mod_s *f3m_mod_load(uint32_t *d);

/**
 * Create a module file structure from a file with the given filename, 
 * The file must be a valid S3M music module file.
 * @param fname Module file filename
 * @return Module file structure (NULL on error)
 */

mod_s *f3m_mod_load_filename(const char *fname);

/**
 * Free the memory associated to a module file structure
 * @param mod Pointer to module file structure
 */

void f3m_mod_free(mod_s *mod);

/**
 * Initialize a music player from a module file structure.
 * This is just like calling f3m_player_init_ex(player, mod, 0, SPU_DATA_BASE_ADDR)
 * @param player Pointer to memory of music player structure
 * @param mod Pointer to module file structure
 * @return Address in SPU memory after uploaded samples
 */

unsigned int f3m_player_init(player_s *player, mod_s *mod);

/**
 * Initialize a music player from a module file structure.
 * @param player Pointer to memory of music player structure
 * @param mod Pointer to module file structure
 * @param basevoice Base voice to use to reproduce samples. Valid range is from 0 to (24 - F3M_VCHNS)
 * @param baseaddr Base address in SPU memory for uploading samples.
 * @return Address in SPU memory after uploaded samples
 */

unsigned int f3m_player_init_ex(player_s *player, mod_s *mod, int basevoice, unsigned int baseaddr);


/**
 * Make a music player play. 
 * This should be called roughly every 1/50th of a second on PAL systems,
 * or every 1/60th of a second on NTSC systems; you can set a flag in a VBlank
 * interrupt handler (see SetVBlankHandler()) to do this.
 * This function is probably not re-entrant, so it is not recommended to call it
 * directly from the VBlank handler.
 * @param player Pointer to music player structure
 */

void f3m_player_play(player_s *player);

/**
 * Stop a music player.
 * @param player Pointer to music player structure
 */

void f3m_player_stop(player_s *player);

/**
 * Set a music player to mono mode. Music players are by default in stereo mode.
 * @param player Pointer to music player structure
 * @param onoff Non-zero for mono mode, zero for stereo mode
 * @return Previous status of mono mode flag.
 */
 
int f3m_set_mono_mode(player_s *player, int onoff);

/**
 * Set maximum volume for a music player. 
 * Default volume for a music player is the maximum allowed by the SPU.
 * @param player Pointer to music player structure
 * @return Previous maximum volume.
 */

uint16_t f3m_set_max_volume(player_s *player, uint16_t maxvolume);

#endif
