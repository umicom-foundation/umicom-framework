/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/document.h
 *
 * PURPOSE:
 *   Exchange versioned non-secret connection observations without bypassing settings validation or persistence ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_PROVIDER_CONNECTIONS_DOCUMENT_H
#define UMICOM_PROVIDER_CONNECTIONS_DOCUMENT_H
#include "umicom/provider_connections/connections.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES 8192U
/* Export one validated record and nonzero observed scope revision. The exact
 * version-1 text is "umicom-provider-connection\n1\n", canonical decimal
 * revision plus LF, followed by the existing canonical connection wire record.
 * Its text fields are lowercase hex-encoded UTF-8, not encrypted. Only public
 * settings and credential REFERENCES belong in the record; never secret values.
 * No store or vault is read. The document contains no application/profile scope.
 *
 * capacity includes the NUL terminator; *out_length excludes it. Buffer, length
 * output and inputs must not overlap. On refusal both outputs are unchanged.
 * Caller-owned buffers may contain sensitive context and should be cleared
 * after use. Existing internal persistence format and fingerprint are retained. */
UmiStatus UmiProviderConnectionDocumentExport(const UmiProviderConnection *connection,
    uint64_t observed_revision,char *out,size_t capacity,size_t *out_length);
/* Read exactly length bytes, with no terminator required. Embedded NUL, CRLF,
 * signs, leading zeroes, noncanonical hex, malformed/trailing data and zero
 * revision are refused. Unknown numeric versions return NOT_IMPLEMENTED.
 * length must be below MAX_BYTES, reserving one byte for private termination.
 * Inputs and outputs must not overlap. Failure preserves both output values.
 *
 * Decoding does not save or activate anything. The revision is an untrusted
 * historical observation, not a token issued by the current store. Re-read
 * current scoped settings, review differences and explicitly save using the
 * current store revision. Never resolve an imported vault reference without
 * current profile authentication and destination review. */
UmiStatus UmiProviderConnectionDocumentImport(const void *bytes,size_t length,
    UmiProviderConnection *out_connection,uint64_t *out_observed_revision);
#ifdef __cplusplus
}
#endif
#endif
