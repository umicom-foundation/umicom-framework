/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/job_inputs.h
 * PURPOSE: Build bounded content identities from an explicitly supplied input set.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DATA_JOB_INPUTS_H
#define UMICOM_DATA_JOB_INPUTS_H
#include "umicom/data/job_identity.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_JOB_INPUT_NAME_CAPACITY 257U
#define UMI_JOB_INPUT_MAX_CAPACITY 65536U
    typedef struct UmiJobInputs UmiJobInputs;
    /** Allocate a caller-sized, single-owner builder for 1..MAX_CAPACITY inputs.
     * out_inputs is cleared on failure. No file, directory or network is accessed.
     * Only logical names and content digests are retained; the host owns actual
     * input buffers and must execute against the same immutable bytes it records. */
    UmiStatus UmiJobInputsCreate(size_t capacity, UmiJobInputs **out_inputs);
    /** Release all owned input names/digests. NULL is allowed. Never share an active
     * builder between threads without host synchronisation. */
    void UmiJobInputsDestroy(UmiJobInputs *inputs);
    /** Add a named buffer without retaining its bytes. NULL bytes is valid only
     * when length is zero. Names are unique, nonempty bounded byte strings with
     * no ASCII control bytes. Names are case-sensitive logical IDs, not interpreted
     * filesystem paths; Unicode normalisation and dependency discovery belong to
     * the host. Duplicate names return ALREADY_EXISTS even for identical content.
     * Failure changes no entries. Adding after sealing returns INVALID_STATE. */
    UmiStatus UmiJobInputsAddBytes(UmiJobInputs *inputs, const char *name, const void *bytes,
                                   size_t length);
    /** Add a precomputed lowercase SHA-256 digest for a large or streamed input.
     * This trusts the caller's computation; it does not read or authenticate a file.
     * Names and digests must terminate within their declared capacities. */
    UmiStatus UmiJobInputsAddDigest(UmiJobInputs *inputs, const char *name,
                                    const char digest[UMI_JOB_IDENTITY_DIGEST_CAPACITY]);
    /** Sort by logical name, hash an unambiguous encoding, and seal the set.
     * Insertion order does not affect the result; names and content both do.
     * An explicitly empty set has a nonempty digest, distinct from unrecorded
     * evidence. Sealing again returns the same digest; no more inputs may be added.
     * Output is unchanged on failure and must have DIGEST_CAPACITY writable bytes. */
    UmiStatus UmiJobInputsSeal(UmiJobInputs *inputs,
                               char out_digest[UMI_JOB_IDENTITY_DIGEST_CAPACITY]);
#ifdef __cplusplus
}
#endif
#endif
