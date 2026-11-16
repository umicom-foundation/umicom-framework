/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/document_file.h
 *
 * PURPOSE:
 *   Read one canonical saved connection observation without activating settings or publishing partial outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_PROVIDER_CONNECTIONS_DOCUMENT_FILE_H
#define UMICOM_PROVIDER_CONNECTIONS_DOCUMENT_FILE_H
#include "umicom/provider_connections/document.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Read one ordinary absolute UTF-8 regular filename under the existing document
 * byte limit (strictly below 8192 bytes). Reuse the shared file admission and
 * canonical document decoder. Final links/reparse points, devices, directories
 * and observable read-time changes are refused; trusted parent aliases may be
 * followed. This is not filesystem confinement or an immutable snapshot.
 *
 * Publish connection and historical revision together only after read/close,
 * decoding and a final cancellation check succeed. Every refusal preserves both
 * outputs. Clear temporary owned file bytes and staged settings before return.
 * Inputs and outputs must not overlap. I/O may block: GUI hosts use a worker.
 * Cancellation is checked before/after I/O and before publication, not inside
 * a blocking read. The caller keeps its token alive for the complete call.
 *
 * No settings store, credential vault or provider is accessed. Imported secret
 * REFERENCES are untrusted metadata; never resolve one without current scope
 * authentication. A saved revision is not permission to overwrite live settings. */
UmiStatus UmiProviderConnectionDocumentFileRead(const char *path,
    const UmiCancellationToken *cancel,UmiProviderConnection *out_connection,
    uint64_t *out_observed_revision);
#ifdef __cplusplus
}
#endif
#endif
