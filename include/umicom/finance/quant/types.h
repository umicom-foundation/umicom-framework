/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/types.h
 *
 * PURPOSE:
 *   Define common quantitative product, curve, model and scenario primitives.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_TYPES_H
#define INCLUDE_UMICOM_FINANCE_QUANT_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_QUANT_NAME_CAPACITY 64U
#define UMI_QUANT_MAX_CURVE_NODES 64U
#define UMI_QUANT_MAX_SURFACE_POINTS 128U
#define UMI_QUANT_MAX_SCENARIOS 64U
#define UMI_QUANT_MAX_MODELS 32U

typedef enum UmiQuantProductClass {
    UMI_QUANT_PRODUCT_UNKNOWN = 0,
    UMI_QUANT_PRODUCT_FX = 1,
    UMI_QUANT_PRODUCT_MONEY_MARKET = 2,
    UMI_QUANT_PRODUCT_BOND = 3,
    UMI_QUANT_PRODUCT_SWAP = 4,
    UMI_QUANT_PRODUCT_FUTURE = 5,
    UMI_QUANT_PRODUCT_FORWARD = 6,
    UMI_QUANT_PRODUCT_OPTION = 7,
    UMI_QUANT_PRODUCT_CREDIT = 8,
    UMI_QUANT_PRODUCT_INFLATION = 9
} UmiQuantProductClass;

typedef enum UmiQuantOptionRight {
    UMI_QUANT_CALL = 1,
    UMI_QUANT_PUT = -1
} UmiQuantOptionRight;

typedef enum UmiQuantModelKind {
    UMI_QUANT_MODEL_DISCOUNTING = 0,
    UMI_QUANT_MODEL_BLACK76 = 1,
    UMI_QUANT_MODEL_BLACK_SCHOLES = 2,
    UMI_QUANT_MODEL_NORMAL = 3,
    UMI_QUANT_MODEL_BINOMIAL = 4,
    UMI_QUANT_MODEL_MONTE_CARLO = 5
} UmiQuantModelKind;

typedef struct UmiQuantCurvePoint {
    int32_t tenor_days;
    double value;
} UmiQuantCurvePoint;

typedef struct UmiQuantNamedValue {
    char name[UMI_QUANT_NAME_CAPACITY];
    double value;
} UmiQuantNamedValue;

/* Copy bounded identifier text without allowing truncation. */
UmiStatus umi_quant_copy_text(char *destination, size_t capacity, const char *source);

/* Return one when a floating-point input is finite and suitable for pricing. */
int umi_quant_number_valid(double value);

/* Return the standard normal cumulative distribution used by option models. */
double umi_quant_normal_cdf(double value);

#ifdef __cplusplus
}
#endif

#endif
