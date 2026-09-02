/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_descriptor.h
 *
 * PURPOSE:
 *   Describe a stable enterprise row identity, display label and revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_ROW_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent row descriptor data shared with callers of this public contract.
 */
typedef struct UmiUiEntRowDescriptor {
    uint64_t row_key;
    char label[UMI_UI_ENT_TEXT_CAPACITY];
    int selectable;
    int editable;
    int enabled;
    uint64_t revision;
} UmiUiEntRowDescriptor;
/**
 * Initialise ui ent row descriptor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_row_descriptor_init(UmiUiEntRowDescriptor *value);
/**
 * Check that ui ent row descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_row_descriptor_validate(const UmiUiEntRowDescriptor *value);
#ifdef __cplusplus
}
#endif

#endif
