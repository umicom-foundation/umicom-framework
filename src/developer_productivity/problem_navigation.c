/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/problem_navigation.c
 *
 * PURPOSE:
 *   Implement next/previous Problems navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/problem_navigation.h"

/*
 * Provide the developer problem next operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_problem_next(
    const UmiDeveloperProblemStore *store,
    size_t after_index,
    UmiDeveloperProductivitySeverity minimum_severity,
    size_t *out_index,
    UmiDeveloperProblem *out_problem)
{
    size_t index;
    const size_t count = umi_developer_problem_store_count(store);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_index == NULL || out_problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = after_index + 1U; index < count; ++index) {
        UmiDeveloperProblem problem;

        /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the developer problem previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_problem_previous(
    const UmiDeveloperProblemStore *store,
    size_t before_index,
    UmiDeveloperProductivitySeverity minimum_severity,
    size_t *out_index,
    UmiDeveloperProblem *out_problem)
{
    size_t index = before_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_index == NULL || out_problem == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        UmiDeveloperProblem problem;
        index -= 1U;

        /* Apply this branch only when its contract condition is satisfied. */
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
