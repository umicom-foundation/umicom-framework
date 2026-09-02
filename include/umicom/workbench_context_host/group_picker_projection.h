/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/group_picker_projection.h
 *
 * PURPOSE:
 *   Project link groups into a toolkit-neutral picker model for GTK4 and future adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_GROUP_PICKER_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_GROUP_PICKER_PROJECTION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workbench context host group picker row data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostGroupPickerRow {
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char title[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    UmiContextChannelColour colour;
    size_t member_count;
    bool active;
} UmiWorkbenchContextHostGroupPickerRow;
/**
 * Represent the workbench context host group picker projection data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextHostGroupPickerProjection {
    UmiWorkbenchContextHostGroupPickerRow rows[UMI_WORKBENCH_CONTEXT_HOST_MAX_GROUPS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchContextHostGroupPickerProjection;
/**
 * Provide the workbench context host group picker projection build operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_group_picker_projection_build(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostGroupPickerProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
