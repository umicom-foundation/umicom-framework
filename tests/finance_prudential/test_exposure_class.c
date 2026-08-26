/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_exposure_class.c
 *
 * PURPOSE:
 *   Verify describe one prudential exposure class and default risk weight.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/exposure_class.h"

int main(void) { UmiPrudentialExposureClass r; if(umi_pru_exposure_class_init(&r,"CORP",1.0)!=UMI_STATUS_OK)return 1; if(r.risk_weight!=1.0)return 2; if(umi_pru_exposure_class_init(&r,"BAD",13.0)!=UMI_STATUS_INVALID_ARGUMENT)return 3; return 0; }
