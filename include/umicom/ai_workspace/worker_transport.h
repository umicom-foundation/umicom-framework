/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/worker_transport.h
 *
 * PURPOSE:
 *   Expose bounded local worker operations to native and thin web clients.
 *
 * EDUCATIONAL OVERVIEW:
 *   Network policy belongs in a reusable service. Loopback ports can represent
 *   authenticated tunnels; model weights and GPU memory remain on the worker.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_WORKER_TRANSPORT_H
#define UMICOM_AI_WORKSPACE_WORKER_TRANSPORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_WORKER_JSON_LIMIT (4U * 1024U * 1024U)
#define UMI_AI_WORKER_MEDIA_LIMIT (256U * 1024U * 1024U)

/* The caller owns its sink and may refuse a write. No response is a file path. */
typedef bool (*UmiAiWorkerWrite)(void *context, const void *bytes, size_t length);
typedef bool (*UmiAiWorkerCancelled)(void *context);
typedef struct UmiAiWorkerEndpoint {
    uint16_t port;              /* Loopback only; use a separately owned SSH tunnel. */
    uint32_t timeoutMillis;     /* 100..60000 per HTTP operation. */
    UmiAiWorkerCancelled cancelled;
    void *cancellationContext;
} UmiAiWorkerEndpoint;

/* Validate a bounded JSON object, rejecting trailing documents and embedded NULs.
 * The returned opaque json-c object is owned by the caller and released normally.
 * This helper is internal to native integration; ordinary callers use Execute. */
struct json_object;
UmiStatus UmiAiWorkerParseJson(const char *text, size_t length, struct json_object **out);

/* Dispatch a bounded, explicitly approved operation to a selected local port.
 * Supported actions: media-info, media-checkpoints, media-submit, media-image,
 * media-status, media-output, text-models and text-tokenize. Model chat itself
 * reuses UmiAiWorkspaceLocalProviderCreate, not a parallel text provider.
 *
 * JSON actions write a JSON response; media-output writes raw bytes only after
 * checking an output descriptor against the selected prompt's own history.
 * A failed call can leave partial sink data; the caller must discard it.
 * There are no redirects, proxy inheritance, remote URLs, downloads of weights,
 * retries, global queue clears, shell commands or unrestricted custom nodes.
 * The API does not authenticate an HTTP server: access control is the caller's
 * SSH/TLS gateway responsibility. Do not expose the worker ports to the LAN. */
UmiStatus UmiAiWorkerExecute(const UmiAiWorkerEndpoint *endpoint,
    const char *json, size_t length, UmiAiWorkerWrite write, void *writeContext,
    char *error, size_t errorCapacity);

#ifdef __cplusplus
}
#endif
#endif
