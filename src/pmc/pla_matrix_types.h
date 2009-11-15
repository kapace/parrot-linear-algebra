#ifndef _PLA_MATRIX_TYPES_H_
#define _PLA_MATRIX_TYPES_H_

#include <cblas.h>

#define SWAP_XY(a) do { \
        const INTVAL __temp_val = a->rows; \
        a->rows = a->cols; \
        a->cols = __temp_val; \
    } while(0);

#define GET_KEY_INDICES_ROWMAJOR(i, k, row, col) \
do { \
    (row) = VTABLE_get_integer((i), (k)); \
    (k) = VTABLE_shift_pmc((i), (k)); \
    (col) = VTABLE_get_integer((i), (k)); \
} while(0);

#define GET_KEY_INDICES_COLMAJOR(i, k, row, col) \
do { \
    (col) = VTABLE_get_integer((i), (k)); \
    (k) = VTABLE_shift_pmc((i), (k)); \
    (row) = VTABLE_get_integer((i), (k)); \
} while(0);

#define INDEX_MIN(a, b) (((a) <= (b))?(a):(b))
#define INDEX_MAX(a, b) (((a) >= (b))?(a):(b))

/* MEMORY LAYOUT FOR REAL MATRICES

    Memory will be laid out by default in row-major indexing:

    a->storage = [row][row][row][row] ...

    Indexing in row-major format:

    m[2, 3] = second row, third item in the row
            = a->storage[2 * rowsize + 3]

    Indexing in column-major format:

    m[2, 3] = second column, third item in the column
            = a->storage[3 * rowsize + 2]

    When we transpose, we swap the number of rows and columns. So even
    though internally it's still technically the "rowsize", this is how
    we calculate column-major indices in reality:

    m[2, 3] = a->storage[3 * colsize + 2]

    because the new colsize is the old rowsize
*/

/* Real-Valued Matrix Accessors */
#define INDEX_XY_ROWMAJOR(numrows, numcols, row, col) \
    (((row) * (numcols)) + (col))

#define INDEX_XY_COLMAJOR(numrows, numcols, row, col) \
    (((col) * (numrows)) + (row))

#define INDEX_XY(flags, rowsize, colsize, row, col) \
    (((IS_TRANSPOSED(flags)) ? \
    INDEX_XY_COLMAJOR(rowsize, colsize, row, col) : \
    INDEX_XY_ROWMAJOR(rowsize, colsize, row, col)))

#define ITEM_XY_ROWMAJOR(s, rowsize, colsize, row, col) \
    (s)[INDEX_XY_ROWMAJOR(rowsize, colsize, row, col)]

#define ITEM_XY_COLMAJOR(s, rowsize, colsize, row, col) \
    (s)[INDEX_XY_COLMAJOR(rowsize, colsize, row, col)]

#define ITEM_XY(s, flags, rowsize, colsize, row, col) \
    (s)[INDEX_XY(flags, rowsize, colsize, row, col)]

/* MEMORY LAYOUT FOR COMPLEX MATRICES

    Memory will be laid out by default in row-major indexing:

    a->storage = [row][row][row][row] ...

    Where each row is an alternating pattern of real and imaginary
    values:

    row = [RI][RI][RI] ...

    Indexing in row-major format:

    (real)m[2, 3] = second row, third real item in the row
                  = a->storage[2 * (rowsize * 2) + 3]
    (imag)m[2, 3] = second row, third imaginary item in the row
                  = a->storage[2 * (rowsize * 2) + 3  + 1]

    Indexing in column-major format:

    (real)m[2, 3] = second column, third real item in the column
                  = a->storage[3 * (rowsize * 2) + 2]
    (imag)m[2, 3] = second column, third imag item in the column
                  = a->storage[3 * (rowsize * 2) + 2 + 1]
*/

/* Complex-Valued Matrix Accessors */
#define R_INDEX_XY_ROWMAJOR(numrows, numcols, row, col) \
    (((row) * (numcols) * 2) + (col))
#define I_INDEX_XY_ROWMAJOR(numrows, numcols, row, col) \
    (((row) * (numcols) * 2) + (col) + 1)

#define R_INDEX_XY_COLMAJOR(numrows, numcols, row, col) \
    (((col) * (numrows) * 2) + (row))
#define I_INDEX_XY_COLMAJOR(numrows, numcols, row, col) \
    (((col) * (numrows * 2)) + (row) + 1)

#define R_INDEX_XY(flags, rowsize, colsize, row, col) \
    (((IS_TRANSPOSED(flags)) ? \
    R_INDEX_XY_COLMAJOR(rowsize, colsize, row, col) : \
    R_INDEX_XY_ROWMAJOR(rowsize, colsize, row, col)))
#define I_INDEX_XY(flags, rowsize, colsize, row, col) \
    (((IS_TRANSPOSED(flags)) ? \
    I_INDEX_XY_COLMAJOR(rowsize, colsize, row, col) : \
    I_INDEX_XY_ROWMAJOR(rowsize, colsize, row, col)))

#define R_ITEM_XY_ROWMAJOR(s, rowsize, colsize, row, col) \
    (s)[R_INDEX_XY_ROWMAJOR(rowsize, colsize, row, col)]
#define I_ITEM_XY_ROWMAJOR(s, rowsize, colsize, row, col) \
    (s)[I_INDEX_XY_ROWMAJOR(rowsize, colsize, row, col)]

#define R_ITEM_XY_COLMAJOR(s, rowsize, colsize, row, col) \
    (s)[R_INDEX_XY_COLMAJOR(rowsize, colsize, row, col)]
#define I_ITEM_XY_COLMAJOR(s, rowsize, colsize, row, col) \
    (s)[I_INDEX_XY_COLMAJOR(rowsize, colsize, row, col)]

#define R_ITEM_XY(s, flags, rowsize, colsize, row, col) \
    (s)[R_INDEX_XY(flags, rowsize, colsize, row, col)]
#define I_ITEM_XY(s, flags, rowsize, colsize, row, col) \
    (s)[I_INDEX_XY(flags, rowsize, colsize, row, col)]

/* Matrix flags */
#define FLAG_TRANSPOSED     0x01
#define FLAG_SYMMETRIC      0x02
#define FLAG_HERMITIAN      0x04
#define FLAG_UTRIANGLE      0x08
#define FLAG_LTRIANGLE      0x10
#define FLAG_TRIANGLE       (FLAG_UTRIANGLE) | (FLAG_LTRIANGLE)
#define FLAG_TRIDIAGONAL    0x20
#define FLAG_TINY           0x40
#define FLAG_DIAGONAL       (FLAG_SYMMETRIC) | \
                            (FLAG_HERMITIAN) | \
                            (FLAG_LTRIANGLE) | \
                            (FLAG_UTRIANGLE) | \
                            (FLAG_TRIDIAGONAL)

/* Logical flag checking macros */
#define IS_GENERAL(flags)         ((! (flags)))
#define IS_TINY(flags)            (((flags) & (FLAG_TINY)))
#define IS_SYMMETRIC(flags)       (((flags) & (FLAG_SYMMETRIC)))
#define IS_HERMITIAN(flags)       (((flags) & (FLAG_HERMITIAN)))
#define IS_UTRIANGLE(flags)       (((flags) & (FLAG_UTRIANGLE)))
#define IS_LTRIANGLE(flags)       (((flags) & (FLAG_LTRIANGLE)))
#define IS_TRIANGLE(flags)        (((flags) & (FLAG_TRIANGLE)))
#define IS_DIAGONAL(flags)        ((((flags) & (FLAG_DIAGONAL)) == FLAG_DIAGONAL))
#define IS_TRIDIAGONAL(flags)     (((flags) & (FLAG_TRIDIAGONAL)))
#define IS_TRANSPOSED(flags)      (((flags) & (FLAG_TRANSPOSED)))
#define IS_TRANSPOSED_BLAS(flags) (IS_TRANSPOSED(flags) ? CblasTrans : CblasNoTrans)

#endif /* _PLA_MATRIX_TYPES_H_ */
