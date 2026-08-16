/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editor.h
 *
 * PURPOSE:
 *   Aggregate the reusable Umicom editor platform contracts for IDEs and other text-centric applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_EDITOR_EDITOR_H
#define UMICOM_EDITOR_EDITOR_H
#include "umicom/editor/feature.h"
#include "umicom/editor/features.h"
#include "umicom/editor/text_buffer.h"
#include "umicom/editor/line_index.h"
#include "umicom/editor/search_engine.h"
#include "umicom/editor/edit_history.h"
#include "umicom/editor/edit_command.h"
#include "umicom/editor/edit_engine.h"
#include "umicom/editor/source_location.h"
#include "umicom/editor/navigation_history.h"
#include "umicom/editor/navigation_result.h"
#include "umicom/editor/source_bookmark.h"
#include "umicom/editor/symbol_index.h"
#include "umicom/editor/symbol_relationship.h"
#include "umicom/editor/document_outline.h"
#include "umicom/editor/breadcrumb_model.h"
#include "umicom/editor/source_navigation.h"
#include "umicom/editor/navigation_command.h"
#include "umicom/editor/peek_definition.h"
#include "umicom/editor/reference_results.h"
#include "umicom/editor/symbol_hierarchy.h"
#include "umicom/editor/call_hierarchy.h"
#include "umicom/editor/type_hierarchy.h"
#include "umicom/editor/navigation_insights.h"
#include "umicom/editor/workspace_edit.h"
#include "umicom/editor/rename_symbol.h"
#include "umicom/editor/code_lens_model.h"
#include "umicom/editor/inlay_hint_model.h"
#include "umicom/editor/semantic_highlighting.h"
#include "umicom/editor/editor_intelligence.h"
#include "umicom/editor/intelligence_command.h"
#include "umicom/editor/hover_model.h"
#include "umicom/editor/signature_help_model.h"
#include "umicom/editor/parameter_information.h"
#include "umicom/editor/quick_documentation.h"
#include "umicom/editor/editor_assistance.h"
#include "umicom/editor/assistance_command.h"
#include "umicom/editor/formatting_profile.h"
#include "umicom/editor/formatting_session.h"
#include "umicom/editor/import_organisation.h"
#include "umicom/editor/snippet_session.h"
#include "umicom/editor/linked_editing_model.h"
#include "umicom/editor/editing_productivity.h"
#include "umicom/editor/productivity_command.h"
#include "umicom/editor/code_action_provider.h"
#include "umicom/editor/code_action_query.h"
#include "umicom/editor/refactoring_plan.h"
#include "umicom/editor/refactoring_preview.h"
#include "umicom/editor/edit_transaction.h"
#include "umicom/editor/code_action_orchestration.h"
#include "umicom/editor/refactoring_command.h"
#include "umicom/editor/text_scan.h"
#include "umicom/editor/document.h"
#include "umicom/editor/cursor.h"
#include "umicom/editor/selection_range.h"
#include "umicom/editor/marker.h"
#include "umicom/editor/fold_region.h"
#include "umicom/editor/symbol.h"
#include "umicom/editor/diagnostic.h"
#include "umicom/editor/completion.h"
#include "umicom/editor/code_action.h"
#include "umicom/editor/diff_hunk.h"
#include "umicom/editor/configuration.h"
#include "umicom/editor/presentation.h"
#include "umicom/editor/session.h"
#endif
