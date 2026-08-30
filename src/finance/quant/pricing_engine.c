/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/pricing_engine.c
 *
 * PURPOSE:
 *   Route reusable product families to deterministic pricing kernels.
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

#include "umicom/finance/quant/pricing_engine.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_pricing_engine_value(UmiQuantProductClass product_class,double quantity,double market_value,double strike_or_reference,double discount_factor,double *out_pv){double raw;if(out_pv==NULL||quantity<0.0||discount_factor<0.0)return UMI_STATUS_INVALID_ARGUMENT;switch(product_class){case UMI_QUANT_PRODUCT_BOND:raw=quantity*market_value;break;case UMI_QUANT_PRODUCT_FORWARD:case UMI_QUANT_PRODUCT_FX:case UMI_QUANT_PRODUCT_SWAP:raw=quantity*(market_value-strike_or_reference);break;case UMI_QUANT_PRODUCT_OPTION:raw=quantity*fmax(market_value-strike_or_reference,0.0);break;default:raw=quantity*market_value;break;}*out_pv=raw*discount_factor;return UMI_STATUS_OK;}
