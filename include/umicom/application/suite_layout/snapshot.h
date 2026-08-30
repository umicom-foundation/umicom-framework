/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/snapshot.h
 *
 * PURPOSE:
 *   Expose immutable active layout state for Studio, Trader and other thin suite applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_SNAPSHOT_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_SNAPSHOT_H

#include "umicom/application/suite_layout/descriptor.h"
#include "umicom/ui/workspace_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationSuiteLayoutSnapshot {
    char application_id[UMI_UI_ID_CAPACITY];
    char active_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char active_layout_name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    size_t window_count;
    uint64_t revision;
    int loaded;
} UmiApplicationSuiteLayoutSnapshot;

#ifdef __cplusplus
}
#endif
#endif
