/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/job_identity.c
 * PURPOSE: Compare bounded job identities without turning missing evidence into success.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/data/job_identity.h"
#include <stddef.h>
#include <string.h>

/* Fixed-width digests keep private paths and command arguments out of history.
 * Hashes are not encryption; callers must still exclude credentials. */
static bool DigestValid(const char text[UMI_JOB_IDENTITY_DIGEST_CAPACITY])
{
    if (text[0] == '\0')
        return true;
    for (size_t index = 0U; index < 64U; ++index)
    {
        char c = text[index];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
            return false;
    }
    return text[64] == '\0';
}
UmiStatus UmiJobIdentityValidate(const UmiJobIdentity *identity)
{
    if (identity == NULL || !DigestValid(identity->subject) ||
        !DigestValid(identity->configuration) || !DigestValid(identity->inputs))
        return UMI_STATUS_INVALID_ARGUMENT;
    bool subject = identity->subject[0] != '\0';
    bool configuration = identity->configuration[0] != '\0';
    if (subject != configuration || (!subject && identity->inputs[0] != '\0'))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
bool UmiJobIdentityIsRecorded(const UmiJobIdentity *identity)
{
    return UmiJobIdentityValidate(identity) == UMI_STATUS_OK && identity->subject[0] != '\0';
}
UmiJobIdentityComparison UmiJobIdentityCompare(const UmiJobIdentity *recorded,
                                               const UmiJobIdentity *current)
{
    if (!UmiJobIdentityIsRecorded(recorded) || !UmiJobIdentityIsRecorded(current))
        return UMI_JOB_IDENTITY_UNKNOWN;
    if (strcmp(recorded->subject, current->subject) != 0)
        return UMI_JOB_IDENTITY_DIFFERENT_SUBJECT;
    if (strcmp(recorded->configuration, current->configuration) != 0)
        return UMI_JOB_IDENTITY_DIFFERENT_CONFIGURATION;
    if (recorded->inputs[0] == '\0' || current->inputs[0] == '\0')
        return UMI_JOB_IDENTITY_INPUTS_UNRECORDED;
    if (strcmp(recorded->inputs, current->inputs) != 0)
        return UMI_JOB_IDENTITY_DIFFERENT_INPUTS;
    return UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS;
}
const char *UmiJobIdentityComparisonText(UmiJobIdentityComparison comparison)
{
    switch (comparison)
    {
    case UMI_JOB_IDENTITY_DIFFERENT_SUBJECT:
        return "Different project or resource";
    case UMI_JOB_IDENTITY_DIFFERENT_CONFIGURATION:
        return "Different settings";
    case UMI_JOB_IDENTITY_INPUTS_UNRECORDED:
        return "Same settings; input contents not recorded";
    case UMI_JOB_IDENTITY_DIFFERENT_INPUTS:
        return "Same settings; different recorded inputs";
    case UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS:
        return "Same settings and recorded input set";
    default:
        return "Identity not recorded or unavailable";
    }
}
