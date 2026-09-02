/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/adaptive_variant.h
 *
 * PURPOSE:
 *   Define one responsive design variant scoped by breakpoint and orientation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_VARIANT_H
#define UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_VARIANT_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer adaptive variant data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerAdaptiveVariant {
    char variant_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    char parent_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    char breakpoint_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    UmiDesignSizeClass size_class;
    UmiAdaptiveDisplayOrientation orientation;
    uint32_t priority;
} UmiDesignerAdaptiveVariant;
/* Initialise one adaptive variant with optional parent inheritance. */
UmiStatus umi_designer_adaptive_variant_init(UmiDesignerAdaptiveVariant *variant,
                                             const char *variant_id,
                                             const char *parent_id,
                                             const char *breakpoint_id,
                                             UmiDesignSizeClass size_class,
                                             UmiAdaptiveDisplayOrientation orientation,
                                             uint32_t priority);

#ifdef __cplusplus
}
#endif
#endif
