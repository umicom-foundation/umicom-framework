/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/operational_rwa.c
 *
 * PURPOSE:
 *   Calculate operational risk-weighted assets from business indicator and loss multiplier.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/operational_rwa.h"

UmiStatus umi_pru_operational_rwa_calculate(UmiPrudentialOperationalRwa *result,double bic,double lm,double factor) { double charge; if(result==NULL||!umi_pru_number_valid(bic)||!umi_pru_number_valid(lm)||!umi_pru_number_valid(factor)||bic<0.0||lm<0.0||factor<0.0)return UMI_STATUS_INVALID_ARGUMENT; charge=bic*lm; result->business_indicator_component=bic; result->loss_multiplier=lm; result->conversion_factor=factor; result->rwa=charge*factor; return umi_pru_number_valid(result->rwa)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
