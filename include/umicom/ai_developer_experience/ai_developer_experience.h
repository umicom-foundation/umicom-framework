/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/ai_developer_experience.h
 *
 * PURPOSE:
 *   Aggregate the complete toolkit-neutral AI Developer Experience platform:
 *   approvals, governed review/diff, tasks, timeline, durable state, commands,
 *   coding chat and reusable IDE view models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_H

#include "umicom/ai_developer_experience/types.h"
#include "umicom/ai_developer_experience/preferences.h"
#include "umicom/ai_developer_experience/presentation_state.h"
#include "umicom/ai_developer_experience/approval_queue.h"
#include "umicom/ai_developer_experience/approval_service.h"
#include "umicom/ai_developer_experience/diff.h"
#include "umicom/ai_developer_experience/diff_builder.h"
#include "umicom/ai_developer_experience/diff_hunks.h"
#include "umicom/ai_developer_experience/diff_navigation.h"
#include "umicom/ai_developer_experience/side_by_side.h"
#include "umicom/ai_developer_experience/patch_review.h"
#include "umicom/ai_developer_experience/patch_review_service.h"
#include "umicom/ai_developer_experience/task_registry.h"
#include "umicom/ai_developer_experience/task_projection.h"
#include "umicom/ai_developer_experience/task_controller.h"
#include "umicom/ai_developer_experience/timeline.h"
#include "umicom/ai_developer_experience/timeline_projection.h"
#include "umicom/ai_developer_experience/activity_sync.h"
#include "umicom/ai_developer_experience/persistence_codec.h"
#include "umicom/ai_developer_experience/durable_store.h"
#include "umicom/ai_developer_experience/chat_persistence.h"
#include "umicom/ai_developer_experience/task_persistence.h"
#include "umicom/ai_developer_experience/approval_persistence.h"
#include "umicom/ai_developer_experience/presentation_persistence.h"
#include "umicom/ai_developer_experience/preferences_persistence.h"
#include "umicom/ai_developer_experience/state_schema.h"
#include "umicom/ai_developer_experience/state_migration.h"
#include "umicom/ai_developer_experience/restore_report.h"
#include "umicom/ai_developer_experience/session_bundle.h"
#include "umicom/ai_developer_experience/view_ids.h"
#include "umicom/ai_developer_experience/action_ids.h"
#include "umicom/ai_developer_experience/view_helpers.h"
#include "umicom/ai_developer_experience/views/views.h"
#include "umicom/ai_developer_experience/view_factory.h"
#include "umicom/ai_developer_experience/perspective.h"
#include "umicom/ai_developer_experience/command.h"
#include "umicom/ai_developer_experience/command_context.h"
#include "umicom/ai_developer_experience/command_router.h"
#include "umicom/ai_developer_experience/command_registry_bridge.h"
#include "umicom/ai_developer_experience/tool_loop_bridge.h"
#include "umicom/ai_developer_experience/chat_controller.h"
#include "umicom/ai_developer_experience/platform.h"

#endif
