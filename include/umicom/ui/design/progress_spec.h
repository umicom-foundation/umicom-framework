/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/progress_spec.h
 *
 * PURPOSE:
 *   Define determinate and indeterminate progress semantics for tasks and background operations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_PROGRESS_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_PROGRESS_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design progress spec data shared with callers of this public contract.
 */
typedef struct UmiDesignProgressSpec {
    double minimum;
    double maximum;
    double value;
    int indeterminate;
    int show_text;
} UmiDesignProgressSpec;

/* Initialise the semantic progress spec specification. */
UmiStatus umi_design_progress_spec_init(UmiDesignProgressSpec *spec, double minimum, double maximum, double value, int indeterminate, int show_text);
/* Return one when the semantic specification is internally consistent. */
int umi_design_progress_spec_valid(const UmiDesignProgressSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
