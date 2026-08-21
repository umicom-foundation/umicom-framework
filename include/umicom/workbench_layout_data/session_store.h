/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/session_store.h
 *
 * PURPOSE:
 *   Persist active workbench sessions, open panel state and crash-recovery evidence through the authoritative Data Server.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_SESSION_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_SESSION_STORE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout/session.h"
#include "umicom/workbench_layout_data/chunk_store.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutSessionStore {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutChunkStore chunks;
} UmiWorkbenchLayoutSessionStore;

UmiStatus umi_workbench_layout_session_store_init(
    UmiWorkbenchLayoutSessionStore *store,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_session_encode(
    const UmiWorkbenchLayoutSession *session,
    char **out_text,
    size_t *out_size);

UmiStatus umi_workbench_layout_session_decode(
    const char *text,
    UmiWorkbenchLayoutSession *out_session);

UmiStatus umi_workbench_layout_session_store_save(
    const UmiWorkbenchLayoutSessionStore *store,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision);

UmiStatus umi_workbench_layout_session_store_load(
    const UmiWorkbenchLayoutSessionStore *store,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session);

UmiStatus umi_workbench_layout_session_store_delete(
    const UmiWorkbenchLayoutSessionStore *store,
    const char *session_id,
    uint64_t expected_revision);

#ifdef __cplusplus
}
#endif

#endif
