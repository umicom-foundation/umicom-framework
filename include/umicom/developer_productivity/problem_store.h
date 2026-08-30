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

typedef struct UmiDeveloperProblemStore UmiDeveloperProblemStore;

typedef struct UmiDeveloperProblemStoreSnapshot {
    size_t count;
    size_t hints;
    size_t infos;
    size_t warnings;
    size_t errors;
    size_t fatals;
    uint64_t revision;
} UmiDeveloperProblemStoreSnapshot;

UmiStatus umi_developer_problem_store_create(
    UmiDeveloperProblemStore **out_store);

void umi_developer_problem_store_destroy(
    UmiDeveloperProblemStore *store);

UmiStatus umi_developer_problem_store_add(
    UmiDeveloperProblemStore *store,
    const UmiDeveloperProblem *problem);

UmiStatus umi_developer_problem_store_at(
    const UmiDeveloperProblemStore *store,
    size_t index,
    UmiDeveloperProblem *out_problem);

void umi_developer_problem_store_clear(
    UmiDeveloperProblemStore *store);

void umi_developer_problem_store_clear_source(
    UmiDeveloperProblemStore *store,
    const char *source);

size_t umi_developer_problem_store_count(
    const UmiDeveloperProblemStore *store);

UmiDeveloperProblemStoreSnapshot umi_developer_problem_store_snapshot(
    const UmiDeveloperProblemStore *store);

#ifdef __cplusplus
}
#endif

#endif
