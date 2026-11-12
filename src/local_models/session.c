/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/session.c
 *
 * PURPOSE:
 *   Own a verified model server process tree without discarding an active session during replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/platform/file_lease.h"
#include "umicom/platform/filesystem.h"
#include <stdlib.h>
#include <string.h>

struct UmiModelSession {
    UmiProcessSupervisor *supervisor;
    UmiFileLease *lease;
    UmiProcessJobId job;
};
/* A trusted engine still reads inherited configuration. Refuse known settings
 * that enable agents, alternate models, routing, tunnels or authentication.
 * Explicit arguments own host/model/port/CPU settings. Future engine releases
 * require a compatibility review; this is not a universal execution sandbox. */
static bool EnvironmentIsReviewed(void)
{
    static const char *const names[] = {
        "LLAMA_ARG_TOOLS", "LLAMA_ARG_MCP_SERVERS_CONFIG", "LLAMA_ARG_MCP_SERVERS_JSON",
        "LLAMA_ARG_AGENT", "LLAMA_ARG_UI_MCP_PROXY", "LLAMA_ARG_CONNECT", "LLAMA_ARG_CONNECT_CODE",
        "LLAMA_ARG_MODELS_DIR", "LLAMA_ARG_MODELS_PRESET", "LLAMA_ARG_MODEL_DRAFT",
        "LLAMA_ARG_MODEL_URL_DRAFT", "LLAMA_ARG_HF_REPO", "LLAMA_ARG_HF_FILE",
        "LLAMA_ARG_MODEL_URL", "LLAMA_ARG_MMPROJ", "LLAMA_ARG_MMPROJ_URL",
        "LLAMA_API_KEY", "LLAMA_ARG_API_KEY_FILE", "LLAMA_ARG_API_PREFIX",
        "LLAMA_ARG_SSL_KEY_FILE", "LLAMA_ARG_SSL_CERT_FILE"
    };
    for (size_t i = 0U; i < sizeof(names) / sizeof(names[0]); ++i) {
        const char *value = getenv(names[i]);
        if (value != NULL && value[0] != '\0') return false;
    }
    return true;
}
UmiStatus UmiModelSessionStart(const UmiModelRunPlan *plan, bool approved, UmiModelSession **inOutSession)
{
    if (inOutSession == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* An unsuccessful replacement must never close a previously usable model.
     * Stop is a separate owner action, not an implicit side effect of Start. */
    if (*inOutSession != NULL) return UMI_STATUS_BUSY;
    UmiStatus status = UmiModelRunPlanValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    if (!EnvironmentIsReviewed()) return UMI_STATUS_INVALID_STATE;
    if (!umi_fs_is_file(plan->executable)) return UMI_STATUS_NOT_FOUND;
    UmiModelSession *session = calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiFileLeaseTryAcquire(plan->guard, &session->lease);
    if (status != UMI_STATUS_OK) { free(session); return status; }
    UmiModelInspection inspected;
    status = UmiModelInspect(plan->model, plan->sha256, NULL, &inspected);
    if (status != UMI_STATUS_OK) goto failure;
    UmiProcessSupervisorConfig config = umi_process_supervisor_config_default();
    config.capacity = 1U;
    status = umi_process_supervisor_create(&config, &session->supervisor);
    if (status != UMI_STATUS_OK) goto failure;
    const char *arguments[UMI_MODEL_RUN_ARGUMENTS];
    size_t count = 0U;
    status = UmiModelRunArguments(plan, arguments, UMI_MODEL_RUN_ARGUMENTS, &count);
    if (status != UMI_STATUS_OK) goto failure;
    UmiProcessRequest request = {0};
    request.program = plan->executable;
    request.arguments = arguments;
    request.argument_count = count;
    request.working_directory = plan->directory;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.poll_interval_ms = 50U;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    status = UmiProcessSupervisorSubmitWithLifetime(session->supervisor,
        "Umicom local text model", &request, UMI_PROCESS_LIFETIME_TREE, &session->job);
    if (status != UMI_STATUS_OK) goto failure;
    *inOutSession = session;
    return UMI_STATUS_OK;
failure:
    /* Publish a retained owner if cleanup cannot finish, so the caller has a
     * retry path instead of losing live native resources after an error. */
    if (session->supervisor != NULL) {
        UmiStatus stopped = UmiProcessSupervisorRelease(&session->supervisor);
        if (stopped != UMI_STATUS_OK) { *inOutSession = session; return stopped; }
    }
    UmiStatus released = UmiFileLeaseRelease(&session->lease);
    free(session);
    return released == UMI_STATUS_OK ? status : released;
}
UmiStatus UmiModelSessionRead(const UmiModelSession *session, UmiProcessJobSnapshot *outSnapshot)
{
    if (session == NULL || session->supervisor == NULL || session->job == 0U || outSnapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_process_supervisor_snapshot(session->supervisor, session->job, outSnapshot);
}
UmiStatus UmiModelSessionStop(UmiModelSession **inOutSession)
{
    if (inOutSession == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiModelSession *session = *inOutSession;
    if (session == NULL) return UMI_STATUS_OK;
    UmiStatus status = UmiProcessSupervisorRelease(&session->supervisor);
    if (status != UMI_STATUS_OK) return status;
    status = UmiFileLeaseRelease(&session->lease);
    free(session);
    *inOutSession = NULL;
    return status;
}
UmiStatus UmiModelLocalProviderCreate(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiProvider *outProvider)
{
    /* All product clients keep one transport implementation and its existing
     * message limits, policy, JSON validation and cancellation semantics. */
    return UmiAiWorkspaceLocalProviderCreate(port, timeoutMillis, cancellation, outProvider);
}
