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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the ai developer experience snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the ai developer experience platform data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperExperiencePlatform
    UmiAiDeveloperExperiencePlatform;

/**
 * Initialise ai developer experience platform from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ai_developer_experience_platform_create(
    UmiAiCodingRuntimePlatform *coding_runtime,
    UmiAiCodingToolsPlatform *tools,
    UmiAiDeveloperExperiencePlatform **out_platform);

/**
 * Release or reset state held by ai developer experience platform so the same storage can
 * be reused safely.
 */
void umi_ai_developer_experience_platform_destroy(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform enable durable state operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_enable_durable_state(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *path,
    const char *key_prefix);

/**
 * Provide the ai developer experience platform refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_refresh(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Write ai developer experience platform in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_ai_developer_experience_platform_save(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform restore operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_restore(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperRestoreReport *out_report);

/**
 * Provide the ai developer experience platform select pane operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_select_pane(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperPaneKind pane);

/**
 * Provide the ai developer experience platform select approval operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_select_approval(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id);

/**
 * Provide the ai developer experience platform approve operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_approve(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id,
    const char *approved_by);

/**
 * Provide the ai developer experience platform reject operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_reject(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *approval_id);

/**
 * Provide the ai developer experience platform open patch operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_open_patch(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *patch_id);

/**
 * Provide the ai developer experience platform select patch file operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_select_patch_file(
    UmiAiDeveloperExperiencePlatform *platform,
    size_t file_index);

/**
 * Provide the ai developer experience platform mark patch file reviewed operation used by
 * this module and its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_mark_patch_file_reviewed(
    UmiAiDeveloperExperiencePlatform *platform,
    size_t file_index,
    int reviewed);

/**
 * Provide the ai developer experience platform snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_experience_platform_snapshot(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperExperienceSnapshot *out_snapshot);

/**
 * Provide the ai developer experience platform approvals operation used by this module and
 * its client applications.
 */
UmiAiDeveloperApprovalService *
umi_ai_developer_experience_platform_approvals(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform tasks operation used by this module and its
 * client applications.
 */
UmiAiDeveloperTaskRegistry *
umi_ai_developer_experience_platform_tasks(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform timeline operation used by this module and
 * its client applications.
 */
UmiAiDeveloperTimeline *
umi_ai_developer_experience_platform_timeline(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform review operation used by this module and
 * its client applications.
 */
UmiAiDeveloperPatchReviewService *
umi_ai_developer_experience_platform_review(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform presentation operation used by this module
 * and its client applications.
 */
UmiAiDeveloperPresentationState *
umi_ai_developer_experience_platform_presentation(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform preferences operation used by this module
 * and its client applications.
 */
UmiAiDeveloperPreferences *
umi_ai_developer_experience_platform_preferences(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform coding operation used by this module and
 * its client applications.
 */
UmiAiCodingRuntimePlatform *
umi_ai_developer_experience_platform_coding(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer experience platform tools operation used by this module and its
 * client applications.
 */
UmiAiCodingToolsPlatform *
umi_ai_developer_experience_platform_tools(
    UmiAiDeveloperExperiencePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
