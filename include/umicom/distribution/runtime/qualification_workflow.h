/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification_workflow.h
 * Purpose: Coordinate identified qualification phases through explicitly supplied host executors.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_QUALIFICATION_WORKFLOW_H
#define UMICOM_DISTRIBUTION_QUALIFICATION_WORKFLOW_H
#include "umicom/distribution/runtime/qualification.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiQualificationWorkflowStage {
    UMI_QUALIFICATION_WORKFLOW_PREPARED,
    UMI_QUALIFICATION_WORKFLOW_BUILD_RECORDED,
    UMI_QUALIFICATION_WORKFLOW_TESTS_RECORDED,
    UMI_QUALIFICATION_WORKFLOW_CLOSED
} UmiQualificationWorkflowStage;
/** Host executors return OK only when they supplied complete outcome evidence.
 * The result may still be FAILED, SKIPPED or another non-passing outcome.
 * A non-OK callback status means evidence is unavailable and is not recorded.
 * Borrow the immutable plan only during execution. Execute exactly its selection;
 * never substitute an empty test filter or reuse output from an earlier attempt.
 * Host callbacks own trust, tool selection, cancellation, log capture and process
 * termination. Return only after all protected child work has stopped. */
typedef struct UmiQualificationWorkflowHost {
    UmiStatus (*build)(const UmiReleaseImpactPlan *plan, const char *execution,
        UmiReleaseQualificationResult *outResult, void *context);
    /** XML is borrowed through Execute return; host retains and releases storage.
     * This report must belong to this attempt, cover the exact selected names
     * and describe the returned aggregate CTest process result. */
    UmiStatus (*tests)(const UmiReleaseImpactPlan *plan, const char *execution,
        const char **outXml, size_t *outLength,
        UmiReleaseQualificationResult *outProcess, void *context);
    /** Called before execution and while closing. Hash actual reviewed tools,
     * commands and relevant environment; no toolchain identity is guessed. */
    UmiStatus (*toolchain)(char outSha256[65], void *context);
    bool (*cancelled)(void *context);
    /** Synchronous worker callback. Capture an immutable view here, then marshal
     * it to the UI. Never mutate/destroy the borrowed run or re-enter execution. */
    void (*progress)(UmiQualificationWorkflowStage stage, const UmiReleaseQualification *run, void *context);
    void *context;
} UmiQualificationWorkflowHost;
typedef struct UmiQualificationWorkflowRequest {
    const UmiReleaseInventory *inventory;
    const UmiReleaseImpactPlan *plan;
    const char *execution;
    const UmiReleaseSourceOptions *sourceOptions;
    UmiQualificationWorkflowHost host;
    bool guardBuildDirectory;
} UmiQualificationWorkflowRequest;
/** Synchronous worker orchestration: optional cooperative guard, fresh source and
 * toolchain observations, identified ledger creation, build, exact CTest import,
 * after observations, sealing and release. Required callbacks are build, tests
 * and toolchain. This service calls only those host executors, not shell commands.
 *
 * outRun must address NULL. Before ledger creation a failure leaves it NULL.
 * After creation, every return transfers the sealed partial/complete run to the
 * caller for inspection; failed/missing evidence is never discarded or reused.
 * OK means the sealed scoped summary is ready for review. A complete but failing
 * assessment returns INVALID_STATE. Callback, observation and lease failures
 * retain their first status. No tests execute after a failed or invalid build.
 * Cancellation prevents later executor calls and leaves missing evidence visible.
 *
 * All inputs/context remain valid until return. Source options may perform native
 * reads or use an injected provider; providers should use the same cancellation
 * decision. Source observations are not an atomic snapshot. The optional guard
 * uses inventory's existing absolute build root and protects only participants.
 * Hosts must resolve presets to that same root before execution. Nothing is
 * installed, published or automatically retried. A new attempt needs a new id. */
UmiStatus UmiQualificationWorkflowExecute(const UmiQualificationWorkflowRequest *request,
    UmiReleaseQualification **outRun);
#ifdef __cplusplus
}
#endif
#endif
