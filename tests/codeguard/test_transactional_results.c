/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_transactional_results.c
 *
 * PURPOSE:
 *   Exercise real CodeGuard service and result ownership with deterministic scan, allocation and relocation faults.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/codeguard/service.h"

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)

typedef struct Allocation { void *pointer; size_t bytes; } Allocation;
static Allocation allocations[32];
static void *quarantine[16];
static size_t quarantine_count, allocation_count, allocation_fail_at, reallocation_bytes;
static int fail_reallocate;

/* Force growth to move. Quarantine the poisoned previous array until the test
 * ends so a stale read is reproducible without allocator reuse assumptions. */
static size_t AllocationSlot(void *pointer)
{
    for (size_t i = 0U; i < 32U; ++i)
        if (allocations[i].pointer == pointer) return i;
    abort();
}
static void *TestCalloc(size_t count, size_t bytes)
{
    ++allocation_count;
    if (allocation_count == allocation_fail_at) return NULL;
    if (bytes != 0U && count > SIZE_MAX / bytes) return NULL;
    void *pointer = calloc(count, bytes);
    if (pointer != NULL) allocations[AllocationSlot(NULL)] =
        (Allocation){pointer, count * bytes};
    return pointer;
}
static void TestFree(void *pointer)
{
    if (pointer == NULL) return;
    allocations[AllocationSlot(pointer)] = (Allocation){0};
    free(pointer);
}
static void *TestRealloc(void *pointer, size_t bytes)
{
    reallocation_bytes = bytes;
    if (fail_reallocate) return NULL;
    size_t slot = AllocationSlot(pointer);
    void *next = malloc(bytes);
    if (next == NULL) return NULL;
    size_t copy = allocations[slot].bytes < bytes ? allocations[slot].bytes : bytes;
    memcpy(next, pointer, copy);
    memset(pointer, 0xa5, allocations[slot].bytes);
    if (quarantine_count == 16U) abort();
    quarantine[quarantine_count++] = pointer;
    allocations[slot] = (Allocation){next, bytes};
    return next;
}

/* Compile production ownership code once with only its allocator and scanner
 * boundaries substituted. This does not test directory traversal or rules. */
#define calloc TestCalloc
#define realloc TestRealloc
#define free TestFree
#include "../../src/codeguard/result.c"
#include "../../src/codeguard/service.c"
#undef free
#undef realloc
#undef calloc
#include "../../src/codeguard/profile.c"
#include "../../src/codeguard/config.c"
#include "../../src/codeguard/summary.c"
#include "../../src/platform/cancellation.c"

static UmiStatus scan_outcome = UMI_STATUS_OK;
static size_t scan_rows = 1U, scan_calls, expected_retained_rows;
static unsigned scan_seed = 7U;
static char scanned_root[1024];
static UmiCodeGuardProfile scanned_profile;
static UmiCodeGuardService *observed_service;

/* Write real findings before a chosen failure. Observing the retained service
 * during the callback also catches publication before the scan completes. */
UmiStatus umi_codeguard_scan(const UmiCodeGuardConfig *config, UmiCodeGuardResult *result)
{
    ++scan_calls;
    if (observed_service != NULL &&
        umi_codeguard_result_count(umi_codeguard_service_result(observed_service)) !=
            expected_retained_rows) return UMI_STATUS_INTERNAL_ERROR;
    if (strlen(config->root) >= sizeof scanned_root) abort();
    memcpy(scanned_root, config->root, strlen(config->root) + 1U);
    scanned_profile = config->profile;
    for (size_t i = 0U; i < scan_rows; ++i) {
        UmiCodeGuardFinding finding = {0};
        finding.severity = UMI_CODEGUARD_HIGH;
        finding.category = UMI_CODEGUARD_CATEGORY_MEMORY;
        finding.line = i + 1U;
        finding.confidence = scan_seed;
        memcpy(finding.message, "captured finding", sizeof "captured finding");
        UmiStatus status = umi_codeguard_result_add(result, &finding);
        if (status != UMI_STATUS_OK) return status;
    }
    return scan_outcome;
}

/* The ownership fixture substitutes analysis only. The actual atomic token
 * and retained-service publication path remain production code. */
static UmiCancellationToken *request_on_return;
UmiStatus umi_codeguard_scan_cancellable(const UmiCodeGuardConfig *config,
    UmiCodeGuardResult *result, const UmiCancellationToken *cancellation)
{
    (void)cancellation;
    UmiStatus status = umi_codeguard_scan(config, result);
    if (request_on_return != NULL) umi_cancellation_token_request(request_on_return);
    return status;
}

/* Separate invalid-service argument precedence from valid-service preflight cancellation so each registered case proves a distinct contract. The previous implementation is retained for engineering review. */
#if 0
static int CancellationCase(const char *name)
{
    UmiCancellationToken *token = NULL;
    UmiCodeGuardService *service = NULL;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    CHECK(umi_codeguard_service_create("workspace", &service) == UMI_STATUS_OK);
    CHECK(umi_codeguard_service_scan(service, NULL) == UMI_STATUS_OK);
    const UmiCodeGuardResult *stable = umi_codeguard_service_result(service);
    const UmiCodeGuardFinding *borrow = umi_codeguard_result_at(stable, 0U);
    CHECK(borrow != NULL && borrow->confidence == 7U);
    size_t calls = scan_calls, allocated = allocation_count;
    scan_seed = 21U;
    scan_rows = 3U;
    if (!strcmp(name, "token-preflight") || !strcmp(name, "token-invalid")) {
        umi_cancellation_token_request(token);
        CHECK(umi_codeguard_service_scan_cancellable(NULL, NULL, token) ==
            UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_service_scan_cancellable(service, NULL, token) ==
            UMI_STATUS_CANCELLED);
        CHECK(scan_calls == calls && allocation_count == allocated);
    } else if (!strcmp(name, "token-late") || !strcmp(name, "token-error")) {
        request_on_return = token;
        if (!strcmp(name, "token-error")) scan_outcome = UMI_STATUS_IO_ERROR;
        CHECK(umi_codeguard_service_scan_cancellable(service, NULL, token) ==
            (!strcmp(name, "token-error") ? UMI_STATUS_IO_ERROR : UMI_STATUS_CANCELLED));
        CHECK(scan_calls == calls + 1U);
        request_on_return = NULL;
    } else if (!strcmp(name, "token-null")) {
        CHECK(umi_codeguard_service_scan_cancellable(service, NULL, NULL) == UMI_STATUS_OK);
        CHECK(umi_codeguard_service_result(service) == stable);
        CHECK(umi_codeguard_result_count(stable) == 3U);
        CHECK(umi_codeguard_result_at(stable, 0U)->confidence == 21U);
        umi_codeguard_service_destroy(service);
        umi_cancellation_token_destroy(token);
        return 0;
    } else return 2;
    CHECK(umi_codeguard_service_result(service) == stable);
    CHECK(umi_codeguard_result_at(stable, 0U) == borrow);
    CHECK(umi_codeguard_result_count(stable) == 1U && borrow->confidence == 7U);
    CHECK(umi_codeguard_service_summary(service).high == 1U);
    /* Reuse is allowed only after the prior synchronous call has returned. */
    umi_cancellation_token_reset(token);
    scan_outcome = UMI_STATUS_OK;
    CHECK(umi_codeguard_service_scan_cancellable(service, NULL, token) == UMI_STATUS_OK);
    CHECK(umi_codeguard_result_at(stable, 0U)->confidence == 21U);
    umi_codeguard_service_destroy(service);
    umi_cancellation_token_destroy(token);
    return 0;
}
#endif
static int CancellationCase(const char *name)
{
    UmiCancellationToken *token = NULL;
    UmiCodeGuardService *service = NULL;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    CHECK(umi_codeguard_service_create("workspace", &service) == UMI_STATUS_OK);
    CHECK(umi_codeguard_service_scan(service, NULL) == UMI_STATUS_OK);
    const UmiCodeGuardResult *stable = umi_codeguard_service_result(service);
    const UmiCodeGuardFinding *borrow = umi_codeguard_result_at(stable, 0U);
    CHECK(borrow != NULL && borrow->confidence == 7U);
    size_t calls = scan_calls, allocated = allocation_count;
    scan_seed = 21U;
    scan_rows = 3U;
    if (!strcmp(name, "token-invalid")) {
        umi_cancellation_token_request(token);
        CHECK(umi_codeguard_service_scan_cancellable(NULL, NULL, token) ==
            UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_service_scan_cancellable(NULL, NULL, NULL) ==
            UMI_STATUS_INVALID_ARGUMENT);
        CHECK(scan_calls == calls && allocation_count == allocated);
    } else if (!strcmp(name, "token-preflight")) {
        umi_cancellation_token_request(token);
        CHECK(umi_codeguard_service_scan_cancellable(service, NULL, token) ==
            UMI_STATUS_CANCELLED);
        CHECK(scan_calls == calls && allocation_count == allocated);
    } else if (!strcmp(name, "token-late") || !strcmp(name, "token-error")) {
        request_on_return = token;
        if (!strcmp(name, "token-error")) scan_outcome = UMI_STATUS_IO_ERROR;
        CHECK(umi_codeguard_service_scan_cancellable(service, NULL, token) ==
            (!strcmp(name, "token-error") ? UMI_STATUS_IO_ERROR : UMI_STATUS_CANCELLED));
        CHECK(scan_calls == calls + 1U);
        request_on_return = NULL;
    } else if (!strcmp(name, "token-null")) {
        CHECK(umi_codeguard_service_scan_cancellable(service, NULL, NULL) == UMI_STATUS_OK);
        CHECK(umi_codeguard_service_result(service) == stable);
        CHECK(umi_codeguard_result_count(stable) == 3U);
        CHECK(umi_codeguard_result_at(stable, 0U)->confidence == 21U);
        umi_codeguard_service_destroy(service);
        umi_cancellation_token_destroy(token);
        return 0;
    } else return 2;
    CHECK(umi_codeguard_service_result(service) == stable);
    CHECK(umi_codeguard_result_at(stable, 0U) == borrow);
    CHECK(umi_codeguard_result_count(stable) == 1U && borrow->confidence == 7U);
    CHECK(umi_codeguard_service_summary(service).high == 1U);
    /* Reuse is allowed only after the prior synchronous call has returned. */
    umi_cancellation_token_reset(token);
    scan_outcome = UMI_STATUS_OK;
    CHECK(umi_codeguard_service_scan_cancellable(service, NULL, token) == UMI_STATUS_OK);
    CHECK(umi_codeguard_result_at(stable, 0U)->confidence == 21U);
    umi_codeguard_service_destroy(service);
    umi_cancellation_token_destroy(token);
    return 0;
}

/* Compare the public fields, not unspecified C structure padding bytes. */
static int SameFinding(const UmiCodeGuardFinding *a, const UmiCodeGuardFinding *b)
{
    return a->severity == b->severity && a->category == b->category &&
        a->confidence == b->confidence && a->line == b->line &&
        a->column == b->column && !strcmp(a->rule_id, b->rule_id) &&
        !strcmp(a->cwe, b->cwe) && !strcmp(a->path, b->path) &&
        !strcmp(a->message, b->message) && !strcmp(a->remediation, b->remediation);
}

static int ResultCase(const char *name)
{
    UmiCodeGuardResult *left = NULL, *right = NULL;
    if (!strcmp(name, "result-capacity")) {
        left = (UmiCodeGuardResult *)(uintptr_t)1U;
        CHECK(umi_codeguard_result_create(SIZE_MAX, &left) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(left == NULL && allocation_count == 0U);
        CHECK(umi_codeguard_result_create(1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    CHECK(umi_codeguard_result_create(1U, &left) == UMI_STATUS_OK);
    UmiCodeGuardFinding finding = {0};
    finding.confidence = 97U;
    finding.line = 42U;
    finding.severity = UMI_CODEGUARD_HIGH;
    memcpy(finding.message, "owned before growth", sizeof "owned before growth");
    CHECK(umi_codeguard_result_add(left, &finding) == UMI_STATUS_OK);
    const UmiCodeGuardFinding *borrow = umi_codeguard_result_at(left, 0U);
    if (!strcmp(name, "result-alias-growth")) {
        CHECK(umi_codeguard_result_add(left, borrow) == UMI_STATUS_OK);
        CHECK(umi_codeguard_result_count(left) == 2U && quarantine_count == 1U);
        CHECK(SameFinding(umi_codeguard_result_at(left, 1U), &finding));
        CHECK(umi_codeguard_result_at(left, 0U) != borrow);
    } else if (!strcmp(name, "result-alias-no-growth")) {
        CHECK(umi_codeguard_result_add(left, &finding) == UMI_STATUS_OK);
        borrow = umi_codeguard_result_at(left, 1U);
        CHECK(umi_codeguard_result_add(left, borrow) == UMI_STATUS_OK);
        CHECK(SameFinding(umi_codeguard_result_at(left, 2U), &finding));
        CHECK(umi_codeguard_result_at(left, 1U) == borrow);
    } else if (!strcmp(name, "result-growth-failure")) {
        fail_reallocate = 1;
        CHECK(umi_codeguard_result_add(left, borrow) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(umi_codeguard_result_count(left) == 1U);
        CHECK(umi_codeguard_result_at(left, 0U) == borrow);
        CHECK(SameFinding(borrow, &finding));
    } else if (!strcmp(name, "result-growth-boundary")) {
        /* Isolate arithmetic without allocating exabytes; restore real
         * ownership metadata before destruction. */
        const size_t maximum = SIZE_MAX / sizeof finding;
        left->count = maximum / 2U + 1U;
        left->capacity = left->count;
        fail_reallocate = 1;
        CHECK(umi_codeguard_result_add(left, &finding) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(reallocation_bytes == maximum * sizeof finding);
        left->count = maximum;
        left->capacity = maximum;
        CHECK(umi_codeguard_result_add(left, &finding) == UMI_STATUS_CAPACITY_EXCEEDED);
        left->count = 1U;
        left->capacity = 1U;
    } else if (!strcmp(name, "result-swap") || !strcmp(name, "result-swap-empty")) {
        CHECK(umi_codeguard_result_create(1U, &right) == UMI_STATUS_OK);
        if (!strcmp(name, "result-swap")) {
            finding.confidence = 11U;
            CHECK(umi_codeguard_result_add(right, &finding) == UMI_STATUS_OK);
        }
        size_t right_count = umi_codeguard_result_count(right);
        const UmiCodeGuardFinding *right_borrow = umi_codeguard_result_at(right, 0U);
        CHECK(umi_codeguard_result_swap(left, right) == UMI_STATUS_OK);
        CHECK(umi_codeguard_result_count(left) == right_count);
        CHECK(umi_codeguard_result_at(right, 0U) == borrow);
        CHECK(umi_codeguard_result_at(left, 0U) == right_borrow);
        umi_codeguard_result_destroy(right);
    } else if (!strcmp(name, "result-swap-invalid")) {
        CHECK(umi_codeguard_result_swap(left, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_result_swap(NULL, left) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_result_swap(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_result_swap(left, left) == UMI_STATUS_OK);
        CHECK(umi_codeguard_result_at(left, 0U) == borrow);
    } else return 2;
    umi_codeguard_result_destroy(left);
    return 0;
}

static int ServiceCase(const char *name)
{
    UmiCodeGuardService *service = NULL;
    char root[1025];
    memset(root, 'a', sizeof root);
    root[sizeof root - 1U] = '\0';
    if (!strcmp(name, "arguments")) {
        service = (UmiCodeGuardService *)(uintptr_t)1U;
        CHECK(umi_codeguard_service_create(NULL, &service) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(service == NULL);
        CHECK(umi_codeguard_service_create("", &service) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(service == NULL);
        CHECK(umi_codeguard_service_create("workspace", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_service_scan(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_service_result(NULL) == NULL);
        CHECK(umi_codeguard_service_summary(NULL).total == 0U);
        umi_codeguard_service_destroy(NULL);
        return 0;
    }
    if (!strcmp(name, "root-boundary")) {
        CHECK(umi_codeguard_service_create(root, &service) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(service == NULL && allocation_count == 0U);
        root[1023] = '\0';
        CHECK(umi_codeguard_service_create(root, &service) == UMI_STATUS_OK);
        CHECK(umi_codeguard_service_scan(service, NULL) == UMI_STATUS_OK);
        CHECK(!strcmp(scanned_root, root));
        umi_codeguard_service_destroy(service);
        return 0;
    }
    if (!strncmp(name, "create-failure-", 15U)) {
        allocation_fail_at = (size_t)(name[15] - '0');
        CHECK(allocation_fail_at >= 1U && allocation_fail_at <= 3U);
        CHECK(umi_codeguard_service_create("workspace", &service) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(service == NULL);
        return 0;
    }
    memcpy(root, "original-workspace", sizeof "original-workspace");
    CHECK(umi_codeguard_service_create(root, &service) == UMI_STATUS_OK);
    const UmiCodeGuardResult *stable = umi_codeguard_service_result(service);
    CHECK(stable != NULL && umi_codeguard_result_count(stable) == 0U);
    if (!strcmp(name, "root-copy")) {
        memset(root, 'x', sizeof root);
        CHECK(umi_codeguard_service_scan(service, NULL) == UMI_STATUS_OK);
        CHECK(!strcmp(scanned_root, "original-workspace"));
    } else if (!strcmp(name, "profile") || !strcmp(name, "default-profile")) {
        UmiCodeGuardProfile profile = umi_codeguard_profile_security();
        profile.max_line_length = 123U;
        CHECK(umi_codeguard_service_scan(service,
            !strcmp(name, "profile") ? &profile : NULL) == UMI_STATUS_OK);
        UmiCodeGuardProfile expected = !strcmp(name, "profile") ?
            profile : umi_codeguard_profile_default();
        CHECK(scanned_profile.max_line_length == expected.max_line_length);
        CHECK(scanned_profile.scan_duplicates == expected.scan_duplicates);
        CHECK(scanned_profile.fail_on == expected.fail_on);
    } else if (!strcmp(name, "first-failure")) {
        scan_outcome = UMI_STATUS_IO_ERROR;
        CHECK(umi_codeguard_service_scan(service, NULL) == UMI_STATUS_IO_ERROR);
        CHECK(umi_codeguard_result_count(stable) == 0U);
        CHECK(umi_codeguard_service_summary(service).total == 0U);
    } else {
        CHECK(umi_codeguard_service_scan(service, NULL) == UMI_STATUS_OK);
        const UmiCodeGuardFinding *borrow = umi_codeguard_result_at(stable, 0U);
        CHECK(borrow != NULL && borrow->confidence == 7U);
        observed_service = service;
        expected_retained_rows = 1U;
        scan_seed = 21U;
        scan_rows = 3U;
        if (!strcmp(name, "replace") || !strcmp(name, "empty")) {
            if (!strcmp(name, "empty")) scan_rows = 0U;
            CHECK(umi_codeguard_service_scan(service, NULL) == UMI_STATUS_OK);
            CHECK(umi_codeguard_result_count(stable) == scan_rows);
            CHECK(umi_codeguard_service_summary(service).high == scan_rows);
            if (scan_rows != 0U) CHECK(umi_codeguard_result_at(stable, 0U)->confidence == 21U);
        } else {
            UmiStatus expected = UMI_STATUS_IO_ERROR;
            if (!strcmp(name, "cancelled")) expected = UMI_STATUS_CANCELLED;
            else if (!strcmp(name, "scan-memory")) expected = UMI_STATUS_OUT_OF_MEMORY;
            else if (!strcmp(name, "scan-capacity")) expected = UMI_STATUS_CAPACITY_EXCEEDED;
            else if (!strcmp(name, "candidate-object") || !strcmp(name, "candidate-items")) {
                allocation_fail_at = allocation_count +
                    (!strcmp(name, "candidate-object") ? 1U : 2U);
                expected = UMI_STATUS_OUT_OF_MEMORY;
            } else if (strcmp(name, "partial-failure")) return 2;
            scan_outcome = expected;
            size_t calls = scan_calls;
            CHECK(umi_codeguard_service_scan(service, NULL) == expected);
            CHECK(umi_codeguard_result_count(stable) == 1U);
            CHECK(umi_codeguard_result_at(stable, 0U) == borrow);
            CHECK(borrow->confidence == 7U && borrow->line == 1U);
            CHECK(umi_codeguard_service_summary(service).high == 1U);
            if (allocation_fail_at != 0U) CHECK(scan_calls == calls);
        }
    }
    CHECK(umi_codeguard_service_result(service) == stable);
    observed_service = NULL;
    umi_codeguard_service_destroy(service);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int status = !strncmp(argv[1], "token-", 6U) ? CancellationCase(argv[1]) :
        !strncmp(argv[1], "result-", 7U) ?
        ResultCase(argv[1]) : ServiceCase(argv[1]);
    if (status != 0) return status;
    for (size_t i = 0U; i < 32U; ++i) CHECK(allocations[i].pointer == NULL);
    for (size_t i = 0U; i < quarantine_count; ++i) free(quarantine[i]);
    return 0;
}
