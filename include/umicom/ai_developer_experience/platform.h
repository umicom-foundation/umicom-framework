/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/platform.h
 *
 * PURPOSE:
 *   Compose the operational AI Coding Runtime and controlled Coding Tools into
 *   a reusable AI Developer Experience platform with approval, review, task,
 *   timeline, presentation and durable-session services.
 *
 * APPLICATION BOUNDARY:
 *   Thin applications such as Umicom Studio render view models and dispatch
 *   commands; execution, policy and durable state remain Framework-owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PLATFORM_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PLATFORM_H

#include "umicom/ai_developer_experience/activity_sync.h"
#include "umicom/ai_developer_experience/approval_service.h"
#include "umicom/ai_developer_experience/durable_store.h"
#include "umicom/ai_developer_experience/patch_review_service.h"
#include "umicom/ai_developer_experience/preferences.h"
#include "umicom/ai_developer_experience/presentation_state.h"
#include "umicom/ai_developer_experience/session_bundle.h"
#include "umicom/ai_developer_experience/task_projection.h"
#include "umicom/ai_developer_experience/timeline_projection.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperExperienceSnapshot {
    UmiAiCodingRuntimePlatformSnapshot coding;
    UmiAiCodingToolsPlatformSnapshot tools;
    UmiAiDeveloperPresentationState presentation;
    UmiAiDeveloperPreferences preferences;
    size_t task_count;
    size_t pending_approval_count;
    size_t timeline_count;
    uint64_t revision;
    int review_loaded;
    int durable_state_enabled;
} UmiAiDeveloperExperienceSnapshot;

typedef struct UmiAiDeveloperExperiencePlatform
    UmiAiDeveloperExperiencePlatform;

UmiStatus umi_ai_developer_experience_platform_create(
    UmiAiCodingRuntimePlatform *coding_runtime,
    UmiAiCodingToolsPlatform *tools,
    UmiAiDeveloperExperiencePlatform **out_platform);

void umi_ai_developer_experience_platform_destroy(
    UmiAiDeveloperExperiencePlatform *platform);

UmiStatus umi_ai_developer_experience_platform_enable_durable_state(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *path,
    const char *key_prefix);

UmiStatus umi_ai_developer_experience_platform_refresh(
    UmiAiDeveloperExperiencePlatform *platform);

UmiStatus umi_ai_developer_experience_platform_save(
    UmiAiDeveloperExperiencePlatform *platform);

UmiStatus umi_ai_developer_experience_platform_restore(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperRestoreReport *out_report);

UmiStatus umi_ai_developer_experience_platform_select_pane(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperPaneKind pane);

UmiStatus umi_ai_developer_experience_platform_select_approval(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id);

UmiStatus umi_ai_developer_experience_platform_approve(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id,
    const char *approved_by);

UmiStatus umi_ai_developer_experience_platform_reject(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id);

UmiStatus umi_ai_developer_experience_platform_open_patch(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *patch_id);

UmiStatus umi_ai_developer_experience_platform_select_patch_file(
    UmiAiDeveloperExperiencePlatform *platform,
    size_t file_index);

UmiStatus umi_ai_developer_experience_platform_mark_patch_file_reviewed(
    UmiAiDeveloperExperiencePlatform *platform,
    size_t file_index,
    int reviewed);

UmiStatus umi_ai_developer_experience_platform_snapshot(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperExperienceSnapshot *out_snapshot);

UmiAiDeveloperApprovalService *
umi_ai_developer_experience_platform_approvals(
    UmiAiDeveloperExperiencePlatform *platform);

UmiAiDeveloperTaskRegistry *
umi_ai_developer_experience_platform_tasks(
    UmiAiDeveloperExperiencePlatform *platform);

UmiAiDeveloperTimeline *
umi_ai_developer_experience_platform_timeline(
    UmiAiDeveloperExperiencePlatform *platform);

UmiAiDeveloperPatchReviewService *
umi_ai_developer_experience_platform_review(
    UmiAiDeveloperExperiencePlatform *platform);

UmiAiDeveloperPresentationState *
umi_ai_developer_experience_platform_presentation(
    UmiAiDeveloperExperiencePlatform *platform);

UmiAiDeveloperPreferences *
umi_ai_developer_experience_platform_preferences(
    UmiAiDeveloperExperiencePlatform *platform);

UmiAiCodingRuntimePlatform *
umi_ai_developer_experience_platform_coding(
    UmiAiDeveloperExperiencePlatform *platform);

UmiAiCodingToolsPlatform *
umi_ai_developer_experience_platform_tools(
    UmiAiDeveloperExperiencePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
