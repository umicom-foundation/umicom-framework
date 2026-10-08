/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/job_history.h
 * PURPOSE: Attach persistent job evidence to an existing project build session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_JOB_HISTORY_H
#define UMICOM_BUILD_JOB_HISTORY_H
#include "umicom/build/project_session.h"
#include "umicom/data/job_history.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiBuildJobHistoryState
    {
        bool enabled;
        UmiStatus storage_status;
        UmiJobHistoryEntry entry;
    } UmiBuildJobHistoryState;
    /* Owner-thread operation while idle; the history is borrowed until detached or
 * the session is destroyed. Join the session before closing its Data Server.
 * NULL restores the existing in-memory-only workflow. No job is replayed.
 * Hosts attach histories explicitly after selecting a private local database. */
    UmiStatus UmiBuildProjectSessionSetHistory(UmiBuildProjectSession *session, UmiJobHistory *history);
    /* Poll copied metadata without reading SQLite or touching a worker's record. */
    UmiStatus UmiBuildProjectSessionReadHistory(UmiBuildProjectSession *session,
                                                UmiBuildJobHistoryState *out_state);
#ifdef __cplusplus
}
#endif
#endif
