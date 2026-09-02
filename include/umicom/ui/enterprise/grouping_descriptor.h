/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/grouping_descriptor.h
 *
 * PURPOSE:
 *   Describe a grouping key and default expansion semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_GROUPING_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_GROUPING_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent grouping descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntGroupingDescriptor {
    char column_id[UMI_UI_ENT_ID_CAPACITY];
    int32_t level;
    int expanded_by_default;
} UmiUiEntGroupingDescriptor;
/**
 * Initialise ui ent grouping descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_grouping_descriptor_init(UmiUiEntGroupingDescriptor *value);
/**
 * Check that ui ent grouping descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_grouping_descriptor_validate(const UmiUiEntGroupingDescriptor *value);
#ifdef __cplusplus
}
#endif

#endif
