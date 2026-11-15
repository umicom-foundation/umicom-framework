/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/local_models/request_binding.h
 *
 * PURPOSE:
 *   Correlate local model replies with the host session and request that admitted them.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_LOCAL_MODELS_REQUEST_BINDING_H
#define UMICOM_LOCAL_MODELS_REQUEST_BINDING_H
#include "umicom/local_models/manager.h"
#ifdef __cplusplus
extern "C" {
#endif

/* A host-owned correlation value, not a server credential or readiness proof.
 * Identifiers are unique only within this loaded Framework instance. Do not
 * persist them across process runs or compare separate copies of the library.
 * Keep this value with a pending request; do not trust fields supplied by a
 * remote response. The host chooses a nonempty, session-unique request_id. */
typedef struct UmiModelRequestBinding {
    uint64_t session_identity;
    char request_id[128];
    char model_sha256[65];
    char model_alias[128];
    uint16_t port;
    uint32_t context_tokens;
    uint32_t threads;
} UmiModelRequestBinding;

/* Capture the immutable settings of a submitted owner without probing health,
 * reading files, starting a process or sending a request. requestId accepts
 * 1..127 ASCII letters, digits, dots, hyphens and underscores, except . and .. .
 * The owner must remain alive and serialized with lifecycle calls. A cleanup
 * owner without a submitted job is INVALID_STATE. Failure leaves output intact.
 * requestId may refer to outBinding->request_id: capture completes first. */
UmiStatus UmiModelSessionBindRequest(const UmiModelSession *session,
    const char *requestId, UmiModelRequestBinding *outBinding);

/* Validate a host-captured binding against the current owner and expected
 * pending request ID. Malformed input returns INVALID_ARGUMENT; a different
 * owner, request or admitted setting returns INVALID_STATE. No values change.
 * Reusing a request ID in the same session is outside this correlation contract.
 * An exited process may still have a live owner: this check is not liveness.
 * Never pass a session pointer after Stop has consumed its owner. */
UmiStatus UmiModelSessionCheckBinding(const UmiModelSession *session,
    const UmiModelRequestBinding *binding, const char *expectedRequestId);

#ifdef __cplusplus
}
#endif
#endif
