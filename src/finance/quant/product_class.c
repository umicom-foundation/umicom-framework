/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/product_class.c
 *
 * PURPOSE:
 *   Classify reusable quantitative product families.
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

#include "umicom/finance/quant/product_class.h"

#include <math.h>
#include <string.h>

/* Keep product-family labels stable for diagnostics and persisted configuration. */
const char *umi_quant_product_class_name(UmiQuantProductClass value)
{
    switch (value) {
        case UMI_QUANT_PRODUCT_FX: return "fx";
        case UMI_QUANT_PRODUCT_MONEY_MARKET: return "money-market";
        case UMI_QUANT_PRODUCT_BOND: return "bond";
        case UMI_QUANT_PRODUCT_SWAP: return "swap";
        case UMI_QUANT_PRODUCT_FUTURE: return "future";
        case UMI_QUANT_PRODUCT_FORWARD: return "forward";
        case UMI_QUANT_PRODUCT_OPTION: return "option";
        case UMI_QUANT_PRODUCT_CREDIT: return "credit";
        case UMI_QUANT_PRODUCT_INFLATION: return "inflation";
        default: return "unknown";
    }
}

/* Distinguish contingent/forward-style instruments from cash instruments. */
int umi_quant_product_class_is_derivative(UmiQuantProductClass value)
{
    return value == UMI_QUANT_PRODUCT_SWAP || value == UMI_QUANT_PRODUCT_FUTURE || value == UMI_QUANT_PRODUCT_FORWARD || value == UMI_QUANT_PRODUCT_OPTION || value == UMI_QUANT_PRODUCT_CREDIT;
}
