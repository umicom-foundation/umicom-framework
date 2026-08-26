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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/hqla_bucket.h"

UmiStatus umi_pru_hqla_bucket_add(UmiPrudentialHqlaBucket *bucket,UmiHqlaLevel level,double gross,double eligible) { if(bucket==NULL||gross<0.0||eligible<0.0||eligible>gross)return UMI_STATUS_INVALID_ARGUMENT; if(bucket->gross_value==0.0&&bucket->eligible_value==0.0)bucket->level=level; else if(bucket->level!=level)return UMI_STATUS_INVALID_STATE; bucket->gross_value+=gross; bucket->eligible_value+=eligible; return UMI_STATUS_OK; }
