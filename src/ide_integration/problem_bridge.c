/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/problem_bridge.c
 *
 * PURPOSE:
 *   Implement Problems -> editor/AI cross-domain projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/problem_bridge.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

static UmiStatus size_to_u32(size_t value, uint32_t *out_value)
{
    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (value > UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_value = (uint32_t)value;
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_problem_target(
    const UmiDeveloperProblemStore *problems,
    size_t index,
    UmiIdeNavigationTarget *out_target)
{
    UmiDeveloperProblem problem;
    UmiStatus status;

    if (problems == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_problem_store_at(
        problems,
        index,
        &problem);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_target, 0, sizeof(*out_target));
    out_target->domain = UMI_IDE_DOMAIN_PROBLEMS;
    out_target->reason = UMI_IDE_NAVIGATION_PROBLEM;

    (void)snprintf(
        out_target->subject_id,
        sizeof(out_target->subject_id),
        "problem.%llu",
        (unsigned long long)problem.problem_id);
    (void)snprintf(
        out_target->label,
        sizeof(out_target->label),
        "%.220s",
        problem.message);

    umi_ide_location_init(&out_target->location);

    if (problem.location.uri[0] != '\0') {
        uint32_t line = 0U;
        uint32_t column = 0U;

        status = size_to_u32(problem.location.line, &line);
        if (status == UMI_STATUS_OK) {
            status = size_to_u32(problem.location.column, &column);
        }
        if (status != UMI_STATUS_OK) return status;

        status = umi_ide_location_set_uri(
            &out_target->location,
            problem.location.uri,
            line,
            column);
        if (status != UMI_STATUS_OK) return status;

        if (size_to_u32(
                problem.location.end_line,
                &out_target->location.end_line) != UMI_STATUS_OK ||
            size_to_u32(
                problem.location.end_column,
                &out_target->location.end_column) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ide_problem_ai_summary(
    const UmiDeveloperProblemStore *problems,
    size_t index,
    char *out_text,
    size_t capacity)
{
    UmiDeveloperProblem problem;
    int written;
    UmiStatus status;

    if (problems == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_problem_store_at(
        problems,
        index,
        &problem);
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        out_text,
        capacity,
        "Problem source=%.120s code=%.120s severity=%s "
        "location=%.800s:%zu:%zu message=%.900s",
        problem.source,
        problem.code,
        umi_developer_productivity_severity_text(problem.severity),
        problem.location.uri,
        problem.location.line,
        problem.location.column,
        problem.message);

    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}
