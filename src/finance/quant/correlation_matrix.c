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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/correlation_matrix.h"

#include <math.h>
#include <string.h>

/*
 * Initialise quant correlation matrix from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_quant_correlation_matrix_init(UmiQuantCorrelationMatrix *matrix,size_t size){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL||size==0U||size>16U)return UMI_STATUS_INVALID_ARGUMENT;memset(matrix,0,sizeof *matrix);matrix->size=size;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<size;++i)matrix->values[i][i]=1.0;return UMI_STATUS_OK;}
/*
 * Copy quant correlation matrix into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_quant_correlation_matrix_set(UmiQuantCorrelationMatrix *matrix,size_t row,size_t column,double correlation){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL||row>=matrix->size||column>=matrix->size||correlation < -1.0||correlation>1.0)return UMI_STATUS_INVALID_ARGUMENT;matrix->values[row][column]=correlation;matrix->values[column][row]=correlation;return UMI_STATUS_OK;}
/*
 * Provide the quant correlation matrix get operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_correlation_matrix_get(const UmiQuantCorrelationMatrix *matrix,size_t row,size_t column,double *out_correlation){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL||out_correlation==NULL||row>=matrix->size||column>=matrix->size)return UMI_STATUS_INVALID_ARGUMENT;*out_correlation=matrix->values[row][column];return UMI_STATUS_OK;}
