/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/problem.h
 *
 * PURPOSE:
 *   Define one normalized compiler/build/test/runtime problem record for the
 *   Problems panel, navigation, AI repair workflows and machine reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_PROBLEM_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_PROBLEM_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer problem data shared with callers of this public contract.
 */
typedef struct UmiDeveloperProblem {
    uint64_t problem_id;
    char source[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char code[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char message[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
    UmiDeveloperProductivitySeverity severity;
    UmiDeveloperProductivityLocation location;
    int suppressible;
    int transient;
    uint64_t revision;
} UmiDeveloperProblem;

/**
 * Check that developer problem satisfies its contract before another service relies on it.
 */
UmiStatus umi_developer_problem_validate(
    const UmiDeveloperProblem *problem);

#ifdef __cplusplus
}
#endif

#endif
