/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/problem_navigation.h
 *
 * PURPOSE:
 *   Navigate normalized Problems by severity and source location.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_PROBLEM_NAVIGATION_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_PROBLEM_NAVIGATION_H

#include "umicom/developer_productivity/problem_store.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer problem next operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_problem_next(
    const UmiDeveloperProblemStore *store,
    size_t after_index,
    UmiDeveloperProductivitySeverity minimum_severity,
    size_t *out_index,
    UmiDeveloperProblem *out_problem);

/**
 * Provide the developer problem previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_problem_previous(
    const UmiDeveloperProblemStore *store,
    size_t before_index,
    UmiDeveloperProductivitySeverity minimum_severity,
    size_t *out_index,
    UmiDeveloperProblem *out_problem);

#ifdef __cplusplus
}
#endif

#endif
