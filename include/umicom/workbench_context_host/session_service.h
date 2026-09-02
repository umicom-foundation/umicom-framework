/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/session_service.h
 *
 * PURPOSE:
 *   Persist context-host assignments through the Framework Session Store using
 *   bounded, line-safe chunked values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SESSION_SERVICE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SESSION_SERVICE_H

#include "umicom/platform/session_store.h"
#include "umicom/workbench_context_host/session.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Session Store values are intentionally single-line values. Context-host
 * sessions are multi-line text, so session_service.c encodes each raw chunk as
 * hexadecimal before it crosses the Session Store boundary.
 *
 * "H1:" consumes three bytes in the stored value and hexadecimal needs two
 * output characters per input byte. The expression below guarantees the
 * encoded value plus its terminating NUL always fits UMI_SESSION_VALUE_CAPACITY.
 */
#define UMI_WORKBENCH_CONTEXT_HOST_SESSION_CHUNK_CAPACITY \
    ((UMI_SESSION_VALUE_CAPACITY - 4U) / 2U)

/*
 * A complete encoded context-host session is bounded at 65536 bytes. Eighty
 * chunks provide sufficient room even in the worst case after line-safe hex
 * encoding while remaining well below the Session Store entry limit.
 */
#define UMI_WORKBENCH_CONTEXT_HOST_SESSION_MAX_CHUNKS 80U

/**
 * Write workbench context host session in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_context_host_session_save(
    const UmiWorkbenchContextHost *host,
    UmiSessionStore *store,
    const char *key_prefix);
/**
 * Provide the workbench context host session restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_session_restore(
    UmiWorkbenchContextHost *host,
    const UmiSessionStore *store,
    const char *key_prefix,
    bool *out_restored);
/**
 * Remove workbench context host session while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_workbench_context_host_session_remove(
    UmiSessionStore *store,
    const char *key_prefix);

#ifdef __cplusplus
}
#endif

#endif
