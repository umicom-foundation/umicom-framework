/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_descriptor.h
 *
 * PURPOSE:
 *   Describe an enterprise grid column with sizing, edit and interaction capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_COLUMN_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent column descriptor data shared with callers of this public contract.
 */
typedef struct UmiUiEntColumnDescriptor {
    char column_id[UMI_UI_ENT_ID_CAPACITY];
    char label[UMI_UI_ENT_TEXT_CAPACITY];
    int32_t width;
    int32_t minimum_width;
    int32_t maximum_width;
    int sortable;
    int filterable;
    int editable;
    int resizable;
    int visible;
    int frozen;
} UmiUiEntColumnDescriptor;
/**
 * Initialise ui ent column descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_column_descriptor_init(UmiUiEntColumnDescriptor *value);
/**
 * Check that ui ent column descriptor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_column_descriptor_validate(const UmiUiEntColumnDescriptor *value);
#ifdef __cplusplus
}
#endif

#endif
