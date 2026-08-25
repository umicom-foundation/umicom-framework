/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/checkpoint_store.h
 *
 * PURPOSE:
 *   Maintain a bounded in-memory checkpoint catalogue and deterministic latest-checkpoint lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_CHECKPOINT_STORE_H
#define UMICOM_DEVELOPER_HELIX_CHECKPOINT_STORE_H
#include "umicom/developer/helix/checkpoint.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixCheckpointStore { UmiHelixCheckpoint items[UMI_HELIX_MAX_ITEMS]; size_t count; } UmiHelixCheckpointStore;
/* Add a validated checkpoint and reject duplicate identifiers. */
UmiStatus umi_helix_checkpoint_store_add(UmiHelixCheckpointStore *store,const UmiHelixCheckpoint *checkpoint);
/* Resolve a checkpoint by stable identifier. */
const UmiHelixCheckpoint *umi_helix_checkpoint_store_find(const UmiHelixCheckpointStore *store,const char *id);
/* Return the highest-sequence checkpoint. */
const UmiHelixCheckpoint *umi_helix_checkpoint_store_latest(const UmiHelixCheckpointStore *store);
#ifdef __cplusplus
}
#endif
#endif
