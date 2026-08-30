/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_exposure_bucket.c
 *
 * PURPOSE:
 *   Verify aggregate exposure amounts for one prudential class.
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

#include "umicom/finance/prudential/exposure_bucket.h"

int main(void) { UmiPrudentialExposureBucket b; UmiCurrency c={"GBP"}; if(umi_pru_exposure_bucket_init(&b,"B",10.0,c)!=UMI_STATUS_OK)return 1; if(umi_pru_exposure_bucket_add(&b,5.0)!=UMI_STATUS_OK)return 2; return b.amount==15.0?0:3; }
