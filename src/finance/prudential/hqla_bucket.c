/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/hqla_bucket.c
 *
 * PURPOSE:
 *   Aggregate HQLA values by regulatory liquidity level.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/hqla_bucket.h"

/* Add pru hqla bucket only after its inputs and available capacity have been checked. */
UmiStatus umi_pru_hqla_bucket_add(UmiPrudentialHqlaBucket *bucket,UmiHqlaLevel level,double gross,double eligible) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(bucket==NULL||gross<0.0||eligible<0.0||eligible>gross)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(bucket->gross_value==0.0&&bucket->eligible_value==0.0)bucket->level=level; else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(bucket->level!=level)return UMI_STATUS_INVALID_STATE; bucket->gross_value+=gross; bucket->eligible_value+=eligible; return UMI_STATUS_OK; }
