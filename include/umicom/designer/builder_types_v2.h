/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/builder_types_v2.h
 * PURPOSE: Define Visual Application Builder v2 capacities and stable enums.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: These toolkit-neutral types describe authoring intent. GTK4,
 * Qt6, Wt and web frontends may render them without owning semantic state. */
#ifndef UMICOM_DESIGNER_BUILDER_TYPES_V2_H
#define UMICOM_DESIGNER_BUILDER_TYPES_V2_H

#include "umicom/designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_V2_MAX_TREE_NODES UMI_DECL_MAX_COMPONENTS
#define UMI_DESIGNER_V2_MAX_BINDINGS 128U
#define UMI_DESIGNER_V2_MAX_INTERACTIONS 128U
#define UMI_DESIGNER_V2_MAX_TRANSACTION_OPERATIONS 32U
#define UMI_DESIGNER_V2_MAX_TRANSACTIONS 128U
#define UMI_DESIGNER_V2_MAX_CLIPBOARD_NODES 64U
#define UMI_DESIGNER_V2_MAX_TEMPLATES 64U
#define UMI_DESIGNER_V2_MAX_GENERATED_FILES 4U
#define UMI_DESIGNER_V2_GENERATED_CONTENT_CAPACITY 65536U

typedef enum UmiDesignerBindingModeV2 {
    UMI_DESIGNER_BIND_ONE_WAY = 1,
    UMI_DESIGNER_BIND_TWO_WAY = 2,
    UMI_DESIGNER_BIND_ONE_TIME = 3
} UmiDesignerBindingModeV2;

typedef enum UmiDesignerInteractionKindV2 {
    UMI_DESIGNER_INTERACTION_SIGNAL = 1,
    UMI_DESIGNER_INTERACTION_ACTION = 2,
    UMI_DESIGNER_INTERACTION_EVENT = 3,
    UMI_DESIGNER_INTERACTION_COMMAND = 4
} UmiDesignerInteractionKindV2;

typedef enum UmiDesignerDropPositionV2 {
    UMI_DESIGNER_DROP_INTO = 1,
    UMI_DESIGNER_DROP_BEFORE = 2,
    UMI_DESIGNER_DROP_AFTER = 3
} UmiDesignerDropPositionV2;

typedef enum UmiDesignerTransactionStateV2 {
    UMI_DESIGNER_TRANSACTION_DRAFT = 1,
    UMI_DESIGNER_TRANSACTION_APPLIED = 2,
    UMI_DESIGNER_TRANSACTION_UNDONE = 3,
    UMI_DESIGNER_TRANSACTION_FAILED = 4
} UmiDesignerTransactionStateV2;

typedef enum UmiDesignerPreviewHealthV2 {
    UMI_DESIGNER_PREVIEW_IDLE = 1,
    UMI_DESIGNER_PREVIEW_CURRENT = 2,
    UMI_DESIGNER_PREVIEW_STALE = 3,
    UMI_DESIGNER_PREVIEW_INVALID = 4
} UmiDesignerPreviewHealthV2;

const char *umi_designer_binding_mode_v2_text(UmiDesignerBindingModeV2 mode);
const char *umi_designer_interaction_kind_v2_text(
    UmiDesignerInteractionKindV2 kind);
const char *umi_designer_drop_position_v2_text(UmiDesignerDropPositionV2 position);
const char *umi_designer_transaction_state_v2_text(
    UmiDesignerTransactionStateV2 state);
const char *umi_designer_preview_health_v2_text(UmiDesignerPreviewHealthV2 health);

#ifdef __cplusplus
}
#endif

#endif
