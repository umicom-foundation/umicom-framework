/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/test_bridge.c
 *
 * PURPOSE:
 *   Implement Test Platform -> editor/AI projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/test_bridge.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ide_test_item_target(
    UmiTestPlatformService *tests,
    const char *item_id,
    UmiIdeNavigationTarget *out_target)
{
    UmiTestPlatformItemSnapshot item;
    UmiStatus status;

    if (tests == NULL || item_id == NULL ||
        item_id[0] == '\0' || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_test_platform_item_registry_find(
        umi_test_platform_service_item(tests),
        item_id,
        &item);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_target, 0, sizeof(*out_target));
    out_target->domain = UMI_IDE_DOMAIN_TESTS;
    out_target->reason = UMI_IDE_NAVIGATION_TEST;

    (void)snprintf(
        out_target->subject_id,
        sizeof(out_target->subject_id),
        "%s",
        item.id);
    (void)snprintf(
        out_target->label,
        sizeof(out_target->label),
        "%.240s",
        item.name);

    umi_ide_location_init(&out_target->location);

    if (item.source_uri[0] != '\0') {
        status = umi_ide_location_set_uri(
            &out_target->location,
            item.source_uri,
            item.source_line,
            0U);
    } else if (item.uri[0] != '\0') {
        status = umi_ide_location_set_uri(
            &out_target->location,
            item.uri,
            item.source_line,
            0U);
    }

    return status;
}

UmiStatus umi_ide_test_result_summary(
    UmiTestPlatformService *tests,
    const char *result_id,
    char *out_text,
    size_t capacity)
{
    UmiTestPlatformResultSnapshot result;
    int written;
    UmiStatus status;

    if (tests == NULL || result_id == NULL ||
        out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_test_platform_result_registry_find(
        umi_test_platform_service_result(tests),
        result_id,
        &result);
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        out_text,
        capacity,
        "Test result item=%.120s outcome=%s exit=%d duration=%.3fms "
        "message=%.900s details=%.1600s",
        result.item_id,
        umi_test_platform_outcome_text(
            (UmiTestPlatformOutcome)result.outcome),
        result.exit_code,
        result.duration_ms,
        result.message,
        result.failure_details);

    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}
