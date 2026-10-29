/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_command_lock_failures.c
 *
 * PURPOSE:
 *   Verify registry operations preserve state when mutex acquisition fails.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/command_registry.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This executable compiles the production registry with a small test-only
 * mutex implementation. No platform threading object is linked. Deterministic
 * failure injection exercises the error boundary on Windows and Unix alike;
 * it does not replace tests of real thread scheduling or concurrent callers. */
struct UmiMutex { int held; };
static unsigned fail_next_lock;
static unsigned lock_attempts;
static unsigned unlock_attempts;
static unsigned invalid_unlocks;
static unsigned handler_calls;
static unsigned enabled_calls;

UmiStatus umi_mutex_create(UmiMutex **out_mutex)
{
    if (out_mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_mutex = calloc(1U, sizeof(**out_mutex));
    return *out_mutex == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
}
void umi_mutex_destroy(UmiMutex *mutex)
{
    if (mutex != NULL && mutex->held) ++invalid_unlocks;
    free(mutex);
}
UmiStatus umi_mutex_lock(UmiMutex *mutex)
{
    ++lock_attempts;
    if (mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (fail_next_lock != 0U) {
        --fail_next_lock;
        return UMI_STATUS_INTERNAL_ERROR;
    }
    if (mutex->held) return UMI_STATUS_INVALID_STATE;
    mutex->held = 1;
    return UMI_STATUS_OK;
}
UmiStatus umi_mutex_unlock(UmiMutex *mutex)
{
    ++unlock_attempts;
    if (mutex == NULL || !mutex->held) {
        ++invalid_unlocks;
        return UMI_STATUS_INVALID_STATE;
    }
    mutex->held = 0;
    return UMI_STATUS_OK;
}
static UmiStatus Handle(void *context, const char *argument, char *message, size_t capacity)
{
    (void)context;
    (void)argument;
    ++handler_calls;
    if (message != NULL && capacity != 0U)
        (void)snprintf(message, capacity, "%s", "handled");
    return UMI_STATUS_OK;
}
static int Enabled(void *context, const char *argument)
{
    (void)context;
    (void)argument;
    ++enabled_calls;
    return 1;
}
static UmiCommandDescriptor Descriptor(const char *id)
{
    UmiCommandDescriptor descriptor = {0};
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = id;
    descriptor.title = "Lock failure fixture";
    descriptor.category = "Verification";
    descriptor.description = "No mutation or callback without the registry lock";
    descriptor.required_permission = "";
    descriptor.flags = UMI_COMMAND_MUTATES_STATE;
    descriptor.handler = Handle;
    descriptor.enabled = Enabled;
    return descriptor;
}
/* Keep setup and cleanup independent of NDEBUG and report the failing condition
 * before releasing the test registry. Production status values remain visible. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        result = 1; \
        goto cleanup; \
    } \
} while (0)

static int CheckCase(const char *name)
{
    int result = 0;
    UmiCommandRegistry *registry = NULL;
    UmiStatus status = umi_command_registry_create(&registry);
    CHECK(status == UMI_STATUS_OK);
    UmiCommandDescriptor existing = Descriptor("existing.command");
    UmiCommandDescriptor added = Descriptor("new.command");
    status = umi_command_registry_register(registry, &existing);
    CHECK(status == UMI_STATUS_OK);

    UmiCommandSnapshot snapshot, saved;
    (void)memset(&snapshot, 0xa5, sizeof(snapshot));
    (void)memcpy(&saved, &snapshot, sizeof(saved));
    UmiCommandRegistrySummary summary, summary_saved;
    (void)memset(&summary, 0x5a, sizeof(summary));
    (void)memcpy(&summary_saved, &summary, sizeof(summary_saved));
    char message[32] = "sentinel";
    size_t matches = 77U;
    UmiCommandBatchReport report;
    unsigned old_locks = lock_attempts;
    unsigned old_unlocks = unlock_attempts;
    fail_next_lock = 1U;

    if (strcmp(name, "register") == 0) {
        status = umi_command_registry_register(registry, &added);
        CHECK(status == UMI_STATUS_INTERNAL_ERROR);
    } else if (strcmp(name, "unregister") == 0) {
        status = umi_command_registry_unregister(registry, existing.command_id);
        CHECK(status == UMI_STATUS_INTERNAL_ERROR);
    } else if (strcmp(name, "snapshot") == 0) {
        status = umi_command_registry_snapshot(registry, existing.command_id, &snapshot);
        CHECK(status == UMI_STATUS_INTERNAL_ERROR);
        CHECK(memcmp(&snapshot, &saved, sizeof(snapshot)) == 0);
    } else if (strcmp(name, "at") == 0) {
        status = umi_command_registry_at(registry, 0U, &snapshot);
        CHECK(status == UMI_STATUS_INTERNAL_ERROR);
        CHECK(memcmp(&snapshot, &saved, sizeof(snapshot)) == 0);
    } else if (strcmp(name, "count") == 0) {
        size_t count = umi_command_registry_count(registry);
        CHECK(count == 0U);
    } else if (strcmp(name, "contains") == 0) {
        int contains = umi_command_registry_contains(registry, existing.command_id);
        CHECK(!contains);
    } else if (strcmp(name, "enabled") == 0) {
        int enabled = umi_command_registry_is_enabled(registry, existing.command_id, "");
        CHECK(!enabled);
    } else if (strcmp(name, "execute") == 0) {
        status = umi_command_registry_execute(registry, existing.command_id, "",
                                               message, sizeof(message));
        CHECK(status == UMI_STATUS_INTERNAL_ERROR && message[0] == '\0');
    } else if (strcmp(name, "batch") == 0) {
        status = umi_command_registry_register_many(registry, &added, 1U, &report);
        CHECK(status == UMI_STATUS_INTERNAL_ERROR);
        CHECK(report.status == status && report.requested_count == 1U &&
              report.registered_count == 0U && report.failed_index == SIZE_MAX);
    } else if (strcmp(name, "prefix") == 0) {
        status = umi_command_registry_find_prefix(registry, "", &snapshot, 1U, &matches);
        CHECK(status == UMI_STATUS_INTERNAL_ERROR && matches == 77U);
        CHECK(memcmp(&snapshot, &saved, sizeof(snapshot)) == 0);
    } else if (strcmp(name, "summary") == 0) {
        status = UmiCommandRegistryCopySummary(registry, &summary);
        CHECK(status == UMI_STATUS_INTERNAL_ERROR);
        CHECK(memcmp(&summary, &summary_saved, sizeof(summary)) == 0);
    } else {
        CHECK(!"Unknown command lock failure case");
    }
    CHECK(fail_next_lock == 0U && lock_attempts == old_locks + 1U);
    CHECK(unlock_attempts == old_unlocks && invalid_unlocks == 0U);
    CHECK(handler_calls == 0U && enabled_calls == 0U);

    /* Recovery uses ordinary registry operations, so the fixture also detects
     * an unintended insert, removal or damaged lock state after rejection. */
    size_t count = umi_command_registry_count(registry);
    CHECK(count == 1U);
    status = umi_command_registry_snapshot(registry, existing.command_id, &snapshot);
    CHECK(status == UMI_STATUS_OK && snapshot.flags == UMI_COMMAND_MUTATES_STATE);
    int contains = umi_command_registry_contains(registry, added.command_id);
    CHECK(!contains);
    status = umi_command_registry_execute(registry, existing.command_id, "", message, sizeof(message));
    CHECK(status == UMI_STATUS_OK && strcmp(message, "handled") == 0);
    CHECK(handler_calls == 1U && enabled_calls == 1U);
cleanup:
    fail_next_lock = 0U;
    umi_command_registry_destroy(registry);
    if (invalid_unlocks != 0U) result = 1;
    return result;
}
int main(int argc, char **argv)
{
    if (argc != 2) {
        (void)fprintf(stderr, "Supply one command lock failure scenario\n");
        return 2;
    }
    return CheckCase(argv[1]);
}
