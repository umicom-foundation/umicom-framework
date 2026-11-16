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
#include "umicom/local_models/request_binding.h"
#include <stdatomic.h>
#include "umicom/platform/file_lease.h"
#include "umicom/platform/filesystem.h"
#include <stdlib.h>
#include <string.h>

struct UmiModelSession {
    UmiProcessSupervisor *supervisor;
    UmiFileLease *lease;
    UmiProcessJobId job;
    /* The owner retains admitted launch settings independently of caller storage. */
    UmiModelRunPlan plan;
    /* A port and alias can be reused; host correlation must name this owner. */
    uint64_t identity;
};

/* Allocation is shared by independently owned sessions. Relaxed atomics only
 * assign unique values; the caller still serializes the session lifecycle.
 * Refuse exhaustion before a lease, file inspection or process submission.
 * Failed starts may consume identities, which must never be recycled. */
static atomic_uint_least64_t modelSessionIdentity = 0;
static UmiStatus ModelSessionIdentityReserve(uint64_t *outIdentity)
{
    uint_least64_t current = atomic_load_explicit(&modelSessionIdentity, memory_order_relaxed);
    for (;;) {
        if (current >= UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        uint_least64_t next = current + 1U;
        if (atomic_compare_exchange_weak_explicit(&modelSessionIdentity, &current,
                next, memory_order_relaxed, memory_order_relaxed)) {
            *outIdentity = (uint64_t)next;
            return UMI_STATUS_OK;
        }
    }
}

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
/* Each native model session now owns a complete captured run plan for stable SDK discovery. The former implementation is retained for owner review. */
#if 0
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
#endif
/* A session identity now prevents reused aliases and ports from accepting an earlier owner reply. Existing lease, submission and retained-cleanup behavior is preserved. The former implementation is retained for owner review. */
#if 0
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
    /* Freeze settings before lease/hash/submission so later descriptions are
     * tied to this owner's admitted plan, not a mutable application form. */
    session->plan = *plan;
    plan = &session->plan;
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

#endif
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
    status = ModelSessionIdentityReserve(&session->identity);
    if (status != UMI_STATUS_OK) { free(session); return status; }
    /* Freeze settings before lease/hash/submission so later descriptions are
     * tied to this owner's admitted plan, not a mutable application form. */
    session->plan = *plan;
    plan = &session->plan;
    status = UmiFileLeaseTryAcquire(plan->guard, &session->lease);
    if (status != UMI_STATUS_OK) { free(session); return status; }
    UmiModelInspection inspected;
    /* Basic header/hash inspection is retained below for review. Shared role
     * admission now also rejects auxiliary and unknown GGUF before creating
     * a supervisor; the original lease and failure cleanup remain unchanged. */
#if 0
    status = UmiModelInspect(plan->model, plan->sha256, NULL, &inspected);
#endif
    status = UmiModelInspectForRun(plan->model, plan->sha256, NULL, &inspected);
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
/* A failed Start may retain a cleanup owner without a submitted job. Do not
 * describe it as an admitted active configuration. Stopped owners are invalid
 * pointers and must never be supplied by the caller. */
UmiStatus UmiModelSessionReadPlan(const UmiModelSession *session, UmiModelRunPlan *outPlan)
{
    if (session == NULL || outPlan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->supervisor == NULL || session->job == 0U) return UMI_STATUS_INVALID_STATE;
    if (UmiModelRunPlanValidate(&session->plan) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    *outPlan = session->plan;
    return UMI_STATUS_OK;
}

/* Keep admission and plan ownership in the model service. Applications pass
 * value snapshots to their workers instead of holding mutable form settings. */
UmiStatus UmiModelSessionBindRequest(const UmiModelSession *session,
    const char *requestId, UmiModelRequestBinding *outBinding)
{
    if (session == NULL || outBinding == NULL ||
        !UmiModelNameValid(requestId, sizeof(outBinding->request_id), false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->identity == 0U) return UMI_STATUS_INVALID_STATE;
    UmiModelRunPlan plan;
    UmiStatus status = UmiModelSessionReadPlan(session, &plan);
    if (status != UMI_STATUS_OK) return status;
    uint64_t port = 0U, context = 0U, threads = 0U;
    if (!UmiModelNumber(plan.port, 1024U, 65535U, &port) ||
        !UmiModelNumber(plan.context, 512U, 32768U, &context) ||
        !UmiModelNumber(plan.threads, 1U, 256U, &threads)) return UMI_STATUS_INVALID_STATE;
    UmiModelRequestBinding candidate = {0};
    candidate.session_identity = session->identity;
    memcpy(candidate.request_id, requestId, strlen(requestId) + 1U);
    memcpy(candidate.model_sha256, plan.sha256, strlen(plan.sha256) + 1U);
    memcpy(candidate.model_alias, plan.alias, strlen(plan.alias) + 1U);
    candidate.port = (uint16_t)port;
    candidate.context_tokens = (uint32_t)context;
    candidate.threads = (uint32_t)threads;
    *outBinding = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiModelSessionCheckBinding(const UmiModelSession *session,
    const UmiModelRequestBinding *binding, const char *expectedRequestId)
{
    if (session == NULL || binding == NULL || binding->session_identity == 0U ||
        !UmiModelNameValid(binding->request_id, sizeof(binding->request_id), false) ||
        !UmiModelNameValid(binding->model_alias, sizeof(binding->model_alias), false) ||
        binding->model_alias[0] == '-' ||
        !UmiModelDigestValid(binding->model_sha256, 64U) || binding->port < 1024U ||
        binding->context_tokens < 512U || binding->context_tokens > 32768U ||
        binding->threads < 1U || binding->threads > 256U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiModelRequestBinding current;
    UmiStatus status = UmiModelSessionBindRequest(session, expectedRequestId, &current);
    if (status != UMI_STATUS_OK) return status;
    /* Compare named fields, not padding. A validly encoded mismatch is stale
     * context, not malformed input, and must not update the current owner. */
    if (binding->session_identity != current.session_identity ||
        strcmp(binding->request_id, current.request_id) != 0 ||
        strcmp(binding->model_sha256, current.model_sha256) != 0 ||
        strcmp(binding->model_alias, current.model_alias) != 0 ||
        binding->port != current.port || binding->context_tokens != current.context_tokens ||
        binding->threads != current.threads) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
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

UmiStatus UmiModelHealthProbe(uint16_t port, uint32_t timeoutMillis, bool approved,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiLocalHealth *outHealth)
{
    if (outHealth == NULL || port < 1024U || timeoutMillis < 100U || timeoutMillis > 60000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    /* Reuse the same Framework transport as every local-chat client. Health
     * remains a separate observation; no model-session state is rewritten. */
    return UmiAiWorkspaceLocalHealthRead(port, timeoutMillis, cancellation, outHealth);
}
