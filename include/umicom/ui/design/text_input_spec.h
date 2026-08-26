/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/text_input_spec.h
 *
 * PURPOSE:
 *   Define text-entry mode, placeholder and validation semantics for reusable inputs.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_TEXT_INPUT_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_TEXT_INPUT_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignTextInputSpec {
    char placeholder[UMI_DESIGN_TEXT_CAPACITY];
    uint32_t max_length;
    int password;
    int search;
    int multiline;
} UmiDesignTextInputSpec;

/* Initialise the semantic text input spec specification. */
UmiStatus umi_design_text_input_spec_init(UmiDesignTextInputSpec *spec, const char *placeholder, uint32_t max_length, int password, int search, int multiline);
/* Return one when the semantic specification is internally consistent. */
int umi_design_text_input_spec_valid(const UmiDesignTextInputSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
