/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/problem.h
 *
 * PURPOSE:
 *   Define an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_UI_PROBLEM_H
#define UMICOM_UI_PROBLEM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PROBLEM_CAPACITY 4096U
#define UMI_UI_PROBLEM_API_VERSION 1U

/**
 * Represent the ui problem snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiProblemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char source[128];
    char code[128];
    char message[1024];
    char uri[1024];
    uint32_t line;
    uint32_t column;
    int severity;
    int resolved;
    uint64_t revision;
} UmiUiProblemSnapshot;

/**
 * Represent the ui problem registry data shared with callers of this public contract.
 */
typedef struct UmiUiProblemRegistry UmiUiProblemRegistry;

/**
 * Initialise ui problem registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_problem_registry_create(UmiUiProblemRegistry **out_registry);
/**
 * Release or reset state held by ui problem registry so the same storage can be reused
 * safely.
 */
void umi_ui_problem_registry_destroy(UmiUiProblemRegistry *registry);
/**
 * Provide the ui problem registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_problem_registry_upsert(UmiUiProblemRegistry *registry, const UmiUiProblemSnapshot *item);
/**
 * Remove ui problem registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_problem_registry_remove(UmiUiProblemRegistry *registry, const char *id);
/**
 * Find ui problem registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_problem_registry_find(const UmiUiProblemRegistry *registry, const char *id, UmiUiProblemSnapshot *out_item);
/**
 * Find ui problem registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_problem_registry_at(const UmiUiProblemRegistry *registry, size_t index, UmiUiProblemSnapshot *out_item);
/**
 * Return the number of records represented by ui problem registry without changing their
 * state.
 */
size_t umi_ui_problem_registry_count(const UmiUiProblemRegistry *registry);
/**
 * Provide the ui problem registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_problem_registry_revision(const UmiUiProblemRegistry *registry);
/**
 * Release or reset state held by ui problem registry so the same storage can be reused
 * safely.
 */
void umi_ui_problem_registry_clear(UmiUiProblemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
