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
    /** Identify the resolved project directory and copied profile values.
     * Paths are resolved when called, without reading source files or launching
     * programs. Prefer absolute source directories in persistent hosts.
     * Inputs remains empty: a settings match is not source/build qualification.
     * Output is unchanged on failure. Hashes are not encryption; never put
     * credentials in build settings. The phase is recorded separately by the
     * job caption; this identity permits comparing settings across phases. */
    UmiStatus UmiBuildProfileJobIdentity(const UmiBuildProfile *profile, UmiJobIdentity *out);
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
