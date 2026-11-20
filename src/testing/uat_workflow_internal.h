/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/uat_workflow_internal.h
 *
 * PURPOSE:
 *   Bind admitted target metadata to the workflow owner before dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_UAT_WORKFLOW_INTERNAL_H
#define UMICOM_TESTING_UAT_WORKFLOW_INTERNAL_H

#include "umicom/testing/uat_target.h"

/* Target admission supplies a validated scope and one operation per step.
 * Legacy workflows pass null for both inputs. */
UmiStatus UmiUatRunWithEvidence(const UmiUatStep *steps, size_t count, UmiUatExecuteFn execute,
    void *context, UmiCancellationToken *cancellation, UmiCancellationToken *cleanupCancellation,
    const UmiUatTargetEvidence *scope, const UmiUatOperation *operations, UmiUatReport **outReport);

#endif
