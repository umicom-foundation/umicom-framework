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
