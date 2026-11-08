/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/run_restart.inc
 *
 * PURPOSE:
 *   Keep stop-and-relaunch admission inside the Framework project session owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Included by project_session.c: all public entry points below belong to the
 * same owner thread as submit/destroy. Worker state is read through snapshots. */
static void RestartObserveStop(UmiBuildProjectSession *session)
{
    if (session->restart.pending && atomic_load(&session->restart_stop_requested)) {
        session->restart.pending = false;
        session->restart.status = UMI_STATUS_CANCELLED;
    }
}

static UmiStatus RestartCheckAuthority(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, const char *project_root, bool trusted)
{
    if (session == NULL || profile == NULL || project_root == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!trusted) return UMI_STATUS_PERMISSION_DENIED;
    UmiStatus status = UmiBuildProfileCheckProject(profile, project_root);
    if (status != UMI_STATUS_OK) return status;
    UmiJobIdentity current;
    status = UmiBuildProfileJobIdentity(profile, &current);
    if (status != UMI_STATUS_OK) return status;
    if (!UmiJobIdentityIsRecorded(&session->submitted_identity) ||
        strcmp(current.subject, session->submitted_identity.subject) != 0 ||
        strcmp(current.configuration, session->submitted_identity.configuration) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus UmiBuildRunRestartReview(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, const char *project_root, bool trusted)
{
    UmiStatus status = RestartCheckAuthority(session, profile, project_root, trusted);
    if (status != UMI_STATUS_OK) return status;
    RestartObserveStop(session);
    if (session->restart.pending) return UMI_STATUS_BUSY;
    UmiBuildProjectSessionSnapshot snapshot;
    status = umi_build_project_session_snapshot(session, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.active || snapshot.cancellation_requested ||
        atomic_load(&session->restart_stop_requested) ||
        snapshot.requested_phase != UMI_BUILD_PHASE_RUN ||
        snapshot.current_phase != UMI_BUILD_PHASE_RUN)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus UmiBuildRunRestartRequest(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, const char *project_root, bool trusted)
{
    UmiStatus status = UmiBuildRunRestartReview(session, profile, project_root, trusted);
    if (status != UMI_STATUS_OK) return status;
    UmiBuildProjectSessionSnapshot current;
    status = umi_build_project_session_snapshot(session, &current);
    if (status != UMI_STATUS_OK) return status;
    UmiBuildRunRestartSnapshot intent = {0};
    intent.previous_operation_id = current.operation_id;
    intent.status = UMI_STATUS_BUSY;
    UmiBuildLogSnapshot log;
    status = UmiBuildProjectSessionReadLog(session, &log);
    if (status != UMI_STATUS_OK) return status;
    if (log.enabled) {
        /* A deterministic sibling makes retained evidence discoverable. Exclusive
         * output creation at submission refuses collisions rather than erasing it. */
        int length = snprintf(intent.log_path, sizeof intent.log_path,
            "%s.restart-%" PRIu64, log.file.path, current.operation_id + UINT64_C(1));
        if (length < 0 || (size_t)length >= sizeof intent.log_path)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        status = UmiOutputFileValidatePath(intent.log_path);
        if (status != UMI_STATUS_OK) return status;
    }
    intent.pending = true;
    session->restart = intent;
    /* General Cancel also revokes intent; use the shared token here so only
     * this accepted restart survives the cancellation it requested itself. */
    umi_cancellation_token_request(session->cancellation);
    return UMI_STATUS_OK;
}

UmiStatus UmiBuildRunRestartAdvance(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, const char *project_root, bool trusted,
    size_t consumed_results, bool *submitted)
{
    if (submitted != NULL) *submitted = false;
    if (session == NULL || submitted == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    RestartObserveStop(session);
    if (!session->restart.pending) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = RestartCheckAuthority(session, profile, project_root, trusted);
    UmiBuildProjectSessionSnapshot current = {0};
    if (status == UMI_STATUS_OK)
        status = umi_build_project_session_snapshot(session, &current);
    if (status == UMI_STATUS_OK && current.operation_id != session->restart.previous_operation_id)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && (current.active ||
        consumed_results < current.completed_phase_count)) return UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK && consumed_results != current.completed_phase_count)
        status = UMI_STATUS_INVALID_ARGUMENT;
    /* Revoke first: an admission failure is terminal, never an idle-loop retry.
     * Ordinary Run preparation rechecks the selected file and working directory. */
    session->restart.pending = false;
    if (status == UMI_STATUS_OK) {
        const char *log_path = session->restart.log_path[0] != '\0'
            ? session->restart.log_path : NULL;
        session->restart_submitting = true;
        status = UmiBuildProjectSessionRunCurrent(session, profile, true, log_path);
        session->restart_submitting = false;
        if (status == UMI_STATUS_OK) {
            session->restart.submitted_operation_id = session->snapshot.operation_id;
            *submitted = true;
        }
    }
    session->restart.status = status;
    return status;
}

UmiStatus UmiBuildRunRestartRead(UmiBuildProjectSession *session,
    UmiBuildRunRestartSnapshot *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    RestartObserveStop(session);
    *out_snapshot = session->restart;
    return UMI_STATUS_OK;
}
