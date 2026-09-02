/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/problem_store.h
 *
 * PURPOSE:
 *   Retain normalized problems by value with source replacement/filtering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_PROBLEM_STORE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_PROBLEM_STORE_H

#include "umicom/developer_productivity/problem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROBLEM_CAPACITY 4096U

/**
 * Represent the developer problem store data shared with callers of this public contract.
 */
typedef struct UmiDeveloperProblemStore UmiDeveloperProblemStore;

/**
 * Represent the developer problem store snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProblemStoreSnapshot {
    size_t count;
    size_t hints;
    size_t infos;
    size_t warnings;
    size_t errors;
    size_t fatals;
    uint64_t revision;
} UmiDeveloperProblemStoreSnapshot;

/**
 * Initialise developer problem store from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_problem_store_create(
    UmiDeveloperProblemStore **out_store);

/**
 * Release or reset state held by developer problem store so the same storage can be reused
 * safely.
 */
void umi_developer_problem_store_destroy(
    UmiDeveloperProblemStore *store);

/**
 * Add developer problem store only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_problem_store_add(
    UmiDeveloperProblemStore *store,
    const UmiDeveloperProblem *problem);

/**
 * Find developer problem store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_problem_store_at(
    const UmiDeveloperProblemStore *store,
    size_t index,
    UmiDeveloperProblem *out_problem);

/**
 * Release or reset state held by developer problem store so the same storage can be reused
 * safely.
 */
void umi_developer_problem_store_clear(
    UmiDeveloperProblemStore *store);

/**
 * Provide the developer problem store clear source operation used by this module and its
 * client applications.
 */
void umi_developer_problem_store_clear_source(
    UmiDeveloperProblemStore *store,
    const char *source);

/**
 * Return the number of records represented by developer problem store without changing
 * their state.
 */
size_t umi_developer_problem_store_count(
    const UmiDeveloperProblemStore *store);

/**
 * Provide the developer problem store snapshot operation used by this module and its
 * client applications.
 */
UmiDeveloperProblemStoreSnapshot umi_developer_problem_store_snapshot(
    const UmiDeveloperProblemStore *store);

#ifdef __cplusplus
}
#endif

#endif
