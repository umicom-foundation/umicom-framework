/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/correlation_matrix.c
 *
 * PURPOSE:
 *   Store and validate bounded symmetric correlation matrices.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/correlation_matrix.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_correlation_matrix_init(UmiQuantCorrelationMatrix *matrix,size_t size){size_t i;if(matrix==NULL||size==0U||size>16U)return UMI_STATUS_INVALID_ARGUMENT;memset(matrix,0,sizeof *matrix);matrix->size=size;for(i=0U;i<size;++i)matrix->values[i][i]=1.0;return UMI_STATUS_OK;}
UmiStatus umi_quant_correlation_matrix_set(UmiQuantCorrelationMatrix *matrix,size_t row,size_t column,double correlation){if(matrix==NULL||row>=matrix->size||column>=matrix->size||correlation < -1.0||correlation>1.0)return UMI_STATUS_INVALID_ARGUMENT;matrix->values[row][column]=correlation;matrix->values[column][row]=correlation;return UMI_STATUS_OK;}
UmiStatus umi_quant_correlation_matrix_get(const UmiQuantCorrelationMatrix *matrix,size_t row,size_t column,double *out_correlation){if(matrix==NULL||out_correlation==NULL||row>=matrix->size||column>=matrix->size)return UMI_STATUS_INVALID_ARGUMENT;*out_correlation=matrix->values[row][column];return UMI_STATUS_OK;}
