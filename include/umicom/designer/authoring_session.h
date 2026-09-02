/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/authoring_session.h
 *
 * PURPOSE:
 *   Define an owned visual-authoring session combining signal/action bindings, property schemas, alignment operations, clipboard items and templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the designer authoring session data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerAuthoringSession UmiDesignerAuthoringSession;

/**
 * Represent the designer authoring session snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise designer authoring session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_authoring_session_create(UmiDesignerAuthoringSession **out_service);
/**
 * Release or reset state held by designer authoring session so the same storage can be
 * reused safely.
 */
void umi_designer_authoring_session_destroy(UmiDesignerAuthoringSession *service);
/**
 * Provide the designer authoring session snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_authoring_session_snapshot(const UmiDesignerAuthoringSession *service, UmiDesignerAuthoringSessionSnapshot *out_snapshot);
/**
 * Provide the designer authoring session signals operation used by this module and its
 * client applications.
 */
UmiDesignerSignalBindingRegistry *umi_designer_authoring_session_signals(UmiDesignerAuthoringSession *service);
/**
 * Provide the designer authoring session actions operation used by this module and its
 * client applications.
 */
UmiDesignerActionBindingRegistry *umi_designer_authoring_session_actions(UmiDesignerAuthoringSession *service);
/**
 * Provide the designer authoring session properties operation used by this module and its
 * client applications.
 */
UmiDesignerPropertySchemaRegistry *umi_designer_authoring_session_properties(UmiDesignerAuthoringSession *service);
/**
 * Provide the designer authoring session alignments operation used by this module and its
 * client applications.
 */
UmiDesignerAlignmentRegistry *umi_designer_authoring_session_alignments(UmiDesignerAuthoringSession *service);
/**
 * Provide the designer authoring session clipboard operation used by this module and its
 * client applications.
 */
UmiDesignerClipboardItemRegistry *umi_designer_authoring_session_clipboard(UmiDesignerAuthoringSession *service);
/**
 * Provide the designer authoring session templates operation used by this module and its
 * client applications.
 */
UmiDesignerTemplatePaletteRegistry *umi_designer_authoring_session_templates(UmiDesignerAuthoringSession *service);

#ifdef __cplusplus
}
#endif
#endif
