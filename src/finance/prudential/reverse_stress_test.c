/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/reverse_stress_test.c
 *
 * PURPOSE:
 *   Estimate the linear shock required to exhaust available headroom.
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

#include "umicom/finance/prudential/reverse_stress_test.h"

/*
 * Provide the pru reverse stress test calculate operation used by this module and its
 * client applications.
 */
UmiStatus umi_pru_reverse_stress_test_calculate(UmiPrudentialReverseStressTest *result,double headroom,double exposure,double loss_per_unit_shock) { double denominator; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||headroom<0.0||exposure<0.0||loss_per_unit_shock<=0.0)return UMI_STATUS_INVALID_ARGUMENT; denominator=exposure*loss_per_unit_shock; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(denominator<=0.0)return UMI_STATUS_INVALID_ARGUMENT; result->headroom=headroom; result->exposure=exposure; result->loss_per_unit_shock=loss_per_unit_shock; result->required_shock=headroom/denominator; return umi_pru_number_valid(result->required_shock)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
