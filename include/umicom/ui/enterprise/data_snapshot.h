/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/data_snapshot.h
 *
 * PURPOSE:
 *   Describe an immutable logical data snapshot used by virtual views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_DATA_SNAPSHOT_H
#define UMICOM_UI_ENTERPRISE_DATA_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntDataSnapshot {
    uint64_t generation;
    size_t row_count;
    size_t column_count;
    int complete;
    uint64_t source_revision;
} UmiUiEntDataSnapshot;
UmiStatus umi_ui_ent_data_snapshot_init(UmiUiEntDataSnapshot *value);
int umi_ui_ent_data_snapshot_validate(const UmiUiEntDataSnapshot *value);
#ifdef __cplusplus
}
#endif

#endif
