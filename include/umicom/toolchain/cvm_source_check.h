/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/cvm_source_check.h
 *
 * PURPOSE:
 *   Associate optional compiler warning reports with explicit source revisions without executing code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TOOLCHAIN_CVM_SOURCE_CHECK_H
#define UMICOM_TOOLCHAIN_CVM_SOURCE_CHECK_H
#include "umicom/base/status.h"
#include "umicom_c/source_check.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCvmSourceCheckResult {
    uint64_t source_revision;
    bool completed;
    UmcSourceReport report;
    UmcDiagnostic diagnostic;
} UmiCvmSourceCheckResult;

/* The optional adapter requires the source-warning-capable Umicom C SDK.
 * Source and result are disjoint caller-owned storage, stable during this call.
 * No source pointer is retained. Wrong result size or NULL result is refused
 * without access to that object. Once result storage is admitted, a new request
 * clears the previous report before checking, including on source failure.
 * This deliberately differs from the lower-level compiler report transaction:
 * UI consumers must not confuse an old success with this new request.
 *
 * source_revision is an opaque caller token; zero is allowed. The caller must
 * change it whenever source content changes. This service does not hash text,
 * synchronize threads, schedule background work or execute the program. */
UmiStatus umi_cvm_check_source(const char *source, size_t length,
    uint64_t source_revision, UmiCvmSourceCheckResult *result, size_t result_size);

/* True only for a completed result with this caller token. A token comparison
 * cannot detect changed bytes if the caller incorrectly reuses a revision. */
bool umi_cvm_source_check_matches(const UmiCvmSourceCheckResult *result,
    size_t result_size, uint64_t current_source_revision);

#ifdef __cplusplus
}
#endif
#endif
