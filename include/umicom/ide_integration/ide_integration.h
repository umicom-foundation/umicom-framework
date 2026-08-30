/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/ide_integration.h
 *
 * PURPOSE:
 *   Aggregate the Framework cross-domain IDE integration platform: canonical
 *   service bindings, editor/document navigation, Problems/Tests/VCS/Debug/
 *   language bridges, inline AI, workflow/self-host gates, commands, surfaces
 *   and toolkit-neutral integration views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_H
#define UMICOM_IDE_INTEGRATION_H

#include "umicom/ide_integration/types.h"
#include "umicom/ide_integration/location.h"
#include "umicom/ide_integration/uri.h"
#include "umicom/ide_integration/bindings.h"
#include "umicom/ide_integration/active_context.h"
#include "umicom/ide_integration/productivity_bridge.h"
#include "umicom/ide_integration/problem_bridge.h"
#include "umicom/ide_integration/test_bridge.h"
#include "umicom/ide_integration/language_bridge.h"
#include "umicom/ide_integration/source_control_bridge.h"
#include "umicom/ide_integration/debug_bridge.h"
#include "umicom/ide_integration/document_bridge.h"
#include "umicom/ide_integration/navigation_history.h"
#include "umicom/ide_integration/cross_navigation.h"
#include "umicom/ide_integration/selection.h"
#include "umicom/ide_integration/ai_editor_bridge.h"
#include "umicom/ide_integration/edit_adapter.h"
#include "umicom/ide_integration/inline_executor.h"
#include "umicom/ide_integration/inline_history.h"
#include "umicom/ide_integration/inline_controller.h"
#include "umicom/ide_integration/workflow_policy.h"
#include "umicom/ide_integration/workflow_report.h"
#include "umicom/ide_integration/workflow_evaluator.h"
#include "umicom/ide_integration/workflow_service.h"
#include "umicom/ide_integration/workflow_profiles/profiles.h"
#include "umicom/ide_integration/surface.h"
#include "umicom/ide_integration/surface_profile.h"
#include "umicom/ide_integration/surface_profiles/profiles.h"
#include "umicom/ide_integration/builtin_surfaces.h"
#include "umicom/ide_integration/perspective.h"
#include "umicom/ide_integration/self_host_manifest.h"
#include "umicom/ide_integration/self_host_checker.h"
#include "umicom/ide_integration/platform.h"
#include "umicom/ide_integration/ai_action_bridge.h"
#include "umicom/ide_integration/command.h"
#include "umicom/ide_integration/command_context.h"
#include "umicom/ide_integration/command_router.h"
#include "umicom/ide_integration/command_registry_bridge.h"
#include "umicom/ide_integration/view_helpers.h"
#include "umicom/ide_integration/view_ids.h"
#include "umicom/ide_integration/views/views.h"
#include "umicom/ide_integration/view_factory.h"

#endif
