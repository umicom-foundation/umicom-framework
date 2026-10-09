/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/job_inputs.c
 * PURPOSE: Seal deterministic input identities without retaining source contents.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/data/job_inputs.h"
#include "umicom/base/sha256.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct JobInput
{
    char name[UMI_JOB_INPUT_NAME_CAPACITY];
    char digest[UMI_JOB_IDENTITY_DIGEST_CAPACITY];
} JobInput;
struct UmiJobInputs
{
    size_t count, capacity;
    bool sealed;
    JobInput *entries;
    size_t *name_index;
    size_t index_capacity;
    char digest[UMI_JOB_IDENTITY_DIGEST_CAPACITY];
};
UmiStatus UmiJobInputsCreate(size_t capacity, UmiJobInputs **out_inputs)
{
    if (out_inputs == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_inputs = NULL;
    if (capacity == 0U || capacity > UMI_JOB_INPUT_MAX_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the allocation arithmetic explicit even on smaller future targets. */
    if (capacity > SIZE_MAX / sizeof(JobInput) || capacity > SIZE_MAX / 2U / sizeof(size_t))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJobInputs *inputs = calloc(1U, sizeof(*inputs));
    if (inputs == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    /* A fixed reservation prevents accidental partial collection on growth
     * failure. Callers choose a suitable bound and must handle a full set. */
    inputs->entries = calloc(capacity, sizeof(*inputs->entries));
    if (inputs->entries == NULL)
    {
        free(inputs);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    inputs->index_capacity = capacity * 2U;
    inputs->name_index = calloc(inputs->index_capacity, sizeof(*inputs->name_index));
    if (inputs->name_index == NULL)
    {
        free(inputs->entries);
        free(inputs);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    inputs->capacity = capacity;
    *out_inputs = inputs;
    return UMI_STATUS_OK;
}
void UmiJobInputsDestroy(UmiJobInputs *inputs)
{
    if (inputs == NULL)
        return;
    free(inputs->name_index);
    free(inputs->entries);
    free(inputs);
}
/* This in-memory index avoids scanning every earlier asset for every add.
 * It is not part of the persisted digest. Collisions compare complete names;
 * probing is bounded and the table is never more than half full. Unsigned
 * multiplication wraps intentionally; complete-name comparison decides equality. */
static size_t NameSlot(const UmiJobInputs *inputs, const char *name)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    for (const unsigned char *p = (const unsigned char *)name; *p != 0U; ++p)
    {
        hash ^= *p;
        hash *= UINT64_C(1099511628211);
    }
    return (size_t)(hash % (uint64_t)inputs->index_capacity);
}
static UmiStatus CheckName(UmiJobInputs *inputs, const char *name, size_t *out_slot)
{
    if (inputs == NULL || name == NULL || name[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (inputs->sealed)
        return UMI_STATUS_INVALID_STATE;
    size_t length = 0U;
    while (length < UMI_JOB_INPUT_NAME_CAPACITY && name[length] != '\0')
    {
        unsigned char c = (unsigned char)name[length++];
        if (c < 32U || c == 127U)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length == UMI_JOB_INPUT_NAME_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t slot = NameSlot(inputs, name);
    for (size_t attempt = 0U; attempt < inputs->index_capacity; ++attempt)
    {
        size_t stored = inputs->name_index[slot];
        if (stored == 0U)
        {
            if (inputs->count == inputs->capacity)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            if (out_slot != NULL)
                *out_slot = slot;
            return UMI_STATUS_OK;
        }
        if (strcmp(inputs->entries[stored - 1U].name, name) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
        slot = (slot + 1U) % inputs->index_capacity;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
UmiStatus UmiJobInputsAddDigest(UmiJobInputs *inputs, const char *name, const char digest[65])
{
    size_t slot = 0U;
    UmiStatus status = CheckName(inputs, name, &slot);
    if (status != UMI_STATUS_OK)
        return status;
    if (digest == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The shared identity validator owns digest spelling. Require an actual
     * digest here; empty means unknown in history, not a collected input. */
    UmiJobIdentity candidate = {0};
    size_t length = 0U;
    while (length < sizeof(candidate.subject) && digest[length] != '\0')
        ++length;
    if (length != 64U)
        return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(candidate.subject, digest, length + 1U);
    memcpy(candidate.configuration, digest, length + 1U);
    if (UmiJobIdentityValidate(&candidate) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    JobInput *entry = &inputs->entries[inputs->count];
    memcpy(entry->name, name, strlen(name) + 1U);
    memcpy(entry->digest, digest, length + 1U);
    inputs->name_index[slot] = inputs->count + 1U;
    ++inputs->count;
    return UMI_STATUS_OK;
}
UmiStatus UmiJobInputsAddBytes(UmiJobInputs *inputs, const char *name, const void *bytes,
                               size_t length)
{
    UmiStatus status = CheckName(inputs, name, NULL);
    if (status != UMI_STATUS_OK)
        return status;
    char digest[UMI_JOB_IDENTITY_DIGEST_CAPACITY];
    status = UmiSha256Buffer(bytes, length, digest);
    return status == UMI_STATUS_OK ? UmiJobInputsAddDigest(inputs, name, digest) : status;
}
static int CompareInputs(const void *left, const void *right)
{
    const JobInput *a = left, *b = right;
    return strcmp(a->name, b->name);
}
/* Fixed byte order and length prefixes make names and digests unambiguous on
 * every supported host. The domain label separates this grammar from a raw
 * file digest; preserve its encoding when adding another consumer. */
static UmiStatus AddNumber(UmiSha256 *hash, uint64_t value)
{
    unsigned char bytes[8];
    for (size_t index = 0U; index < sizeof(bytes); ++index)
        bytes[7U - index] = (unsigned char)(value >> (index * 8U));
    return UmiSha256Update(hash, bytes, sizeof(bytes));
}
static UmiStatus AddText(UmiSha256 *hash, const char *text)
{
    size_t length = strlen(text);
    UmiStatus status = AddNumber(hash, (uint64_t)length);
    return status == UMI_STATUS_OK ? UmiSha256Update(hash, text, length) : status;
}
UmiStatus UmiJobInputsSeal(UmiJobInputs *inputs, char out_digest[65])
{
    if (inputs == NULL || out_digest == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!inputs->sealed)
    {
        qsort(inputs->entries, inputs->count, sizeof(*inputs->entries), CompareInputs);
        UmiSha256 hash;
        UmiSha256Init(&hash);
        UmiStatus status = AddText(&hash, "umicom.job.inputs");
        if (status == UMI_STATUS_OK)
            status = AddNumber(&hash, (uint64_t)inputs->count);
        for (size_t index = 0U; status == UMI_STATUS_OK && index < inputs->count; ++index)
        {
            status = AddText(&hash, inputs->entries[index].name);
            if (status == UMI_STATUS_OK)
                status = AddText(&hash, inputs->entries[index].digest);
        }
        unsigned char bytes[UMI_SHA256_BYTES];
        if (status == UMI_STATUS_OK)
            status = UmiSha256Final(&hash, bytes);
        if (status != UMI_STATUS_OK)
            return status;
        UmiSha256Hex(bytes, inputs->digest);
        inputs->sealed = true;
    }
    memcpy(out_digest, inputs->digest, sizeof(inputs->digest));
    return UMI_STATUS_OK;
}
