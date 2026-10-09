/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/selection_identity_internal.h
 * PURPOSE: Share ordered selection hashing between archive writes and verified reads.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_SELECTION_IDENTITY_INTERNAL_H
#define UMICOM_TESTING_SELECTION_IDENTITY_INTERNAL_H
#include "umicom/testing/selection_identity.h"
/* Each caller owns its accumulator. Incremental use avoids retaining another
 * complete catalogue while the archive already has bounded request storage. */
typedef struct UmiTestSelectionHasher
{
    UmiSha256 hash;
    size_t expected;
    size_t added;
    UmiStatus status;
} UmiTestSelectionHasher;
void UmiTestSelectionBegin(UmiTestSelectionHasher *hasher, const UmiCtestJobPlanSnapshot *plan);
void UmiTestSelectionAdd(UmiTestSelectionHasher *hasher, const UmiCtestJobRequest *request);
UmiStatus UmiTestSelectionFinish(UmiTestSelectionHasher *hasher,
                                 char out_digest[UMI_SHA256_HEX_CAPACITY]);
#endif
