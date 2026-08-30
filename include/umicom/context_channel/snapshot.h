/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/snapshot.h
 *
 * PURPOSE:
 *   Expose immutable runtime snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SNAPSHOT_H
#define UMICOM_CONTEXT_CHANNEL_SNAPSHOT_H
#include "umicom/context_channel/registry.h"
#include "umicom/context_channel/router.h"
#include "umicom/context_channel/metrics.h"
#include "umicom/context_channel/panel.h"
#include "umicom/context_channel/panel_instance.h"
#include "umicom/context_channel/group_member.h"
#include "umicom/context_channel/link_group.h"
#include "umicom/context_channel/history_entry.h"
#include "umicom/context_channel/diagnostic.h"
#include "umicom/context_channel/bookmark.h"
#include "umicom/context_channel/session.h"
#include "umicom/context_channel/capability.h"
#include "umicom/context_channel/filter_rule.h"
#include "umicom/context_channel/transformer_rule.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_SNAPSHOT_NAME "snapshot"
typedef struct UmiContextSnapshotState {
    uint64_t revision;
    uint64_t operation_count;
    UmiStatus last_status;
    char last_message[UMI_CONTEXT_VALUE_CAPACITY];
} UmiContextSnapshotState;
void umi_context_snapshot_state_init(UmiContextSnapshotState *state);
UmiStatus umi_context_snapshot_state_record(UmiContextSnapshotState *state,UmiStatus status,const char *message);
uint64_t umi_context_snapshot_operation_count(const UmiContextSnapshotState *state);
#ifdef __cplusplus
}
#endif
#endif
