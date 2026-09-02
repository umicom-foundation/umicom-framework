/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/session_store.h
 *
 * PURPOSE:
 *   Persist active workbench sessions, open panel state and crash-recovery evidence through the authoritative Data Server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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


/**
 * Represent the workbench layout session store data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutSessionStore {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutChunkStore chunks;
} UmiWorkbenchLayoutSessionStore;

/**
 * Initialise workbench layout session store from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_session_store_init(
    UmiWorkbenchLayoutSessionStore *store,
    UmiDataServer *server);

/**
 * Write workbench layout session in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_workbench_layout_session_encode(
    const UmiWorkbenchLayoutSession *session,
    char **out_text,
    size_t *out_size);

/**
 * Read workbench layout session into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_workbench_layout_session_decode(
    const char *text,
    UmiWorkbenchLayoutSession *out_session);

/**
 * Write workbench layout session store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_session_store_save(
    const UmiWorkbenchLayoutSessionStore *store,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision);

/**
 * Read workbench layout session store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_session_store_load(
    const UmiWorkbenchLayoutSessionStore *store,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session);

/**
 * Provide the workbench layout session store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_session_store_delete(
    const UmiWorkbenchLayoutSessionStore *store,
    const char *session_id,
    uint64_t expected_revision);

#ifdef __cplusplus
}
#endif

#endif
