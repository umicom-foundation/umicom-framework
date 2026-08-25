/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/xva_profile.h
 *
 * PURPOSE:
 *   Aggregate CVA, DVA, FVA and MVA valuation adjustments.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_XVA_PROFILE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_XVA_PROFILE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantXvaProfile { double cva; double dva; double fva; double mva; } UmiQuantXvaProfile;
/* Initialise a signed XVA profile. */
UmiStatus umi_quant_xva_profile_init(UmiQuantXvaProfile *profile, double cva, double dva, double fva, double mva);
/* Return the net adjustment to clean value. */
double umi_quant_xva_profile_net(const UmiQuantXvaProfile *profile);

#ifdef __cplusplus
}
#endif

#endif
