/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/snapshot_contract_cases.h
 * PURPOSE: Reuse behavioural checks across explicitly typed registry consumers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_SNAPSHOT_CONTRACT_CASES_H
#define UMICOM_TEST_SNAPSHOT_CONTRACT_CASES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)
#define FIELD_COUNT (sizeof(contract_fields) / sizeof(contract_fields[0]))

#ifdef UMI_TEST_SNAPSHOT_ALLOCATION
static int fail_next_allocation;
void *__real_malloc(size_t size);
void *__wrap_malloc(size_t size)
{
    if (fail_next_allocation) { fail_next_allocation = 0; return NULL; }
    return __real_malloc(size);
}
#endif

static CONTRACT_SNAPSHOT Record(const char *id)
{
    CONTRACT_SNAPSHOT item = {0};
    ContractPayload(&item);
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    return item;
}

static int Unchanged(CONTRACT_REGISTRY *registry, size_t count, uint64_t revision,
    const CONTRACT_SNAPSHOT *before)
{
    CONTRACT_SNAPSHOT after;
    CHECK(CONTRACT_COUNT(registry) == count && CONTRACT_REVISION(registry) == revision);
    if (before != NULL) {
        CHECK(CONTRACT_FIND(registry, before->id, &after) == UMI_STATUS_OK);
        CHECK(ContractSnapshotEqual(before, &after));
    }
    return 0;
}

/* Domains may extend public snapshots and normalise legacy scalar flags.
 * Exact payload comparisons remain in place after explicit canonicalisation. */
#ifndef CONTRACT_API_VERSION
#define CONTRACT_API_VERSION 1U
#endif
#ifndef CONTRACT_NORMALISE
#define CONTRACT_NORMALISE(item) ((void)(item))
#endif
static int Lifecycle(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT input = Record("alpha"), output, second;
    UmiSnapshotValidation detail = {UMI_SNAPSHOT_INVALID_SCHEMA, "previous", 9U, 9U};
    CHECK(CONTRACT_VALIDATE(&input, &detail) == UMI_STATUS_OK);
    CHECK(detail.issue == UMI_SNAPSHOT_VALID && detail.field == NULL && detail.field_index == SIZE_MAX);
    uint64_t revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_OK);
    CHECK(CONTRACT_COUNT(registry) == 1U && CONTRACT_REVISION(registry) == revision + 1U);
    CHECK(CONTRACT_FIND(registry, "alpha", &output) == UMI_STATUS_OK);
/* Use the API version declared by the domain snapshot. The previous implementation remains for engineering review. */
#if 0
    CHECK(output.struct_size == sizeof(output) && output.api_version == 1U && output.revision == revision + 1U);
#endif
    CHECK(output.struct_size == sizeof(output) && output.api_version == CONTRACT_API_VERSION && output.revision == revision + 1U);
    CONTRACT_SNAPSHOT expected = input;
    expected.struct_size = (uint32_t)sizeof(expected);
/* Expect the domain canonical record while checking every field. The previous implementation remains for engineering review. */
#if 0
    expected.api_version = 1U;
#endif
    expected.api_version = CONTRACT_API_VERSION;
    CONTRACT_NORMALISE(&expected);
    expected.revision = revision + 1U;
    CHECK(ContractSnapshotEqual(&expected, &output));
    /* The registry owns a copy, including text; changing input is not an edit. */
    input.id[0] = 'x';
    CHECK(CONTRACT_FIND(registry, "alpha", &second) == UMI_STATUS_OK && ContractSnapshotEqual(&output, &second));
    input = Record("alpha");
    input.struct_size = 7U; input.api_version = 42U;
    CHECK(CONTRACT_VALIDATE(&input, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_OK);
    CHECK(CONTRACT_COUNT(registry) == 1U && CONTRACT_REVISION(registry) == revision + 2U);
    CHECK(CONTRACT_AT(registry, 0U, &output) == UMI_STATUS_OK);
/* Use the API version declared by the domain snapshot. The previous implementation remains for engineering review. */
#if 0
    CHECK(output.struct_size == sizeof(output) && output.api_version == 1U);
#endif
    CHECK(output.struct_size == sizeof(output) && output.api_version == CONTRACT_API_VERSION);
    CHECK(CONTRACT_REMOVE(registry, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(CONTRACT_REVISION(registry) == revision + 2U);
    CHECK(CONTRACT_REMOVE(registry, "alpha") == UMI_STATUS_OK && CONTRACT_COUNT(registry) == 0U);
    return 0;
}

static int Fields(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT input = Record("alpha"), before;
    CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &before) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    for (size_t field = 0U; field < FIELD_COUNT; ++field) {
        input = Record("alpha");
        unsigned char *bytes = (unsigned char *)&input;
        memset(bytes + contract_fields[field].offset, 'x', contract_fields[field].capacity);
        UmiSnapshotValidation detail;
        CHECK(CONTRACT_VALIDATE(&input, &detail) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(detail.issue == UMI_SNAPSHOT_UNTERMINATED_TEXT && detail.field_index == field);
        CHECK(detail.capacity == contract_fields[field].capacity && strcmp(detail.field, contract_fields[field].name) == 0);
        CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    }
    return 0;
}

static int Boundaries(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT input = Record("alpha"), output;
    for (size_t field = 0U; field < FIELD_COUNT; ++field) {
        input = Record("alpha");
        char *text = (char *)&input + contract_fields[field].offset;
        size_t capacity = contract_fields[field].capacity;
        memset(text, 'z', capacity - 1U); text[capacity - 1U] = '\0';
        CHECK(CONTRACT_VALIDATE(&input, NULL) == UMI_STATUS_OK);
        CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_OK);
        CHECK(CONTRACT_FIND(registry, input.id, &output) == UMI_STATUS_OK);
        CHECK(memcmp((char *)&output + contract_fields[field].offset, text, capacity) == 0);
        CHECK(CONTRACT_REMOVE(registry, input.id) == UMI_STATUS_OK);
    }
    input = Record("caf\xc3\xa9");
    /* A terminated field is valid even when unused trailing bytes are nonzero. */
    input.id[sizeof(input.id) - 1U] = 'q';
    CHECK(CONTRACT_VALIDATE(&input, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "caf\xc3\xa9", &output) == UMI_STATUS_OK);
    return 0;
}

static int Invalid(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT empty = {0};
    UmiSnapshotValidation detail;
    CHECK(CONTRACT_VALIDATE(NULL, &detail) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(detail.issue == UMI_SNAPSHOT_NULL_RECORD && detail.field_index == SIZE_MAX);
    CHECK(CONTRACT_UPSERT(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_VALIDATE(&empty, &detail) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(detail.issue == UMI_SNAPSHOT_EMPTY_REQUIRED_TEXT && strcmp(detail.field, "id") == 0);
    CHECK(CONTRACT_UPSERT(registry, &empty) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_COUNT(registry) == 0U);
    return 0;
}

static int BatchSuccess(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT items[2] = {Record("alpha"), Record("beta")};
    /* Replace a real payload value, not only its revision metadata. */
    /* Contracts with only an ID text field must change a scalar payload:
     * changing the key would test insertion rather than replacement. */
#ifdef CONTRACT_CHANGE_PAYLOAD
    CONTRACT_CHANGE_PAYLOAD(&items[0]);
#else
    *((char *)&items[0] + contract_fields[FIELD_COUNT - 1U].offset) = 'Q';
#endif
    CONTRACT_SNAPSHOT untouched[2] = {items[0], items[1]}, output;
    CONTRACT_SNAPSHOT existing = Record("alpha"), middle = Record("middle");
    CHECK(CONTRACT_UPSERT(registry, &existing) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(registry, &middle) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_BATCH(registry, items, 2U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 2U && result.rejected_index == SIZE_MAX && result.validation.issue == UMI_SNAPSHOT_VALID);
    CHECK(CONTRACT_COUNT(registry) == 3U && CONTRACT_REVISION(registry) == revision + 2U);
    CHECK(CONTRACT_AT(registry, 0U, &output) == UMI_STATUS_OK && strcmp(output.id, "alpha") == 0 && output.revision == revision + 1U);
    CHECK(CONTRACT_AT(registry, 1U, &output) == UMI_STATUS_OK && strcmp(output.id, "middle") == 0);
    CHECK(CONTRACT_AT(registry, 2U, &output) == UMI_STATUS_OK && strcmp(output.id, "beta") == 0 && output.revision == revision + 2U);
    for (size_t index = 0U; index < 2U; ++index) {
        CONTRACT_SNAPSHOT expected = items[index];
        expected.struct_size = (uint32_t)sizeof(expected);
    /* Expect the domain canonical record while checking every field. The previous implementation remains for engineering review. */
#if 0
        expected.api_version = 1U;
#endif
        expected.api_version = CONTRACT_API_VERSION;
        CONTRACT_NORMALISE(&expected);
        expected.revision = revision + index + 1U;
        CHECK(CONTRACT_FIND(registry, expected.id, &output) == UMI_STATUS_OK);
        CHECK(ContractSnapshotEqual(&expected, &output));
    }
    CHECK(ContractSnapshotEqual(&items[0], &untouched[0]) && ContractSnapshotEqual(&items[1], &untouched[1]));
    return 0;
}

static int BatchInvalid(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT input = Record("alpha"), before;
    CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &before) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    CONTRACT_SNAPSHOT items[2] = {Record("alpha"), Record("beta")};
    const size_t field = FIELD_COUNT - 1U;
    memset((char *)&items[1] + contract_fields[field].offset, 'x', contract_fields[field].capacity);
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_BATCH(registry, items, 2U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result.applied == 0U && result.rejected_index == 1U && result.validation.field_index == field);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    items[1] = Record("beta");
    CHECK(CONTRACT_BATCH(registry, items, 2U, NULL) == UMI_STATUS_OK && CONTRACT_COUNT(registry) == 2U);
    return 0;
}

static int BatchDuplicate(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT items[2] = {Record("same"), Record("same")};
    uint64_t revision = CONTRACT_REVISION(registry);
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_BATCH(registry, items, 2U, &result) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(result.applied == 0U && result.rejected_index == 1U && result.validation.issue == UMI_SNAPSHOT_DUPLICATE_ID);
    CHECK(strcmp(result.validation.field, "id") == 0);
    CHECK(Unchanged(registry, 0U, revision, NULL) == 0);
    CHECK(CONTRACT_BATCH(registry, items, 1U, &result) == UMI_STATUS_OK);
    CHECK(CONTRACT_BATCH(registry, items, 1U, &result) == UMI_STATUS_OK && CONTRACT_COUNT(registry) == 1U);
    return 0;
}

static int BatchCapacity(CONTRACT_REGISTRY *registry)
{
    for (size_t index = 0U; index + 1U < CONTRACT_CAPACITY; ++index) {
        CONTRACT_SNAPSHOT item = {0};
        (void)snprintf(item.id, sizeof(item.id), "entry.%zu", index);
        CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    }
    CONTRACT_SNAPSHOT before;
    CHECK(CONTRACT_FIND(registry, "entry.0", &before) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    CONTRACT_SNAPSHOT items[3] = {Record("entry.0"), Record("last.slot"), Record("overflow")};
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_BATCH(registry, items, 3U, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result.applied == 0U && result.rejected_index == 2U);
    CHECK(Unchanged(registry, CONTRACT_CAPACITY - 1U, revision, &before) == 0);
    CHECK(CONTRACT_BATCH(registry, &items[1], 1U, &result) == UMI_STATUS_OK && CONTRACT_COUNT(registry) == CONTRACT_CAPACITY);
    CHECK(CONTRACT_BATCH(registry, &items[0], 1U, &result) == UMI_STATUS_OK && result.applied == 1U);
    CHECK(CONTRACT_UPSERT(registry, &items[2]) == UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}

static int BatchEmpty(CONTRACT_REGISTRY *registry)
{
    UmiSnapshotBatchResult result;
    CONTRACT_SNAPSHOT input = Record("alpha");
    uint64_t revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_BATCH(registry, NULL, 0U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(CONTRACT_BATCH(NULL, NULL, 0U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_BATCH(registry, NULL, 1U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_BATCH(registry, &input, (size_t)CONTRACT_CAPACITY + 1U, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(CONTRACT_BATCH(registry, &input, SIZE_MAX, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(Unchanged(registry, 0U, revision, NULL) == 0);
    return 0;
}

#ifdef UMI_TEST_SNAPSHOT_ALLOCATION
static int Allocation(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT item = Record("alpha"), before;
    CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &before) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    CONTRACT_SNAPSHOT items[2] = {Record("alpha"), Record("beta")};
    UmiSnapshotBatchResult result;
    fail_next_allocation = 1;
    CHECK(CONTRACT_BATCH(registry, items, 2U, &result) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(!fail_next_allocation && result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    CHECK(CONTRACT_BATCH(registry, items, 2U, &result) == UMI_STATUS_OK && result.applied == 2U);
    return 0;
}
#endif

#ifdef CONTRACT_REPLACE_DOCUMENT
#include "snapshot_scope_cases.h"
#endif

#include "snapshot_transfer_cases.h"

static int Run(CONTRACT_REGISTRY *registry, const char *name)
{
    if (strcmp(name, "capture") == 0) return TransferCapture(registry);
    if (strcmp(name, "replace") == 0) return TransferReplace(registry);
    if (strcmp(name, "replace-reject") == 0) return TransferReject(registry);
    if (strcmp(name, "replace-empty") == 0) return TransferEmpty(registry);
#ifdef UMI_TEST_SNAPSHOT_ALLOCATION
    if (strcmp(name, "replace-allocation") == 0) return TransferAllocation(registry);
#endif
#ifdef CONTRACT_REPLACE_DOCUMENT
#ifdef UMI_TEST_SNAPSHOT_ALLOCATION
    if (strcmp(name, "scope-allocation") == 0) return ScopeAllocation(registry);
#endif
    if (strcmp(name, "scope") == 0) return ScopeReplace(registry);
    if (strcmp(name, "scope-empty") == 0) return ScopeEmpty(registry);
    if (strcmp(name, "scope-reject") == 0) return ScopeReject(registry);
    if (strcmp(name, "scope-capacity") == 0) return ScopeCapacity(registry);
    if (strcmp(name, "scope-stale") == 0) return ScopeStale(registry);
    if (strcmp(name, "scope-alias") == 0) return ScopeAlias(registry);
#endif
    if (strcmp(name, "lifecycle") == 0) return Lifecycle(registry);
    if (strcmp(name, "fields") == 0) return Fields(registry);
    if (strcmp(name, "boundaries") == 0) return Boundaries(registry);
    if (strcmp(name, "invalid") == 0) return Invalid(registry);
    if (strcmp(name, "batch") == 0) return BatchSuccess(registry);
    if (strcmp(name, "batch-invalid") == 0) return BatchInvalid(registry);
    if (strcmp(name, "batch-duplicate") == 0) return BatchDuplicate(registry);
    if (strcmp(name, "batch-capacity") == 0) return BatchCapacity(registry);
    if (strcmp(name, "batch-empty") == 0) return BatchEmpty(registry);
#ifdef UMI_TEST_SNAPSHOT_ALLOCATION
    if (strcmp(name, "allocation") == 0) return Allocation(registry);
#endif
    return 2;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    CONTRACT_REGISTRY *registry = NULL;
    CHECK(CONTRACT_CREATE(&registry) == UMI_STATUS_OK);
    int result = Run(registry, argv[1]);
    CONTRACT_DESTROY(registry);
    return result;
}
#endif
