/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/selection_identity.c
 * PURPOSE: Encode selection fields explicitly so padding and transient IDs cannot change evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "selection_identity_internal.h"
#include <string.h>

/* Fixed big-endian integers and length-prefixed text are independent of CPU
 * byte order and cannot confuse adjacent fields containing punctuation. */
static void selection_number(UmiTestSelectionHasher *hasher, uint64_t value)
{
    unsigned char bytes[8];
    for (size_t i = 0; i < sizeof(bytes); ++i)
        bytes[sizeof(bytes) - 1U - i] = (unsigned char)(value >> (i * 8U));
    if (hasher->status == UMI_STATUS_OK)
        hasher->status = UmiSha256Update(&hasher->hash, bytes, sizeof(bytes));
}
static void selection_text(UmiTestSelectionHasher *hasher, const char *value, size_t capacity,
                           bool required)
{
    if (hasher->status != UMI_STATUS_OK)
        return;
    const char *end = memchr(value, '\0', capacity);
    if (end == NULL || (required && end == value))
    {
        hasher->status = UMI_STATUS_INVALID_ARGUMENT;
        return;
    }
    size_t length = (size_t)(end - value);
    selection_number(hasher, (uint64_t)length);
    if (hasher->status == UMI_STATUS_OK)
        hasher->status = UmiSha256Update(&hasher->hash, value, length);
}
void UmiTestSelectionBegin(UmiTestSelectionHasher *hasher, const UmiCtestJobPlanSnapshot *plan)
{
    memset(hasher, 0, sizeof(*hasher));
    hasher->status = UMI_STATUS_INVALID_ARGUMENT;
    if (plan == NULL || plan->request_count == 0 ||
        plan->request_count > UMI_CTEST_JOB_MAX_ATTEMPTS || plan->repeat_count == 0 ||
        plan->repeat_count > UMI_CTEST_JOB_MAX_ATTEMPTS / plan->request_count)
        return;
    hasher->expected = plan->request_count;
    UmiSha256Init(&hasher->hash);
    hasher->status = UMI_STATUS_OK;
    selection_text(hasher, "umicom.test.selection", sizeof("umicom.test.selection"), true);
    selection_number(hasher, (uint64_t)plan->request_count);
    selection_number(hasher, plan->repeat_count);
    selection_number(hasher, plan->stop_on_failure ? 1U : 0U);
}
void UmiTestSelectionAdd(UmiTestSelectionHasher *hasher, const UmiCtestJobRequest *request)
{
    if (hasher->status != UMI_STATUS_OK)
        return;
    if (request == NULL || hasher->added >= hasher->expected ||
        (request->enabled != 0 && request->enabled != 1) || request->test_id[0] == '\0' ||
        memchr(request->test_id, '\0', sizeof(request->test_id)) == NULL)
    {
        hasher->status = UMI_STATUS_INVALID_ARGUMENT;
        return;
    }
    selection_text(hasher, request->name, sizeof(request->name), true);
    selection_text(hasher, request->build_directory, sizeof(request->build_directory), true);
    selection_text(hasher, request->configuration, sizeof(request->configuration), false);
    selection_number(hasher, request->timeout_ms);
    selection_number(hasher, (uint64_t)request->enabled);
    ++hasher->added;
}
UmiStatus UmiTestSelectionFinish(UmiTestSelectionHasher *hasher,
                                 char out_digest[UMI_SHA256_HEX_CAPACITY])
{
    if (out_digest == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (hasher->status != UMI_STATUS_OK)
        return hasher->status;
    if (hasher->added != hasher->expected)
        return UMI_STATUS_INVALID_STATE;
    unsigned char bytes[UMI_SHA256_BYTES];
    UmiStatus status = UmiSha256Final(&hasher->hash, bytes);
    if (status == UMI_STATUS_OK)
        UmiSha256Hex(bytes, out_digest);
    return status;
}
UmiStatus UmiTestSelectionDigest(const UmiCtestJobPlanSnapshot *plan,
                                 const UmiCtestJobRequest *requests,
                                 char out_digest[UMI_SHA256_HEX_CAPACITY])
{
    if (requests == NULL || out_digest == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestSelectionHasher hasher;
    UmiTestSelectionBegin(&hasher, plan);
    for (size_t i = 0; hasher.status == UMI_STATUS_OK && i < hasher.expected; ++i)
        UmiTestSelectionAdd(&hasher, &requests[i]);
    return UmiTestSelectionFinish(&hasher, out_digest);
}
