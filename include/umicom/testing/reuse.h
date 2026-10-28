/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/reuse.h
 *
 * PURPOSE:
 *   Retain individual test outcomes without treating cached evidence as a fresh pass.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_REUSE_H
#define UMICOM_TESTING_REUSE_H
#include "umicom/testing/ctest_report.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TEST_REUSE_MAX_BYTES (64U * 1024U * 1024U)
typedef struct UmiTestReuseState UmiTestReuseState;
/* The private receipt format is versioned, bounded and contains no executable
 * code: header "UMICOM-TEST-REUSE 1 <context SHA256>\n", then name<TAB>input
 * SHA256<TAB>state-number\n. Names are printable ASCII, at most 255 bytes.
 * Sorted unique names give deterministic exports and bounded lookup costs. */
UmiStatus UmiTestReuseImport(const char *text, size_t length, UmiTestReuseState **out);
void UmiTestReuseDestroy(UmiTestReuseState *state);
size_t UmiTestReuseCount(const UmiTestReuseState *state);
const char *UmiTestReuseName(const UmiTestReuseState *state, size_t index);
UmiTestState UmiTestReuseOutcome(const UmiTestReuseState *state, size_t index);
/* current is a manifest of tests whose complete input contract the producer
 * has verified. Unknown/dynamic/service/fixture tests must not be included.
 * Preserve only matching previous PASSED records. Everything else becomes
 * NOT_RUN before execution, so an interrupted attempt cannot reuse an old pass.
 * A context change invalidates every receipt. The inputs are never modified. */
UmiStatus UmiTestReusePrepare(const UmiTestReuseState *current,
    const UmiTestReuseState *previous, UmiTestReuseState **out);
/* Import a complete, strictly parsed CTest JUnit observation. Unknown tests are
 * retained by the caller's original report, not added to this closed-input cache.
 * Missing tests remain NOT_RUN. Skips, disabled tests and failures never qualify
 * for reuse. Caller must check stable inputs before and after execution.
 * This API makes no assertion that a supplied report belongs to a current run. */
UmiStatus UmiTestReuseRecord(UmiTestReuseState *state, const char *xml, size_t length);
/* Measure with NULL/zero. required includes NUL; insufficient capacity changes
 * neither the buffer nor required. Persist with the platform atomic-file API. */
UmiStatus UmiTestReuseExport(const UmiTestReuseState *state, char *text,
    size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
