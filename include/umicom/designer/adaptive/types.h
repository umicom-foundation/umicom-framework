/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/types.h
 *
 * PURPOSE:
 *   Define shared toolkit-neutral values for responsive visual-authoring, inheritance,
 *   preview matrices and adaptive application-shell design.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_TYPES_H
#define UMICOM_DESIGNER_ADAPTIVE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/designer/rad/types.h"
#include "umicom/ui/adaptive/types.h"
#include "umicom/ui/design/breakpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_ADAPTIVE_ID_CAPACITY 128U
#define UMI_DESIGNER_ADAPTIVE_VALUE_CAPACITY 512U
#define UMI_DESIGNER_ADAPTIVE_MAX_ITEMS 64U
#define UMI_DESIGNER_ADAPTIVE_MAX_VARIANTS 32U
#define UMI_DESIGNER_ADAPTIVE_MAX_PREVIEWS 32U

typedef enum UmiDesignerAdaptiveInheritance {
    UMI_DESIGNER_ADAPTIVE_INHERIT_PARENT = 1,
    UMI_DESIGNER_ADAPTIVE_OVERRIDE = 2,
    UMI_DESIGNER_ADAPTIVE_RESET = 3
} UmiDesignerAdaptiveInheritance;

typedef enum UmiDesignerAdaptivePreviewStatus {
    UMI_DESIGNER_PREVIEW_PENDING = 1,
    UMI_DESIGNER_PREVIEW_PASS = 2,
    UMI_DESIGNER_PREVIEW_FAIL = 3
} UmiDesignerAdaptivePreviewStatus;

typedef struct UmiDesignerAdaptiveValue {
    char text[UMI_DESIGNER_ADAPTIVE_VALUE_CAPACITY];
    int64_t integer_value;
    double real_value;
    int boolean_value;
} UmiDesignerAdaptiveValue;

/* Copy responsive-designer metadata into a bounded public ABI field. */
UmiStatus umi_designer_adaptive_copy_text(char *destination, size_t capacity, const char *source);
/* Return whether a canonical size class is within an inclusive range. */
int umi_designer_adaptive_size_in_range(UmiDesignSizeClass value,
                                        UmiDesignSizeClass minimum,
                                        UmiDesignSizeClass maximum);

#ifdef __cplusplus
}
#endif
#endif
