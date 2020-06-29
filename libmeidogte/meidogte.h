#ifndef _MEIDOGTE_H
#define _MEIDOGTE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <meidogte_inline.h>

/**
 * One degree = 4096
 */
#define ONE		4096


typedef struct {
	short	m[3][3];
	int		t[3];
} MATRIX;

typedef struct {
	int		vx, vy, vz;
} VECTOR;

typedef struct {
	short	vx, vy, vz, pad;
} SVECTOR;

typedef struct {
	unsigned char r, g, b, cd;
} CVECTOR;

/**
 * Initialize MeidoGTE library
 */

void InitGeom();

/**
 * Integer sine function (4096 = 360 degrees)
 * @param a Input
 * @return Sine of input
 */
int isin(int a);

/**
 * Integer cosine function (4096 = 360 degrees)
 * @param a Input
 * @return Cosine of input
 */
int icos(int a);

/**
 * Higher precision integer sine function (131072 = 360 degrees)
 * @param a Input
 * @return Sine of input
 */
int hisin(int a);
/**
 * Higher precision integer cosine function (131072 = 360 degrees)
 * @param a Input
 * @return Cosine of input
 */
int hicos(int a);

/**
 * Save a constant rotation matrix in stack.
 */
void PushMatrix();

/**
 * Reset a constant rotation matrix from stack.
 */
void PopMatrix();

/**
 * Find rotation matrix from a rotation angle. (4096 = 360 degrees)
 * @param r Rotation angle (input)
 * @param m Rotation matrix (output)
 * @return Pointer to m
 */
 
MATRIX *RotMatrix(SVECTOR *r, MATRIX *m);

/**
 * Find rotation matrix from a rotation angle. (high-precision) (131072 = 360 degrees)
 * @param r Rotation angle (input)
 * @param m Rotation matrix (output)
 * @return Pointer to m
 */
MATRIX *HiRotMatrix(VECTOR *r, MATRIX *m);

/**
 * Give an amount of parallel transfer expressed by v to the matrix m.
 * @param m Pointer to matrix (output)
 * @param v Pointer to transfer vector (input)
 * @return Pointer to m
 */
MATRIX *TransMatrix(MATRIX *m, VECTOR *r);
/**
 * Scale m by v.
 * @param m Pointer to matrix (output)
 * @param v Pointer to scale vector (input)
 * @return Pointer to m
 */
MATRIX *ScaleMatrix(MATRIX *m, VECTOR *s);

/**
 * Multiply two matrices.
 * @param m0 First matrix (result is saved here)
 * @param m1 Second matrix
 * @return Pointer to m0.
 */
MATRIX *MulMatrix(MATRIX *m0, MATRIX *m1);
/**
 * Multiply two matrices.
 * @param m0 First matrix
 * @param m1 Second matrix
 * @param m2 Output matrix
 * @return Pointer to m2
 */
MATRIX *MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2);
/**
 * Make a composite coordinate transformation matrix.
 * @param m0 First matrix
 * @param m1 Second matrix
 * @param m2 Output matrix
 * @return Pointer to m2
 */
MATRIX *CompMatrixLV(MATRIX *v0, MATRIX *v1, MATRIX *v2);
/**
 * Multiply a vector by a matrix.
 * @param m Pointer to matrix to be multiplied
 * @param v0 Pointer to vector (input)
 * @param v1 Pointer to vector (output)
 * @return Pointer to v1
 */
VECTOR *ApplyMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1);
/**
 * Normalize a vector.
 * Warning: if ((v0->vx)^2 + (v1->vx)^2 +(v2->vx)^2) > 0x7FFFFFF,
 * a processor exception will occur.
 * @param v0 Pointer to vector (input)
 * @param v1 Pointer to vector (output)
 */
void VectorNormalS(VECTOR *v0, SVECTOR *v1);
/**
 * Return a vector, obtained by squaring each term of the vector v0, to v1.
 * @param v0 Pointer to vector (input)
 * @param v1 Pointer to vector (output)
 */
void Square0(VECTOR *v0, VECTOR *v1);
/**
 * Square root
 * @param a Input value
 * @return Square root of input value
 */
int SquareRoot0(int a);
/**
 * Square root
 * @param a Input value in (0, 20, 12) format
 * @return Square root of input value in (0, 20, 12) format
 */
int SquareRoot12(int a);

#ifdef __cplusplus
}
#endif

#endif // _MEIDOGTE_H
