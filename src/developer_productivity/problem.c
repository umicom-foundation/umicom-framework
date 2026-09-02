/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/problem.c
 *
 * PURPOSE:
 *   Validate normalized developer problems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/problem.h"

/* Check that developer problem satisfies its contract before another service relies on it. */
UmiStatus umi_developer_problem_validate(
    const UmiDeveloperProblem *problem)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (problem == NULL ||
        problem->source[0] == '\0' ||
        problem->message[0] == '\0' ||
        problem->severity < UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_HINT ||
        problem->severity > UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_FATAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (problem->location.uri[0] != '\0') {
        return umi_developer_productivity_location_validate(
            &problem->location);
    }

    return UMI_STATUS_OK;
}
