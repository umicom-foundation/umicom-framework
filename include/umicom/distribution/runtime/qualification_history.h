/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification_history.h
 * Purpose: Persist scoped qualification progress through the shared job-history service.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_HISTORY_H
#define UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_HISTORY_H
#include "umicom/distribution/runtime/qualification.h"
#include "umicom/data/job_history.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Map execution, plan and observed-source fingerprints to the history identity.
 * Subject identifies this attempt, configuration identifies the plan, and inputs
 * identifies its source observation. This scheme is distinct from build-profile
 * history. Equal digests do not imply successful execution or authorize replay. */
UmiStatus UmiReleaseQualificationJobIdentity(const UmiReleaseQualification *run,
    UmiJobIdentity *outIdentity);
/** Create the history entry before executing external work. Only a pristine open
 * run can begin. Three steps represent recorded build, complete test accounting,
 * and sealed observations; they are not a count of individual tests.
 * The host owns the history and run. label must be short and non-sensitive.
 * No logs, paths, test names, commands or credentials are persisted in this entry.
 * The host must retain detailed results separately in its private evidence store. */
UmiStatus UmiReleaseQualificationHistoryBegin(const UmiReleaseQualification *run,
    UmiJobHistory *history, const char *label, UmiJobHistoryEntry *outEntry);
/** Publish the current summary with optimistic revision checks. Call once
 * immediately after Begin to enter RUNNING, before external execution.
 * Persisted identity, kind and step count are checked before updating. A stale
 * revision or mismatched entry is rejected without replacing evidence.
 * Closed ready runs become SUCCEEDED. Interrupted cancellations become CANCELLED;
 * other closed non-ready runs become FAILED, retaining a recorded execution status
 * or INVALID_STATE when the assessment alone blocks review. Detailed outcome
 * categories remain on the run.
 * Success from PREPARED is refused rather than inventing a recorded start.
 * Repeating an unchanged state with its current revision returns it unchanged.
 * Storage failure does not roll back the run or launch/replay any work. */
UmiStatus UmiReleaseQualificationHistoryPublish(const UmiReleaseQualification *run,
    UmiJobHistory *history, uint64_t entryId, uint64_t expectedRevision,
    UmiJobHistoryEntry *outEntry);
#ifdef __cplusplus
}
#endif
#endif
