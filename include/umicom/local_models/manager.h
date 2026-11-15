/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/local_models/manager.h
 *
 * PURPOSE:
 *   Share reviewed model downloads, GGUF inspection and owned local server sessions across Umicom products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LOCAL_MODELS_MANAGER_H
#define UMICOM_LOCAL_MODELS_MANAGER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include "umicom/platform/process_supervisor.h"
#include "umicom/ai_workspace/providers.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_MODEL_PATH_CAPACITY 4096U
#define UMI_MODEL_URL_CAPACITY 1024U
#define UMI_MODEL_MAX_BYTES UINT64_C(1099511627776)
#define UMI_MODEL_RUN_ARGUMENTS 18U

/* A revision pins repository metadata; SHA-256 pins the actual file bytes.
 * The catalogue is deliberately small, not a hardware or licence endorsement. */
typedef struct UmiModelSource {
    char repository[128];
    char revision[41];
    char filename[512];
    char sha256[65];
    uint64_t bytes;
} UmiModelSource;
typedef struct UmiModelCatalogueEntry {
    const char *id;
    const char *title;
    const char *licence;
    UmiModelSource source;
} UmiModelCatalogueEntry;
size_t UmiModelCatalogueCount(void);
const UmiModelCatalogueEntry *UmiModelCatalogueAt(size_t index);
const UmiModelCatalogueEntry *UmiModelCatalogueFind(const char *id);
UmiStatus UmiModelSourceValidate(const UmiModelSource *source);

typedef struct UmiModelDownloadPlan {
    UmiModelSource source;
    char url[UMI_MODEL_URL_CAPACITY];
    char destination[UMI_MODEL_PATH_CAPACITY];
} UmiModelDownloadPlan;
/* Pure preview: no network, filesystem access, directory creation or execution.
 * Only public Hugging Face GGUF files at full 40-digit commits are supported.
 * The selected destination must end in .gguf, under existing trusted parents. */
UmiStatus UmiModelDownloadPlanCreate(const UmiModelSource *source,
    const char *destination, UmiModelDownloadPlan *outPlan);
bool UmiModelDownloadAvailable(void);

typedef struct UmiModelInspection {
    uint64_t bytes;
    uint64_t tensors;
    uint64_t metadata_entries;
    uint32_t gguf_version;
    char sha256[65];
} UmiModelInspection;
/* Hash the whole file with bounded memory. Check GGUF magic, v2/v3 little-endian
 * header and basic counts; this is not a tensor parser or compatibility test.
 * Optional expectedSha256 must be 64 lowercase hex digits. No result on error.
 * Files must remain stable until the external engine finishes using them. */
UmiStatus UmiModelInspect(const char *path, const char *expectedSha256,
    const UmiCancellationToken *cancellation, UmiModelInspection *outInspection);
/* Explicit network/write approval. The destination is created exclusively and
 * is never overwritten. Partial or rejected bytes are retained for review:
 * a file's existence alone NEVER means it is installed/verified. Success
 * requires HTTP 200, exact byte count, complete GGUF inspection and SHA-256.
 * No automatic resume, tokens, credential lookup, proxies or model execution.
 * Cancellation is borrowed during this blocking call. Use a worker in a GUI. */
UmiStatus UmiModelDownload(const UmiModelDownloadPlan *plan, bool approved,
    const UmiCancellationToken *cancellation, UmiModelInspection *outInspection);

typedef struct UmiModelRunPlan {
    char executable[UMI_MODEL_PATH_CAPACITY];
    char model[UMI_MODEL_PATH_CAPACITY];
    char directory[UMI_MODEL_PATH_CAPACITY];
    char guard[UMI_MODEL_PATH_CAPACITY];
    char sha256[65];
    char alias[128];
    char port[6];
    char context[6];
    char threads[4];
} UmiModelRunPlan;
/* Pure preview. CPU-only, one inference slot, loopback only. The caller selects
 * an already-installed trusted llama-server; this service never installs it.
 * Fixed arguments require an engine supporting --offline and --no-webui.
 * Context 512..32768, threads 1..256, port 1024..65535. No arbitrary flags. */
UmiStatus UmiModelRunPlanCreate(const char *executable, const char *model,
    const char *sha256, const char *alias, uint16_t port, uint32_t context,
    uint32_t threads, UmiModelRunPlan *outPlan);
/* Borrow arguments until plan changes/dies; no quoting or shell interpretation.
 * argv excludes the executable; capacity must be UMI_MODEL_RUN_ARGUMENTS. */
UmiStatus UmiModelRunArguments(const UmiModelRunPlan *plan,
    const char **arguments, size_t capacity, size_t *outCount);

typedef struct UmiModelSession UmiModelSession;
/* All lifecycle calls on a session must be serialized by its owner. Start
 * refuses a nonempty owner BEFORE inspecting/replacing any existing session.
 * It rehashes the model, takes a cooperative <model>.umicom-run.lock lease and
 * submits an owned process tree. An OK return is submission, NOT readiness.
 * Session owns supervisor/lease; the executable and model must be trusted.
 * Environment is inherited by the platform runner. This is not a sandbox. */
UmiStatus UmiModelSessionStart(const UmiModelRunPlan *plan, bool approved,
    UmiModelSession **inOutSession);
UmiStatus UmiModelSessionRead(const UmiModelSession *session, UmiProcessJobSnapshot *outSnapshot);
/* Copy the plan captured at successful submission, independently of later
 * edits to the caller's plan. No process/health/file observation is performed.
 * A failed-start cleanup owner returns INVALID_STATE. Output stays unchanged
 * on failure. Serialize with all other session operations and keep it alive. */
UmiStatus UmiModelSessionReadPlan(const UmiModelSession *session, UmiModelRunPlan *outPlan);

/* Request cancellation, join and release. On supervisor failure retain owner
 * for retry. Always stop/join application requests before stopping the server. */
UmiStatus UmiModelSessionStop(UmiModelSession **inOutSession);
/* Connect a thin application client through the existing bounded, loopback AI
 * provider. This does not probe health or prove endpoint ownership. The server
 * must be trusted; ordinary loopback is shared with other local processes.
 * Successful registry add transfers ownership; otherwise caller destroys it. */
UmiStatus UmiModelLocalProviderCreate(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiProvider *outProvider);
/* One explicitly approved health GET through the shared local AI transport.
 * No process is started, stopped or relabelled. Output is unchanged on failure.
 * A successful observation does not prove model or endpoint identity. */
UmiStatus UmiModelHealthProbe(uint16_t port, uint32_t timeoutMillis, bool approved,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiLocalHealth *outHealth);
/* Readiness is an explicit sequence of observations, never a process action.
 * Each request timeout is 100..60000 ms, attempts 1..64 and pause 50..5000 ms.
 * The sum of request timeouts and intervening pauses must not exceed 60000 ms.
 * That sum is a nominal budget, not a wall-clock deadline: scheduling and
 * transport cleanup can add time. The last attempt has no following pause. */
typedef struct UmiModelReadinessPlan {
    uint16_t port;
    uint32_t attempts;
    uint32_t interval_millis;
    uint32_t timeout_millis;
} UmiModelReadinessPlan;
/* Pure admission; outputs remain unchanged on failure. Budget also validates
 * a caller-edited plan. Neither function accesses an endpoint or a file. */
UmiStatus UmiModelReadinessPlanCreate(uint16_t port, uint32_t attempts,
    uint32_t intervalMillis, uint32_t timeoutMillis, UmiModelReadinessPlan *outPlan);
UmiStatus UmiModelReadinessBudget(const UmiModelReadinessPlan *plan, uint32_t *outMillis);
typedef struct UmiModelReadinessReport {
    uint32_t attempts_completed;
    UmiStatus last_probe_status;
    bool has_health;
    UmiAiLocalHealth last_health;
} UmiModelReadinessReport;
/* Invalid arguments, denied approval and pre-cancellation leave output intact.
 * After admission, EVERY return publishes this call's report, including on
 * timeout, cancellation or a fatal probe error. has_health describes only the
 * last completed probe; a failed later probe clears the earlier observation.
 * OK means the last endpoint report was ready. Exhaustion returns TIMEOUT;
 * last_probe_status can still be OK when that report was unavailable (503).
 * IO_ERROR and TIMEOUT are retried, including unexpected HTTP statuses mapped
 * to IO_ERROR by the health transport. Malformed/oversized responses and other
 * errors stop immediately. No retry occurs after cancellation or readiness.
 * Call on a worker; keep the borrowed token alive until that worker joins.
 * Inputs are captured before output is published. Serialize caller access.
 * No model, session, provider registry or process state is changed. */
UmiStatus UmiModelReadinessWait(const UmiModelReadinessPlan *plan, bool approved,
    const UmiAiWorkspaceCancellation *cancellation, UmiModelReadinessReport *outReport);
/* Shared console execution with an explicit session owner. Initialize the slot
 * to NULL. An occupied slot returns the BUSY diagnostic before any command is
 * parsed or executed; resolve it with UmiModelSessionStop first.
 * Any non-null slot on return remains owned by the caller, even when the
 * command returned nonzero. Some cleanup errors consume the owner: inspect
 * both the exit code and slot. Keep a retained slot reachable and retry checked
 * stop deliberately. Never free/overwrite the pointer to hide a failed stop.
 * Preview/refusal paths leave an empty slot empty. All console operations and
 * their stdin/stdout access must be serialized by the host. This function is
 * blocking and is not a GUI worker scheduler. Exit codes remain 0/1/2. */
int UmiModelsCommandWithSession(int argc, char **argv, UmiModelSession **inOutSession);
/* The legacy entry below uses a retained process-local owner when cleanup fails.
 * These accessors concern only that compatibility owner, never explicit slots.
 * Release is an explicit checked retry; it is idempotent when empty. A non-null
 * retained owner continues blocking legacy commands until the slot becomes empty.
 * A release error can consume the owner; check both status and the query.
 * Serialize these calls with all legacy command execution. Process exit is not
 * a substitute for verified cleanup and may leave platform-dependent children. */
bool UmiModelsCommandHasRetainedSession(void);
UmiStatus UmiModelsCommandRelease(void);
/* Shared CLI entry excluding executable and the optional 'models' prefix. */
int UmiModelsCommand(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
