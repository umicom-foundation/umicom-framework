/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/exposure_bucket.c
 *
 * PURPOSE:
 *   Aggregate exposure amounts for one prudential class.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/exposure_bucket.h"

#include <string.h>
UmiStatus umi_pru_exposure_bucket_init(UmiPrudentialExposureBucket *bucket,const char *bucket_id,double amount,UmiCurrency currency) { UmiStatus s; if(bucket==NULL||bucket_id==NULL||!umi_pru_number_valid(amount)||amount<0.0)return UMI_STATUS_INVALID_ARGUMENT; memset(bucket,0,sizeof *bucket); s=umi_pru_copy_text(bucket->bucket_id,sizeof bucket->bucket_id,bucket_id); if(s!=UMI_STATUS_OK)return s; bucket->amount=amount; bucket->currency=currency; return UMI_STATUS_OK; }
UmiStatus umi_pru_exposure_bucket_add(UmiPrudentialExposureBucket *bucket,double amount) { if(bucket==NULL||!umi_pru_number_valid(amount)||amount<0.0)return UMI_STATUS_INVALID_ARGUMENT; bucket->amount+=amount; return umi_pru_number_valid(bucket->amount)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
