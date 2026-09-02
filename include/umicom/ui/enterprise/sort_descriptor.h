/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/sort_descriptor.h
 *
 * PURPOSE:
 *   Describe one deterministic multi-column sort key.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SORT_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_SORT_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent sort descriptor data shared with callers of this public contract.
 */
typedef struct UmiUiEntSortDescriptor {
    char column_id[UMI_UI_ENT_ID_CAPACITY];
    UmiUiEntSortDirection direction;
    int32_t priority;
    int case_sensitive;
} UmiUiEntSortDescriptor;
/**
 * Initialise ui ent sort descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_sort_descriptor_init(UmiUiEntSortDescriptor *value);
/**
 * Check that ui ent sort descriptor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_sort_descriptor_validate(const UmiUiEntSortDescriptor *value);
#ifdef __cplusplus
}
#endif

#endif
