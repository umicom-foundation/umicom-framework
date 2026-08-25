/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/product_class.h
 *
 * PURPOSE:
 *   Classify reusable quantitative product families.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_PRODUCT_CLASS_H
#define INCLUDE_UMICOM_FINANCE_QUANT_PRODUCT_CLASS_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return a stable text label for a product family. */
const char *umi_quant_product_class_name(UmiQuantProductClass value);

/* Return one when a product has derivative-style future contingent value. */
int umi_quant_product_class_is_derivative(UmiQuantProductClass value);

#ifdef __cplusplus
}
#endif

#endif
