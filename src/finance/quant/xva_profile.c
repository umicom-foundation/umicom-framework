/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/xva_profile.c
 *
 * PURPOSE:
 *   Aggregate CVA, DVA, FVA and MVA valuation adjustments.
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

#include "umicom/finance/quant/xva_profile.h"

#include <math.h>
#include <string.h>

/*
 * Initialise quant xva profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_quant_xva_profile_init(UmiQuantXvaProfile *profile,double cva,double dva,double fva,double mva){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile==NULL)return UMI_STATUS_INVALID_ARGUMENT;profile->cva=cva;profile->dva=dva;profile->fva=fva;profile->mva=mva;return UMI_STATUS_OK;}
/*
 * Provide the quant xva profile net operation used by this module and its client
 * applications.
 */
double umi_quant_xva_profile_net(const UmiQuantXvaProfile *profile){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile==NULL)return 0.0;return -profile->cva+profile->dva-profile->fva-profile->mva;}
