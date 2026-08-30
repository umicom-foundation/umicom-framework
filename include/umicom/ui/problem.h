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

/* BEGINNER NOTE:
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

typedef struct UmiUiProblemRegistry UmiUiProblemRegistry;

UmiStatus umi_ui_problem_registry_create(UmiUiProblemRegistry **out_registry);
void umi_ui_problem_registry_destroy(UmiUiProblemRegistry *registry);
UmiStatus umi_ui_problem_registry_upsert(UmiUiProblemRegistry *registry, const UmiUiProblemSnapshot *item);
UmiStatus umi_ui_problem_registry_remove(UmiUiProblemRegistry *registry, const char *id);
UmiStatus umi_ui_problem_registry_find(const UmiUiProblemRegistry *registry, const char *id, UmiUiProblemSnapshot *out_item);
UmiStatus umi_ui_problem_registry_at(const UmiUiProblemRegistry *registry, size_t index, UmiUiProblemSnapshot *out_item);
size_t umi_ui_problem_registry_count(const UmiUiProblemRegistry *registry);
uint64_t umi_ui_problem_registry_revision(const UmiUiProblemRegistry *registry);
void umi_ui_problem_registry_clear(UmiUiProblemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
