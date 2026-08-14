/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/property_editor_v2.h
 * PURPOSE: Edit typed component properties with schema validation and undo.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: A draft separates typing and validation from mutation. The
 * document changes only when a valid draft is committed through history. */
#ifndef UMICOM_DESIGNER_PROPERTY_EDITOR_V2_H
#define UMICOM_DESIGNER_PROPERTY_EDITOR_V2_H

#include "umicom/designer/history.h"
#include "umicom/designer/inspector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerPropertyDraftV2 {
    char node_id[UMI_DECL_ID_CAPACITY];
    char component_type[UMI_DECL_ID_CAPACITY];
    UmiDeclPropertyDescriptor descriptor;
    char original_value[UMI_DECL_TEXT_CAPACITY];
    char value[UMI_DECL_TEXT_CAPACITY];
    UmiStatus validation_status;
    int valid;
    int changed;
} UmiDesignerPropertyDraftV2;

UmiStatus umi_designer_property_editor_v2_begin(
    const UmiDesignerDocument *document,
    const UmiDeclComponentRegistry *registry,
    const char *node_id,
    const char *property_name,
    UmiDesignerPropertyDraftV2 *out_draft);
UmiStatus umi_designer_property_editor_v2_set(
    UmiDesignerPropertyDraftV2 *draft,
    const char *value_text);
UmiStatus umi_designer_property_editor_v2_commit(
    UmiDesignerHistory *history,
    UmiDesignerDocument *document,
    const UmiDesignerPropertyDraftV2 *draft);

#ifdef __cplusplus
}
#endif

#endif
