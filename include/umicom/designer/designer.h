/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/designer.h
 *
 * PURPOSE:
 *   Provide one aggregate include for the reusable semantic visual-designer engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_DESIGNER_H
#define UMICOM_DESIGNER_DESIGNER_H
#include "umicom/designer/types.h"
#include "umicom/designer/document.h"
#include "umicom/designer/operation.h"
#include "umicom/designer/history.h"
#include "umicom/designer/selection.h"
#include "umicom/designer/palette.h"
#include "umicom/designer/inspector.h"
#include "umicom/designer/surface.h"
#include "umicom/designer/preview.h"
#include "umicom/designer/project.h"
#include "umicom/designer/catalogue.h"
#include "umicom/designer/command.h"
#include "umicom/designer/signal_binding.h"
#include "umicom/designer/action_binding.h"
#include "umicom/designer/property_schema.h"
#include "umicom/designer/alignment.h"
#include "umicom/designer/clipboard.h"
#include "umicom/designer/template_palette.h"
#include "umicom/designer/authoring_session.h"
#include "umicom/designer/builder_types.h"
#include "umicom/designer/component_tree.h"
#include "umicom/designer/property_editor.h"
#include "umicom/designer/binding_editor.h"
#include "umicom/designer/interaction_editor.h"
#include "umicom/designer/drag_drop.h"
#include "umicom/designer/transaction.h"
#include "umicom/designer/semantic_clipboard.h"
#include "umicom/designer/template.h"
#include "umicom/designer/layout.h"
#include "umicom/designer/live_preview.h"
#include "umicom/designer/source_generation.h"
#include "umicom/designer/builder_session.h"
#endif
