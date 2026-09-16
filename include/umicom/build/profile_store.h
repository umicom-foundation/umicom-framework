/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/profile_store.h
 *
 * PURPOSE:
 *   Save and restore reviewed project settings through the existing Data Server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_PROFILE_STORE_H
#define UMICOM_BUILD_PROFILE_STORE_H

#include "umicom/build/profile.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Load one platform-specific profile identified by an absolute, normalised
 * workspace path. On OK both outputs are assigned. On NOT_FOUND only
 * outRevision is set to zero. Other failures preserve both outputs.
 * No project file is executed, no program is guessed, and no trust decision is
 * restored. Path identity is lexical; symlink aliases are not resolved.
 *
 * The caller serialises use of this connection on its owning thread. These
 * calls own a complete Data Server transaction and reject an already active
 * transaction. Independent SQLite connections use its existing BEGIN IMMEDIATE
 * transaction path. Memory backends retain data only until server destruction.
 */
UmiStatus UmiBuildProfileStoreLoad(UmiDataServer *server,
    const char *workspaceRoot, UmiBuildProfile *outProfile, uint64_t *outRevision);

/** Save a validated profile only when expectedRevision matches the stored
 * revision. Zero creates a previously absent record. INVALID_STATE reports a
 * stale writer or incompatible stored schema; malformed records are preserved.
 * All fields and the next revision are committed together. A failed save leaves
 * outRevision unchanged. A successful save does not grant workspace trust or
 * start a compiler. Values are encoded by field, never as a raw C structure.
 */
UmiStatus UmiBuildProfileStoreSave(UmiDataServer *server,
    const UmiBuildProfile *profile, uint64_t expectedRevision,
    uint64_t *outRevision);

#ifdef __cplusplus
}
#endif
#endif
