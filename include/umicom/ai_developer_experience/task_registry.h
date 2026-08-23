/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/task_registry.h
 *
 * PURPOSE:
 *   Retain bounded coding-agent task summaries for IDE task lists and durable
 *   restoration without owning the underlying agent execution state machine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_REGISTRY_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_REGISTRY_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperTaskRegistry UmiAiDeveloperTaskRegistry;

UmiStatus umi_ai_developer_task_registry_create(
    UmiAiDeveloperTaskRegistry **out_registry);
void umi_ai_developer_task_registry_destroy(
    UmiAiDeveloperTaskRegistry *registry);

UmiStatus umi_ai_developer_task_registry_upsert(
    UmiAiDeveloperTaskRegistry *registry,
    const UmiAiDeveloperTaskEntry *entry);

UmiStatus umi_ai_developer_task_registry_find(
    const UmiAiDeveloperTaskRegistry *registry,
    const char *task_id,
    UmiAiDeveloperTaskEntry *out_entry);

UmiStatus umi_ai_developer_task_registry_at(
    const UmiAiDeveloperTaskRegistry *registry,
    size_t index,
    UmiAiDeveloperTaskEntry *out_entry);

size_t umi_ai_developer_task_registry_count(
    const UmiAiDeveloperTaskRegistry *registry);

uint64_t umi_ai_developer_task_registry_revision(
    const UmiAiDeveloperTaskRegistry *registry);

void umi_ai_developer_task_registry_clear(
    UmiAiDeveloperTaskRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
