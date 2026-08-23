/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/problem_navigation.c
 *
 * PURPOSE:
 *   Implement next/previous Problems navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/problem_navigation.h"

UmiStatus umi_developer_problem_next(
    const UmiDeveloperProblemStore *store,
    size_t after_index,
    UmiDeveloperProductivitySeverity minimum_severity,
    size_t *out_index,
    UmiDeveloperProblem *out_problem)
{
    size_t index;
    const size_t count = umi_developer_problem_store_count(store);

    if (store == NULL || out_index == NULL || out_problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = after_index + 1U; index < count; ++index) {
        UmiDeveloperProblem problem;

        if (umi_developer_problem_store_at(
                store, index, &problem) == UMI_STATUS_OK &&
            problem.severity >= minimum_severity) {
            *out_index = index;
            *out_problem = problem;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_developer_problem_previous(
    const UmiDeveloperProblemStore *store,
    size_t before_index,
    UmiDeveloperProductivitySeverity minimum_severity,
    size_t *out_index,
    UmiDeveloperProblem *out_problem)
{
    size_t index = before_index;

    if (store == NULL || out_index == NULL || out_problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (index > 0U) {
        UmiDeveloperProblem problem;
        index -= 1U;

        if (umi_developer_problem_store_at(
                store, index, &problem) == UMI_STATUS_OK &&
            problem.severity >= minimum_severity) {
            *out_index = index;
            *out_problem = problem;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
