/* 
 * GTE Graphics Example (Spinning Cube)
 * 2019 Meido-Tek Productions
 * 2019 modified by nextvolume for PSXSDK
 *
 * licensed under Mozilla Public License v2.0 
 */
 
#include <stdio.h>
#include <string.h>
#include <psx.h>
#include <psxgpu.h>
#include <meidogte.h>

/* Screen resolution */
#define SCREEN_XRES		320
#define SCREEN_YRES		240

/* Screen center position */
#define CENTERX			SCREEN_XRES>>1
#define CENTERY			SCREEN_YRES>>1

#define OT_LEN 0x8000

/* For easier handling of vertex indices */
typedef struct {
	short v0,v1,v2,v3;
} INDEX;

/* Cube vertices */
SVECTOR cube_verts[] = {
	{ -100, -100, -100, 0 },
	{  100, -100, -100, 0 },
	{ -100,  100, -100, 0 },
	{  100,  100, -100, 0 },
	{  100, -100,  100, 0 },
	{ -100, -100,  100, 0 },
	{  100,  100,  100, 0 },
	{ -100,  100,  100, 0 }
};

/* Cube face normals */
SVECTOR cube_norms[] = {
	{ 0, 0, -ONE, 0 },
	{ 0, 0, ONE, 0 },
	{ 0, -ONE, 0, 0 },
	{ 0, ONE, 0, 0 },
	{ -ONE, 0, 0, 0 },
	{ ONE, 0, 0, 0 }
};

/* Cube vertex indices */
INDEX cube_indices[] = {
	{ 0, 1, 2, 3 },
	{ 4, 5, 6, 7 },
	{ 5, 4, 0, 1 },
	{ 6, 7, 3, 2 },
	{ 0, 2, 5, 7 },
	{ 3, 1, 6, 4 }
};

/* Number of faces of cube */
#define CUBE_FACES 6

/* Light color matrix */
/* Each column represents the color matrix of each light source and is */
/* used as material color when using gte_ncs() or multiplied by a */
/* source color when using gte_nccs(). 4096 is 1.0 in this matrix */
/* A column of zeroes disables the light source. */
MATRIX color_mtx = { .m = {
	{ONE, 0, 0},	/* Red   */
	{ONE, 0, 0},	/* Green */
	{ONE, 0, 0}	/* Blue  */ }
};

/* Light matrix */
/* Each row represents a vector direction of each light source. */
/* An entire row of zeroes disables the light source. */
MATRIX light_mtx = { .m = {
	/* X,  Y,  Z */
	{ -2048 , -2048 , -2048},
	{0	  , 0	  , 0},
	{0	  , 0	  , 0} }
};


/* Function declarations */
void init(void);
void display(void);

unsigned int prim_list[OT_LEN];
int current_buf = 0;
volatile int vblank_happened = 0;

GsPoly4 poly[CUBE_FACES];					/* Flat shaded quad primitive */

static void program_vblank_handler(void) {
	vblank_happened = 1;
}

enum {
	OP_SPIN, 
	OP_COLORR1, OP_COLORG1, OP_COLORB1, 
	OP_COLORR2, OP_COLORG2, OP_COLORB2, 
	OP_COLORR3, OP_COLORG3, OP_COLORB3, 
	OP_LIGHTX1, OP_LIGHTY1, OP_LIGHTZ1, 
	OP_LIGHTX2, OP_LIGHTY2, OP_LIGHTZ2, 
	OP_LIGHTX3, OP_LIGHTY3, OP_LIGHTZ3, 
	OP_SCRXOFF, OP_SCRYOFF,
	OP_SCRDEPTH,
	OP_TRANSVX, OP_TRANSVY, OP_TRANSVZ,
	OP_END
};

const char *operationNames[] = {"Change spinning speed",
	"Change light color R1", "Change light color G1", "Change light color B1", 
	"Change light color R2", "Change light color G2", "Change light color B2", 
	"Change light color R3", "Change light color G3", "Change light color B3", 
	"Change light X1", "Change light Y1", "Change light Z1", 
	"Change light X2", "Change light Y2", "Change light Z2", 
	"Change light X3", "Change light Y3", "Change light Z3", 
	"Change screen X offset", "Change screen Y offset",
	"Change screen depth",
	"Change translation vector X", "Change translation vector Y", 
	"Change translation vector Z", 
	};

int stopped = 0;
int currentOperation = 0;

int spinningSpeed = 16;
int screenXOffset = CENTERX;
int screenYOffset = CENTERY;
int screenDepth = CENTERX;
	
VECTOR	pos = { .vx = 0, .vy = 0, .vz = 400 };	/* Translation vector for TransMatrix */
	
#define COLOR_STEP	32
#define LIGHT_STEP		128

/* Main function */
int main() {

	int i,p,wasStart=0,wasLeft=0,wasRight=0;
	int a,b;
	short coord[2];
	unsigned char rgb[3] = {255, 255, 0};
	unsigned short padbuf;
	
	SVECTOR	rot;			/* Rotation vector for Rotmatrix */
	MATRIX	mtx,lmtx;				/* Rotation matrices for geometry and lighting */
	
	bzero(&rot, sizeof(rot));
	
	/* Init graphics and GTE */
	init();

	/* Main loop */
	while( 1 ) {
		/* Set GTE offset (recommended method  of centering) */
		gte_SetGeomOffset( screenXOffset, screenYOffset );
	
		/* Set screen depth (basically FOV control, W/2 works best) */
		gte_SetGeomScreen(screenDepth);
		
		/* Set light ambient color and light color matrix */
		gte_SetBackColor( 63, 63, 63 );
		gte_SetColorMatrix( &color_mtx );
		
		/* Set rotation and translation to the matrix */
		RotMatrix( &rot, &mtx );
		TransMatrix( &mtx, &pos );
		
		/* Multiply light matrix by rotation matrix so light source */
		/* won't appear relative to the model's rotation */
		MulMatrix0( &light_mtx, &mtx, &lmtx );
		
		/* Set rotation and translation matrix */
		gte_SetRotMatrix( &mtx );
		gte_SetTransMatrix( &mtx );
		
		/* Set light matrix */
		gte_SetLightMatrix( &lmtx );
		
		if (!stopped) {
		/* Make the cube SPEEN */
			rot.vx += spinningSpeed;
			rot.vz += spinningSpeed;
		}
			
		for( i=0; i<CUBE_FACES; i++ ) {
			
			/* Load the first 3 vertices of a quad to the GTE */
			gte_ldv3( 
				&cube_verts[cube_indices[i].v0], 
				&cube_verts[cube_indices[i].v1], 
				&cube_verts[cube_indices[i].v2] );
				
			/* Rotation, Translation and Perspective Triple */
			gte_rtpt();
			
			/* Compute normal clip for backface culling */
			gte_nclip();
			
			/* Get result*/
			gte_stopz( &p );
			
			/* Skip this face if backfaced */
			if( p < 0 ) {
				poly[i].attribute = -1;
				continue;
			}
			
			/* Calculate average Z for depth sorting */
			gte_avsz4();
			gte_stotz( &p );
			
			/* Skip if clipping off */
			/* (the shift right operator is to scale the depth precision) */
			if( (p>>2) > OT_LEN ) {
				poly[i].attribute = -1;
				continue;
			}
			
			/* Set the projected vertices to the primitive */
			gte_stsxy0( &coord );
			poly[i].x[0] = coord[0];
			poly[i].y[0] = coord[1];
			
			gte_stsxy1( &coord );
			poly[i].x[1] = coord[0];
			poly[i].y[1] = coord[1];
			
			gte_stsxy2( &coord );
			poly[i].x[2] = coord[0];
			poly[i].y[2] = coord[1];
			
			/* Compute the last vertex and set the result */
			gte_ldv0( &cube_verts[cube_indices[i].v3] );
			gte_rtps();
			gte_stsxy( &coord );
			poly[i].x[3] = coord[0];
			poly[i].y[3] = coord[1];			

			rgb[0] = 255;
			rgb[1] = 255;
			rgb[2] = 255;
			
			gte_ldrgb( &rgb );
			
			/* Load the face normal */
			gte_ldv0( &cube_norms[i] );
			
			/* Normal Color Single */
			gte_nccs();
			
			/* Store result to the primitive */
			gte_strgb( &rgb );
			
			poly[i].r = rgb[0];
			poly[i].g = rgb[1];
			poly[i].b = rgb[2];

			poly[i].attribute = 0;
		}
		
		/* Swap buffers and draw the primitives */
		display();
		
		/* Wait for VBlank to happen */
		vblank_happened = 0;
		while(!vblank_happened);
		
		/* Read joypad status */
		PSX_ReadPad(&padbuf, NULL);
		
		/* Logic for real-time 'configuration' of the cube */
		if ((padbuf & PAD_START) && !wasStart) {
			stopped = !stopped;
			wasStart = 1;
		}
		
		if ((padbuf & PAD_LEFT) && !wasLeft) {
			currentOperation--;
			
			if (currentOperation < 0)
				currentOperation = OP_END - 1;
			
			wasLeft = 1;
		}
		
		if ((padbuf & PAD_RIGHT) && !wasRight) {
			currentOperation++;
			
			if (currentOperation >= OP_END)
				currentOperation = 0;
			
			wasRight = 1;
		}
		
		if (padbuf & PAD_UP) {
			switch(currentOperation) {
				case OP_SPIN:
					spinningSpeed++;
				break;
				case OP_COLORR1 ... OP_COLORB3:
					a = (currentOperation - OP_COLORR1);
					b = a % 3;
					a /= 3;
				
					if(color_mtx.m[b][a] < ONE) 
						color_mtx.m[b][a]+=COLOR_STEP;
				break;
				case OP_LIGHTX1 ... OP_LIGHTZ3:
					a = (currentOperation - OP_LIGHTX1);
					b = a % 3;
					a /= 3;
				
					if(light_mtx.m[a][b] < ONE) 
						light_mtx.m[a][b]+=LIGHT_STEP;
				break;
				case OP_SCRXOFF:
					screenXOffset++;
				break;
				case OP_SCRYOFF:
					screenYOffset++;
				break;
				case OP_SCRDEPTH:
					screenDepth++;
				break;
				case OP_TRANSVX:
					pos.vx++;
				break;
				case OP_TRANSVY:
					pos.vy++;
				break;
				case OP_TRANSVZ:
					pos.vz++;
				break;
			}
		}
		
		if (padbuf & PAD_DOWN) {
			switch(currentOperation) {
				case OP_SPIN:
					spinningSpeed--;
				break;
				case OP_COLORR1 ... OP_COLORB3:
					a = (currentOperation - OP_COLORR1);
					b = a % 3;
					a /= 3;
				
					if(color_mtx.m[b][a] > 0) 
						color_mtx.m[b][a]-=COLOR_STEP;
				break;
				case OP_LIGHTX1 ... OP_LIGHTZ3:
					a = (currentOperation - OP_LIGHTX1);
					b = a % 3;
					a /= 3;
				
					if(light_mtx.m[a][b] > 0) 
						light_mtx.m[a][b]-=LIGHT_STEP;
				break;
				case OP_SCRXOFF:
					screenXOffset--;
				break;
				case OP_SCRYOFF:
					screenYOffset--;
				break;
				case OP_SCRDEPTH:
					screenDepth--;
				break;
				case OP_TRANSVX:
					pos.vx--;
				break;
				case OP_TRANSVY:
					pos.vy--;
				break;
				case OP_TRANSVZ:
					pos.vz--;
				break;
			}
		}
		
		if (!(padbuf & PAD_START))
			wasStart = 0;
		
		if (!(padbuf & PAD_LEFT))
			wasLeft = 0;
		
		if (!(padbuf & PAD_RIGHT))
			wasRight = 0;
	}
	
	return 0;
}

void init(void) {
	/* Initialize PSXSDK */
	PSX_InitEx( 0 );
	/* Initialize the Graphic Synthesizer */
	GsInit();
	/* Clear video memory */
	GsClearMem();
	/* Set primitive list pointer */
	GsSetList( prim_list );
	/* Set automatic GPU waiting */
	GsSetAutoWait();
	/* Set 320x240 video mode */
	GsSetVideoMode(320, 240, EXAMPLES_VMODE);
	/* Set VBlank Handler */
	SetVBlankHandler( program_vblank_handler );
	
	/* Load text font in video memory */
	GsLoadFont(768, 0, 768, 256);

	/* Initialize the MeidoGTE library */
	InitGeom();
}

void display(void) {
	int i;
	
	/* Change display and drawing environment settings */
	/* for double buffering */
	GsSetDispEnvSimple(0, current_buf ? 0 : 256);
	GsSetDrawEnvSimple(0, current_buf ? 256 : 0, SCREEN_XRES, SCREEN_YRES);
	current_buf = !current_buf;
	
	/* Clear the screen with the specified color */
	GsSortCls(63, 0, 127);
	
	/* Add cube faces to drawing list */
	for(i=0;i<CUBE_FACES;i++) {
		if(poly[i].attribute != -1)
			GsSortPoly4(&poly[i]);
	}
	
	/* If stopped, show so on the screen */
	if (stopped)
		GsPrintFont(0, 0, "Stopped");
	
	/* Print current operation name on screen */
	GsPrintFont(0, GsScreenH-16, "%s", operationNames[currentOperation]);
	
	/* Draw list */
	GsDrawList();
}
