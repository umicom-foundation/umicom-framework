/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/adaptive_validation.h
 *
 * PURPOSE:
 *   Aggregate blocking adaptive-designer validation issues across variants, rules and preview cases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_VALIDATION_H
#define UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_VALIDATION_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAdaptiveValidation {
    uint32_t invalid_variants;
    uint32_t invalid_rules;
    uint32_t failed_previews;
    uint32_t warnings;
} UmiDesignerAdaptiveValidation;
/* Return whether adaptive design state is publishable without blocking errors. */
int umi_designer_adaptive_validation_publishable(const UmiDesignerAdaptiveValidation *validation);
/* Return the total blocking issue count. */
uint32_t umi_designer_adaptive_validation_blocking_count(const UmiDesignerAdaptiveValidation *validation);

#ifdef __cplusplus
}
#endif
#endif
