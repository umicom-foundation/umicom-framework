/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/task_persistence.h
 *
 * PURPOSE:
 *   Persist recent AI coding task summaries into the Framework Session Store.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_PERSISTENCE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_PERSISTENCE_H
#include "umicom/ai_developer_experience/persistence_codec.h"
#include "umicom/ai_developer_experience/task_registry.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEVELOPER_PERSISTED_TASKS 6U

UmiStatus umi_ai_developer_tasks_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperTaskRegistry *registry);

UmiStatus umi_ai_developer_tasks_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperTaskRegistry *registry,
    size_t *out_restored_count);

#ifdef __cplusplus
}
#endif
#endif
