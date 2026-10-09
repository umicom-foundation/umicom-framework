/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/job_identity.h
 * PURPOSE: Distinguish a job outcome from evidence about the inputs it used.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DATA_JOB_IDENTITY_H
#define UMICOM_DATA_JOB_IDENTITY_H
#include "umicom/base/status.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_JOB_IDENTITY_DIGEST_CAPACITY 65U
    /** Content digests supplied by the caller, never passwords or raw commands.
     * Subject identifies the project or resource. Configuration identifies the
     * reviewed settings. Inputs is optional evidence about a caller-defined input
     * set. Empty inputs means unrecorded, not an empty or unchanged source tree.
     * Hosts must use the same canonical input scheme before comparing records. */
    typedef struct UmiJobIdentity
    {
        char subject[UMI_JOB_IDENTITY_DIGEST_CAPACITY];
        char configuration[UMI_JOB_IDENTITY_DIGEST_CAPACITY];
        char inputs[UMI_JOB_IDENTITY_DIGEST_CAPACITY];
    } UmiJobIdentity;
    typedef enum UmiJobIdentityComparison
    {
        UMI_JOB_IDENTITY_UNKNOWN = 0,
        UMI_JOB_IDENTITY_DIFFERENT_SUBJECT,
        UMI_JOB_IDENTITY_DIFFERENT_CONFIGURATION,
        UMI_JOB_IDENTITY_INPUTS_UNRECORDED,
        UMI_JOB_IDENTITY_DIFFERENT_INPUTS,
        UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS
    } UmiJobIdentityComparison;
    /** Accept either all-empty legacy evidence or lowercase SHA-256 subject and
     * configuration with an optional SHA-256 inputs digest. Bounded fields must
     * terminate. This validates syntax, not the truth of the supplied evidence. */
    UmiStatus UmiJobIdentityValidate(const UmiJobIdentity *identity);
    /** True only for a valid, nonempty subject/configuration pair. */
    bool UmiJobIdentityIsRecorded(const UmiJobIdentity *identity);
    /** Compare without interpreting job success. Missing or invalid identities
     * produce UNKNOWN. Matching settings without both input digests never establish
     * source equivalence. Neither a match nor a success authorises replay. */
    UmiJobIdentityComparison UmiJobIdentityCompare(const UmiJobIdentity *recorded,
                                                   const UmiJobIdentity *current);
    /** Return a static explanation suitable for a read-only history view. */
    const char *UmiJobIdentityComparisonText(UmiJobIdentityComparison comparison);
#ifdef __cplusplus
}
#endif
#endif
