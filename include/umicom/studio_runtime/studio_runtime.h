/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/studio_runtime.h
 *
 * PURPOSE:
 *   Aggregate the complete Framework-owned Umicom Studio runtime contract:
 *   Application Shell composition, command aliases/activation, cross-domain
 *   selections, document/editor synchronization, semantic layouts/sessions,
 *   status/badges, command search, contract closure and thin host adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_H
#define UMICOM_STUDIO_RUNTIME_H

#include "umicom/studio_runtime/types.h"
#include "umicom/studio_runtime/bindings.h"
#include "umicom/studio_runtime/surface_binding.h"
#include "umicom/studio_runtime/surface_profiles/profiles.h"
#include "umicom/studio_runtime/surface_catalogue.h"
#include "umicom/studio_runtime/activation.h"
#include "umicom/studio_runtime/activation_profiles/profiles.h"
#include "umicom/studio_runtime/activation_catalogue.h"
#include "umicom/studio_runtime/surface_activator.h"
#include "umicom/studio_runtime/command_execution.h"
#include "umicom/studio_runtime/command_state_sync.h"
#include "umicom/studio_runtime/command_alias.h"
#include "umicom/studio_runtime/command_aliases/aliases.h"
#include "umicom/studio_runtime/command_alias_catalogue.h"
#include "umicom/studio_runtime/command_alias_registry.h"
#include "umicom/studio_runtime/selection_state.h"
#include "umicom/studio_runtime/selection_router.h"
#include "umicom/studio_runtime/ai_context_sync.h"
#include "umicom/studio_runtime/document_state.h"
#include "umicom/studio_runtime/document_tabs.h"
#include "umicom/studio_runtime/window_title.h"
#include "umicom/studio_runtime/document_sync.h"
#include "umicom/studio_runtime/close_guard.h"
#include "umicom/studio_runtime/status_model.h"
#include "umicom/studio_runtime/status_sync.h"
#include "umicom/studio_runtime/badge_sync.h"
#include "umicom/studio_runtime/layout_preset.h"
#include "umicom/studio_runtime/layout_presets/presets.h"
#include "umicom/studio_runtime/layout_catalogue.h"
#include "umicom/studio_runtime/layout_session.h"
#include "umicom/studio_runtime/command_search.h"
#include "umicom/studio_runtime/contract.h"
#include "umicom/studio_runtime/closure.h"
#include "umicom/studio_runtime/platform.h"
#include "umicom/studio_runtime/session_controller.h"
#include "umicom/studio_runtime/runtime_commands.h"
#include "umicom/studio_runtime/bootstrap.h"
#include "umicom/studio_runtime/host_adapter.h"
#include "umicom/studio_runtime/host_sync.h"
#include "umicom/studio_runtime/host_controller.h"
#include "umicom/studio_runtime/view_helpers.h"
#include "umicom/studio_runtime/view_ids.h"
#include "umicom/studio_runtime/views/views.h"
#include "umicom/studio_runtime/view_factory.h"

#endif
