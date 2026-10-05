/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/response_tree.h
 * PURPOSE: Validate an owned JSON-RPC response before exposing its result to language tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_RESPONSE_TREE_H
#define UMICOM_LANGUAGE_RUNTIME_RESPONSE_TREE_H
#include "umicom/language_runtime/json_tree.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Parse an exact response byte span and match its positive numeric request ID.
 * Require jsonrpc "2.0", no method, and exactly one result or error member.
 * A well-formed remote error returns UNAVAILABLE; a different ID returns
 * NOT_FOUND. Duplicate members and malformed errors are rejected. Unknown
 * extension fields may be present, but are never interpreted as instructions.
 * On success the caller owns *out_tree and out_result identifies its result,
 * which may be JSON null. On failure outputs are NULL and -1. The supplied
 * limits and cancellation policy are forwarded to the shared JSON reader. */
    UmiStatus UmiLanguageResponseTreeRead(const void *json, size_t bytes, uint64_t expected_request_id,
                                          const UmiJsonTreeLimits *limits, const UmiCancellationToken *cancel,
                                          UmiJsonTree **out_tree, int *out_result);
#ifdef __cplusplus
}
#endif
#endif
