/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/game/session.h
 *
 * PURPOSE:
 *   Track deterministic game ticks and participant acknowledgements without
 *   coupling the game engine to a particular networking library.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_GAME_SESSION_H
#define UMICOM_GAME_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/game/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Session mode describes authority but does not choose a network transport. */
typedef enum UmiGameSessionMode {
    UMI_GAME_SESSION_STANDALONE = 0,
    UMI_GAME_SESSION_HOST = 1,
    UMI_GAME_SESSION_CLIENT = 2
} UmiGameSessionMode;

/* The session owns participant acknowledgement records. */
typedef struct UmiGameSession UmiGameSession;

/* Session snapshots are safe to retain because they contain no owned pointer. */
typedef struct UmiGameSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiGameSessionMode mode;
    size_t participant_count;
    size_t participant_capacity;
    uint64_t current_tick;
    uint64_t confirmed_tick;
    uint64_t revision;
} UmiGameSessionSnapshot;

/* Create a deterministic session with bounded participant storage. */
UmiStatus umi_game_session_create(
    UmiGameSessionMode mode,
    size_t participant_capacity,
    UmiGameSession **out_session);

/* Destroy all state owned by a game session. */
void umi_game_session_destroy(UmiGameSession *session);

/* Add one unique participant at the current simulation tick. */
UmiStatus umi_game_session_join(
    UmiGameSession *session,
    const char *participant_id);

/* Remove one participant and recalculate the confirmed tick. */
UmiStatus umi_game_session_leave(
    UmiGameSession *session,
    const char *participant_id);

/* Advance the authoritative deterministic tick by one. */
UmiStatus umi_game_session_advance(
    UmiGameSession *session,
    uint64_t *out_tick);

/* Record that a participant has processed a monotonic tick. */
UmiStatus umi_game_session_acknowledge(
    UmiGameSession *session,
    const char *participant_id,
    uint64_t tick);

/* Copy session progress for network adapters, diagnostics and UI. */
UmiStatus umi_game_session_snapshot(
    const UmiGameSession *session,
    UmiGameSessionSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
