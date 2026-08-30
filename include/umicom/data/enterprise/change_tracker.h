/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/change_tracker.h
 *
 * PURPOSE:
 *   Track ORM entity mutations and prevent invalid persistence-state transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_CHANGE_TRACKER_H
#define UMICOM_DATA_ENTERPRISE_CHANGE_TRACKER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataTrackedChange { char change_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; char entity_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; char key[UMI_DATA_ENTERPRISE_ID_CAPACITY]; UmiDataChangeKind kind; uint64_t revision; } UmiDataTrackedChange;
typedef struct UmiDataChangeTracker { UmiDataTrackedChange items[UMI_DATA_ENTERPRISE_MAX_ITEMS]; size_t count; uint64_t revision; } UmiDataChangeTracker;
/* Reset change evidence for a new unit of work. */ void umi_data_change_tracker_init(UmiDataChangeTracker *tracker);
/* Track a logical entity change, merging compatible state transitions. */ UmiStatus umi_data_change_tracker_track(UmiDataChangeTracker *tracker,const char *change_id,const char *entity_id,const char *key,UmiDataChangeKind kind);
/* Count pending mutations of a requested kind. */ size_t umi_data_change_tracker_count_kind(const UmiDataChangeTracker *tracker,UmiDataChangeKind kind);

#ifdef __cplusplus
}
#endif
#endif
