/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/command_enablement.h
 *
 * PURPOSE:
 *   Represent command enablement evidence from a state expression.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMMAND_ENABLEMENT_H
#define UMICOM_UI_REACTIVE_COMMAND_ENABLEMENT_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveCommandEnablement {
    char command_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool enabled;
    uint64_t evaluation_revision;
} UmiUiReactiveCommandEnablement;
void umi_ui_reactive_command_enablement_init(UmiUiReactiveCommandEnablement *item);
int umi_ui_reactive_command_enablement_valid(const UmiUiReactiveCommandEnablement *item);
#ifdef __cplusplus
}
#endif
#endif
