/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/authoring_session.h
 *
 * PURPOSE:
 *   Define an owned visual-authoring session combining signal/action bindings, property schemas, alignment operations, clipboard items and templates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract is toolkit-neutral and intended for reuse by Studio,
 * Trader, TMS and future Umicom applications without exposing GUI objects.
 */
#ifndef UMICOM_DESIGNER_AUTHORING_SESSION_H
#define UMICOM_DESIGNER_AUTHORING_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/designer/signal_binding.h"
#include "umicom/designer/action_binding.h"
#include "umicom/designer/property_schema.h"
#include "umicom/designer/alignment.h"
#include "umicom/designer/clipboard.h"
#include "umicom/designer/template_palette.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAuthoringSession UmiDesignerAuthoringSession;

typedef struct UmiDesignerAuthoringSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t signals_count;
    size_t actions_count;
    size_t properties_count;
    size_t alignments_count;
    size_t clipboard_count;
    size_t templates_count;
    uint64_t revision;
} UmiDesignerAuthoringSessionSnapshot;

UmiStatus umi_designer_authoring_session_create(UmiDesignerAuthoringSession **out_service);
void umi_designer_authoring_session_destroy(UmiDesignerAuthoringSession *service);
UmiStatus umi_designer_authoring_session_snapshot(const UmiDesignerAuthoringSession *service, UmiDesignerAuthoringSessionSnapshot *out_snapshot);
UmiDesignerSignalBindingRegistry *umi_designer_authoring_session_signals(UmiDesignerAuthoringSession *service);
UmiDesignerActionBindingRegistry *umi_designer_authoring_session_actions(UmiDesignerAuthoringSession *service);
UmiDesignerPropertySchemaRegistry *umi_designer_authoring_session_properties(UmiDesignerAuthoringSession *service);
UmiDesignerAlignmentRegistry *umi_designer_authoring_session_alignments(UmiDesignerAuthoringSession *service);
UmiDesignerClipboardItemRegistry *umi_designer_authoring_session_clipboard(UmiDesignerAuthoringSession *service);
UmiDesignerTemplatePaletteRegistry *umi_designer_authoring_session_templates(UmiDesignerAuthoringSession *service);

#ifdef __cplusplus
}
#endif
#endif
