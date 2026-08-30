/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/aggregation_descriptor.h
 *
 * PURPOSE:
 *   Describe one summary aggregation over a column.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_AGGREGATION_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_AGGREGATION_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntAggregationDescriptor {
    char aggregation_id[UMI_UI_ENT_ID_CAPACITY];
    char column_id[UMI_UI_ENT_ID_CAPACITY];
    UmiUiEntAggregateKind kind;
} UmiUiEntAggregationDescriptor;
UmiStatus umi_ui_ent_aggregation_descriptor_init(UmiUiEntAggregationDescriptor *value);
int umi_ui_ent_aggregation_descriptor_validate(const UmiUiEntAggregationDescriptor *value);
#ifdef __cplusplus
}
#endif

#endif
