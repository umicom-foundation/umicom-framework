/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/sort_descriptor.h
 *
 * PURPOSE:
 *   Describe one deterministic multi-column sort key.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SORT_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_SORT_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntSortDescriptor {
    char column_id[UMI_UI_ENT_ID_CAPACITY];
    UmiUiEntSortDirection direction;
    int32_t priority;
    int case_sensitive;
} UmiUiEntSortDescriptor;
UmiStatus umi_ui_ent_sort_descriptor_init(UmiUiEntSortDescriptor *value);
int umi_ui_ent_sort_descriptor_validate(const UmiUiEntSortDescriptor *value);
#ifdef __cplusplus
}
#endif

#endif
