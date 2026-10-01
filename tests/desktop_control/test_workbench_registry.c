/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_workbench_registry.c
 * PURPOSE: Validate the Framework-owned workbench registry contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/workbench_registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Compare the documented default fields, not struct padding. */
static int RecordDefaultsEqual(const UmiDesktopWorkbenchRegistrySnapshot *left, const UmiDesktopWorkbenchRegistrySnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->subject_id, right->subject_id, sizeof(left->subject_id)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->state == right->state &&
        left->priority == right->priority &&
        left->revision == right->revision &&
        left->enabled == right->enabled;
}
/* Public records may arrive from a caller's memory. Each fixed text field
 * must contain its own terminator; a later field cannot supply one for it. */
static int RecordRejectsUnterminatedFields(void)
{
    UmiDesktopWorkbenchRegistrySnapshot value;
    umi_desktop_workbench_registry_init(&value, "bounded-record");
    memset(value.id, 'x', sizeof(value.id));
    if (umi_desktop_workbench_registry_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_desktop_workbench_registry_init(&value, "bounded-record");
    memset(value.subject_id, 'x', sizeof(value.subject_id));
    if (umi_desktop_workbench_registry_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_desktop_workbench_registry_init(&value, "bounded-record");
    memset(value.detail, 'x', sizeof(value.detail));
    if (umi_desktop_workbench_registry_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    return 0;
}
#define RECORD_TYPE UmiDesktopWorkbenchRegistrySnapshot
#define RECORD_INIT umi_desktop_workbench_registry_init
#define RECORD_VALIDATE umi_desktop_workbench_registry_validate
#define RECORD_INIT_CHECKED umi_desktop_workbench_registry_init_checked
#include "../record_integrity/record_construction_cases.h"

/* Each mutation refusal must preserve the complete accepted value. These
 * checks remain active in optimized builds where assert may be disabled. */
static int RecordMutationCases(void)
{
    UmiDesktopWorkbenchRegistrySnapshot value;
    umi_desktop_workbench_registry_init(&value, "mutation-record");
    unsigned char before[sizeof(value)];
    umi_desktop_workbench_registry_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.subject_id) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_desktop_workbench_registry_set_subject(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_desktop_workbench_registry_set_subject(&value, oversized) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_desktop_workbench_registry_set_subject(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_desktop_workbench_registry_set_subject(&value, value.subject_id + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.subject_id, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.subject_id) - 1U] = '\0';
        if (umi_desktop_workbench_registry_set_subject(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.subject_id, oversized) != 0) return 1;
    }
    umi_desktop_workbench_registry_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_desktop_workbench_registry_set_subject(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_desktop_workbench_registry_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_desktop_workbench_registry_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_desktop_workbench_registry_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_desktop_workbench_registry_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_desktop_workbench_registry_set_detail(&value, value.detail + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.detail) - 1U] = '\0';
        if (umi_desktop_workbench_registry_set_detail(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, oversized) != 0) return 1;
    }
    umi_desktop_workbench_registry_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_desktop_workbench_registry_set_detail(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_desktop_workbench_registry_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_desktop_workbench_registry_set_state(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_desktop_workbench_registry_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_desktop_workbench_registry_set_priority(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_desktop_workbench_registry_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_desktop_workbench_registry_set_enabled(&value, true) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    UmiDesktopWorkbenchRegistrySnapshot other;
    umi_desktop_workbench_registry_init(&value, "identity-record");
    umi_desktop_workbench_registry_init(&other, "identity-record");
    if (!umi_desktop_workbench_registry_same_identity(&value, &other)) return 1;
    memcpy(before, &value, sizeof(value));
    memset(other.id, 'x', sizeof(other.id));
    if (umi_desktop_workbench_registry_same_identity(&value, &other) || umi_desktop_workbench_registry_same_identity(&other, &value)) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

int main(void)
{
    if (RecordMutationCases() != 0) return 1;
    if (RecordConstructionCases() != 0) return 1;
    UmiDesktopWorkbenchRegistrySnapshot value;
    UmiDesktopWorkbenchRegistrySnapshot copy;
    umi_desktop_workbench_registry_init(&value, "workbench_registry.primary");
    assert(umi_desktop_workbench_registry_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_workbench_registry_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_workbench_registry_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_workbench_registry_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_workbench_registry_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_workbench_registry_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
