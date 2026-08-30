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
typedef struct UmiUiEntGroupingDescriptor {
    char column_id[UMI_UI_ENT_ID_CAPACITY];
    int32_t level;
    int expanded_by_default;
} UmiUiEntGroupingDescriptor;
UmiStatus umi_ui_ent_grouping_descriptor_init(UmiUiEntGroupingDescriptor *value);
int umi_ui_ent_grouping_descriptor_validate(const UmiUiEntGroupingDescriptor *value);
#ifdef __cplusplus
}
#endif

#endif
