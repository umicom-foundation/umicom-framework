/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/variant_resolver.h
 *
 * PURPOSE:
 *   Choose the highest-priority adaptive variant matching size class and orientation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_VARIANT_RESOLVER_H
#define UMICOM_DESIGNER_ADAPTIVE_VARIANT_RESOLVER_H
#include "umicom/designer/adaptive/variant_set.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Resolve the best matching adaptive variant for an active preview/runtime context. */
UmiStatus umi_designer_variant_resolver_resolve(const UmiDesignerAdaptiveVariantSet *set,
                                                UmiDesignSizeClass size_class,
                                                UmiAdaptiveDisplayOrientation orientation,
                                                UmiDesignerAdaptiveVariant *out_variant);

#ifdef __cplusplus
}
#endif
#endif
