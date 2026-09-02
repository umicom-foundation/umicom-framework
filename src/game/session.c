/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/game/session.c
 *
 * PURPOSE:
 *   Implement deterministic participant and tick acknowledgement state while
 *   leaving network transport to an independently replaceable adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/game/session.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Participant storage copies IDs so caller-owned text may be released safely. */
typedef struct UmiGameParticipant {
    char participant_id[UMI_GAME_PARTICIPANT_ID_CAPACITY];
    uint64_t acknowledged_tick;
    bool active;
} UmiGameParticipant;

/* Session internals remain opaque to keep networking adapters replaceable. */
struct UmiGameSession {
    UmiGameSessionMode mode;
    UmiGameParticipant *participants;
    size_t capacity;
    size_t count;
    uint64_t current_tick;
    uint64_t confirmed_tick;
    uint64_t revision;
};

/* Find an active participant without exposing the internal record publicly. */
static UmiGameParticipant *find_participant(
    UmiGameSession *session,
    const char *participant_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || participant_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->capacity; ++index) {
        /* Inactive slots may contain cleared text and are never a valid match. */
        if (session->participants[index].active &&
            strcmp(session->participants[index].participant_id,
                   participant_id) == 0) {
            return &session->participants[index];
        }
    }
    return NULL;
}

/* Recalculate the tick processed by every currently active participant. */
static void refresh_confirmed_tick(UmiGameSession *session)
{
    size_t index;
    uint64_t confirmed = session->current_tick;
    /* With nobody connected, no acknowledgement can hold back local progress. */
    if (session->count == 0U) {
        session->confirmed_tick = session->current_tick;
        return;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->capacity; ++index) {
        /* The minimum acknowledgement is the last globally confirmed state. */
        if (session->participants[index].active &&
            session->participants[index].acknowledged_tick < confirmed) {
            confirmed = session->participants[index].acknowledged_tick;
        }
    }
    session->confirmed_tick = confirmed;
}

/* Allocate session and bounded participant storage with checked size math. */
UmiStatus umi_game_session_create(
    UmiGameSessionMode mode,
    size_t participant_capacity,
    UmiGameSession **out_session)
{
    UmiGameSession *session;
    /* Invalid modes, zero capacity and multiplication overflow are rejected. */
    if (out_session == NULL || mode < UMI_GAME_SESSION_STANDALONE ||
        mode > UMI_GAME_SESSION_CLIENT || participant_capacity == 0U ||
        participant_capacity > SIZE_MAX / sizeof(UmiGameParticipant)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiGameSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->participants = (UmiGameParticipant *)calloc(
        participant_capacity, sizeof(*session->participants));
    /* Free the owner if participant allocation cannot be completed. */
    if (session->participants == NULL) {
        free(session);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    session->mode = mode;
    session->capacity = participant_capacity;
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/* Release all owned participant data; destroying NULL is intentionally safe. */
void umi_game_session_destroy(UmiGameSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    free(session->participants);
    free(session);
}

/* Add a unique participant and initialise acknowledgement at the current tick. */
UmiStatus umi_game_session_join(
    UmiGameSession *session,
    const char *participant_id)
{
    size_t index;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || participant_id == NULL ||
        participant_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Duplicate identities would make acknowledgement ownership ambiguous. */
    if (find_participant(session, participant_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count >= session->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->capacity; ++index) {
        UmiGameParticipant *participant = &session->participants[index];
        /* Cleared inactive slots can be reused without moving active records. */
        if (!participant->active) {
            (void)memset(participant, 0, sizeof(*participant));
            written = snprintf(participant->participant_id,
                               sizeof(participant->participant_id), "%s",
                               participant_id);
            /* Reject truncation so external identity remains exact. */
            if (written < 0 ||
                (size_t)written >= sizeof(participant->participant_id)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            participant->acknowledged_tick = session->current_tick;
            participant->active = true;
            session->count += 1U;
            session->revision += 1U;
            refresh_confirmed_tick(session);
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_INTERNAL_ERROR;
}

/* Remove a participant and release its influence on global confirmation. */
UmiStatus umi_game_session_leave(
    UmiGameSession *session,
    const char *participant_id)
{
    UmiGameParticipant *participant;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || participant_id == NULL || participant_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    participant = find_participant(session, participant_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (participant == NULL) return UMI_STATUS_NOT_FOUND;
    (void)memset(participant, 0, sizeof(*participant));
    session->count -= 1U;
    session->revision += 1U;
    refresh_confirmed_tick(session);
    return UMI_STATUS_OK;
}

/* Advance deterministic simulation time without depending on wall-clock time. */
UmiStatus umi_game_session_advance(
    UmiGameSession *session,
    uint64_t *out_tick)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_tick == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Tick wrap would make old acknowledgements appear newer, so it is refused. */
    if (session->current_tick == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    session->current_tick += 1U;
    session->revision += 1U;
    refresh_confirmed_tick(session);
    *out_tick = session->current_tick;
    return UMI_STATUS_OK;
}

/* Record monotonic processing progress for one active participant. */
UmiStatus umi_game_session_acknowledge(
    UmiGameSession *session,
    const char *participant_id,
    uint64_t tick)
{
    UmiGameParticipant *participant;
    /* Future acknowledgements are invalid because that state does not exist. */
    if (session == NULL || participant_id == NULL || participant_id[0] == '\0' ||
        tick > session->current_tick) return UMI_STATUS_INVALID_ARGUMENT;
    participant = find_participant(session, participant_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (participant == NULL) return UMI_STATUS_NOT_FOUND;
    /* Moving backwards would incorrectly lower the globally confirmed tick. */
    if (tick < participant->acknowledged_tick) return UMI_STATUS_INVALID_STATE;
    participant->acknowledged_tick = tick;
    session->revision += 1U;
    refresh_confirmed_tick(session);
    return UMI_STATUS_OK;
}

/* Copy session state without returning participant-storage pointers. */
UmiStatus umi_game_session_snapshot(
    const UmiGameSession *session,
    UmiGameSessionSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Zeroing makes future extended fields safe for older callers and logs. */
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->mode = session->mode;
    out_snapshot->participant_count = session->count;
    out_snapshot->participant_capacity = session->capacity;
    out_snapshot->current_tick = session->current_tick;
    out_snapshot->confirmed_tick = session->confirmed_tick;
    out_snapshot->revision = session->revision;
    return UMI_STATUS_OK;
}
