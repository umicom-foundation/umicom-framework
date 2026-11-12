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
/* Request cancellation, join and release. On supervisor failure retain owner
 * for retry. Always stop/join application requests before stopping the server. */
UmiStatus UmiModelSessionStop(UmiModelSession **inOutSession);
/* Connect a thin application client through the existing bounded, loopback AI
 * provider. This does not probe health or prove endpoint ownership. The server
 * must be trusted; ordinary loopback is shared with other local processes.
 * Successful registry add transfers ownership; otherwise caller destroys it. */
UmiStatus UmiModelLocalProviderCreate(uint16_t port, uint32_t timeoutMillis,
    const UmiAiWorkspaceCancellation *cancellation, UmiAiProvider *outProvider);
/* Shared CLI entry excluding executable and the optional 'models' prefix. */
int UmiModelsCommand(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
