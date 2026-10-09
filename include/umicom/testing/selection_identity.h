/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/selection_identity.h
 * PURPOSE: Identify the exact ordered test selection without launching a process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_SELECTION_IDENTITY_H
#define UMICOM_TESTING_SELECTION_IDENTITY_H
#include "umicom/base/sha256.h"
#include "umicom/testing/ctest_capture.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Hash the plan and its ordered requests into lowercase SHA-256 text.
     * Names, build directories, configurations, enabled flags, timeouts, repeats
     * and early-stop policy are included. Discovery IDs are deliberately excluded:
     * they can change when the same tests are rediscovered.
     * Paths are compared as supplied; no filesystem aliases are resolved. The
     * digest describes a selection, not source bytes, test binaries or outcomes.
     * requests contains exactly plan->request_count entries. Nothing is borrowed
     * after return. Errors preserve out_digest; it must not overlap the inputs. */
    UmiStatus UmiTestSelectionDigest(const UmiCtestJobPlanSnapshot *plan,
                                     const UmiCtestJobRequest *requests,
                                     char out_digest[UMI_SHA256_HEX_CAPACITY]);
#ifdef __cplusplus
}
#endif
#endif
