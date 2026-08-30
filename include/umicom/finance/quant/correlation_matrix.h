/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/correlation_matrix.h
 *
 * PURPOSE:
 *   Store and validate bounded symmetric correlation matrices.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CORRELATION_MATRIX_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CORRELATION_MATRIX_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantCorrelationMatrix { double values[16][16]; size_t size; } UmiQuantCorrelationMatrix;
/* Initialise an identity correlation matrix up to sixteen factors. */
UmiStatus umi_quant_correlation_matrix_init(UmiQuantCorrelationMatrix *matrix, size_t size);
/* Set a symmetric correlation coefficient. */
UmiStatus umi_quant_correlation_matrix_set(UmiQuantCorrelationMatrix *matrix, size_t row, size_t column, double correlation);
/* Read one matrix coefficient. */
UmiStatus umi_quant_correlation_matrix_get(const UmiQuantCorrelationMatrix *matrix, size_t row, size_t column, double *out_correlation);

#ifdef __cplusplus
}
#endif

#endif
