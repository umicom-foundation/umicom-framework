/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/dialog_spec.h
 *
 * PURPOSE:
 *   Define dialog modality, sizing and governed action semantics.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_DIALOG_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_DIALOG_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"
#include "umicom/ui/design/size_class.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignDialogSpec {
    UmiDesignSizeClass width_class;
    uint16_t action_count;
    int modal;
    int destructive_action;
} UmiDesignDialogSpec;

/* Initialise the semantic dialog spec specification. */
UmiStatus umi_design_dialog_spec_init(UmiDesignDialogSpec *spec, UmiDesignSizeClass width_class, uint16_t action_count, int modal, int destructive_action);
/* Return one when the semantic specification is internally consistent. */
int umi_design_dialog_spec_valid(const UmiDesignDialogSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
