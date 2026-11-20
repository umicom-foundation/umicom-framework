/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/uat_target.h
 *
 * PURPOSE:
 *   Bind a workflow to one named test target and session, admitting only the
 *   operations approved by the fixture owner and supported by its adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_UAT_TARGET_H
#define UMICOM_TESTING_UAT_TARGET_H

#include "umicom/testing/uat_workflow.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UAT_TARGET_PROTOCOL 1U
#define UMI_UAT_TARGET_NAME_CAPACITY 96U

typedef enum UmiUatTargetPlatform {
    UMI_UAT_DESKTOP = 1,
    UMI_UAT_MOBILE = 2,
    UMI_UAT_WEB = 3,
    UMI_UAT_EMBEDDED = 4
} UmiUatTargetPlatform;

typedef enum UmiUatOperation {
    UMI_UAT_UI_OBSERVE = 1U << 0,
    UMI_UAT_UI_INTERACT = 1U << 1,
    UMI_UAT_WORKFLOW = 1U << 2,
    UMI_UAT_SECURITY_ASSERT = 1U << 3,
    UMI_UAT_DATABASE_READ = 1U << 4,
    UMI_UAT_DATABASE_WRITE = 1U << 5,
    UMI_UAT_UNIT = 1U << 6
} UmiUatOperation;

/** A trusted caller assigns each identifier when preparing the fixture. IDs
 * are case-sensitive, contain letters, digits, dots, dashes or underscores,
 * and start with a letter or digit. Protocol changes require explicit support.
 */
typedef struct UmiUatTarget {
    uint32_t protocol;
    UmiUatTargetPlatform platform;
    char targetId[UMI_UAT_TARGET_NAME_CAPACITY];
    char sessionId[UMI_UAT_TARGET_NAME_CAPACITY];
} UmiUatTarget;

/** target and test are borrowed during the call. Cancellation, timeout and
 * result obligations match UmiUatExecuteFn. The trusted adapter owns access to
 * its actual fixture, including OS, browser, database and device permissions.
 */
typedef UmiStatus (*UmiUatDriverExecuteFn)(void *context, const UmiUatTarget *target,
    UmiUatOperation operation, const UmiTestCase *test, UmiCancellationToken *cancellation,
    UmiTestResult *result);

typedef struct UmiUatDriver {
    UmiUatTarget binding;
    uint32_t supportedOperations;
    UmiUatDriverExecuteFn execute;
    void *context;
} UmiUatDriver;

/** Identity and operation masks captured at admission. Use fixture labels for
 * targetId and sessionId because this record is included in scoped JSON. */
typedef struct UmiUatTargetEvidence {
    UmiUatTarget target;
    uint32_t allowedOperations;
    uint32_t supportedOperations;
} UmiUatTargetEvidence;

/** Match the exact target, session, platform and protocol before any callback.
 * allowedOperations is the fixture owner's operation mask. Each operations
 * row names one operation for the corresponding workflow row, even if the
 * row is disabled. Every operation must be both allowed and supported.
 *
 * Incorrect arguments return INVALID_ARGUMENT, unsupported revisions or
 * operations return UNAVAILABLE, and out-of-scope operations or identities
 * return PERMISSION_DENIED. A refusal clears *outReport. An admitted run
 * inherits UmiUatRun's result and cleanup rules. Dispatch uses copied inputs.
 * Callers serialize this synchronous operation. The binding is a contract
 * between trusted local components. Enforce external permissions separately.
 */
UmiStatus UmiUatRunTarget(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatDriver *driver, const UmiUatStep *steps, const UmiUatOperation *operations,
    size_t count, UmiCancellationToken *cancellation,
    UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport);

/** Copy the admitted target and operation masks. Returns NOT_FOUND for an
 * unscoped report. On failure a supplied output is cleared. */
UmiStatus UmiUatReportTarget(const UmiUatReport *report, UmiUatTargetEvidence *outEvidence);

/** Read the operation declared for a finding, including skipped work. Returns
 * NOT_FOUND for an unscoped report or an index outside the plan. */
UmiStatus UmiUatReportOperation(const UmiUatReport *report, size_t index, UmiUatOperation *outOperation);

#ifdef __cplusplus
}
#endif
#endif
