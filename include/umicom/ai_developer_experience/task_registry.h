/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/task_registry.h
 *
 * PURPOSE:
 *   Retain bounded coding-agent task summaries for IDE task lists and durable
 *   restoration without owning the underlying agent execution state machine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_REGISTRY_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_REGISTRY_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai developer task registry data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperTaskRegistry UmiAiDeveloperTaskRegistry;

/**
 * Initialise ai developer task registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_task_registry_create(
    UmiAiDeveloperTaskRegistry **out_registry);
/**
 * Release or reset state held by ai developer task registry so the same storage can be
 * reused safely.
 */
void umi_ai_developer_task_registry_destroy(
    UmiAiDeveloperTaskRegistry *registry);

/**
 * Provide the ai developer task registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_task_registry_upsert(
    UmiAiDeveloperTaskRegistry *registry,
    const UmiAiDeveloperTaskEntry *entry);

/**
 * Find ai developer task registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_developer_task_registry_find(
    const UmiAiDeveloperTaskRegistry *registry,
    const char *task_id,
    UmiAiDeveloperTaskEntry *out_entry);

/**
 * Find ai developer task registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_developer_task_registry_at(
    const UmiAiDeveloperTaskRegistry *registry,
    size_t index,
    UmiAiDeveloperTaskEntry *out_entry);

/**
 * Return the number of records represented by ai developer task registry without changing
 * their state.
 */
size_t umi_ai_developer_task_registry_count(
    const UmiAiDeveloperTaskRegistry *registry);

/**
 * Provide the ai developer task registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ai_developer_task_registry_revision(
    const UmiAiDeveloperTaskRegistry *registry);

/**
 * Release or reset state held by ai developer task registry so the same storage can be
 * reused safely.
 */
void umi_ai_developer_task_registry_clear(
    UmiAiDeveloperTaskRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
