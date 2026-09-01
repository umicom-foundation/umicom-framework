/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/builder_types.h
 * PURPOSE: Define Visual Application Builder v2 capacities and stable enums.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* These toolkit-neutral types describe authoring intent. GTK4,
 * Qt6, Wt and web frontends may render them without owning semantic state. */
#ifndef UMICOM_DESIGNER_BUILDER_TYPES_H
#define UMICOM_DESIGNER_BUILDER_TYPES_H

#include "umicom/designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_MAX_TREE_NODES UMI_DECL_MAX_COMPONENTS
#define UMI_DESIGNER_MAX_BINDINGS 128U
#define UMI_DESIGNER_MAX_INTERACTIONS 128U
#define UMI_DESIGNER_MAX_TRANSACTION_OPERATIONS 32U
#define UMI_DESIGNER_MAX_TRANSACTIONS 128U
#define UMI_DESIGNER_MAX_CLIPBOARD_NODES 64U
#define UMI_DESIGNER_MAX_TEMPLATES 64U
#define UMI_DESIGNER_MAX_GENERATED_FILES 4U
#define UMI_DESIGNER_GENERATED_CONTENT_CAPACITY 65536U

typedef enum UmiDesignerBindingMode {
    UMI_DESIGNER_BIND_ONE_WAY = 1,
    UMI_DESIGNER_BIND_TWO_WAY = 2,
    UMI_DESIGNER_BIND_ONE_TIME = 3
} UmiDesignerBindingMode;

typedef enum UmiDesignerInteractionKind {
    UMI_DESIGNER_INTERACTION_SIGNAL = 1,
    UMI_DESIGNER_INTERACTION_ACTION = 2,
    UMI_DESIGNER_INTERACTION_EVENT = 3,
    UMI_DESIGNER_INTERACTION_COMMAND = 4
} UmiDesignerInteractionKind;

typedef enum UmiDesignerDropPosition {
    UMI_DESIGNER_DROP_INTO = 1,
    UMI_DESIGNER_DROP_BEFORE = 2,
    UMI_DESIGNER_DROP_AFTER = 3
} UmiDesignerDropPosition;

typedef enum UmiDesignerTransactionState {
    UMI_DESIGNER_TRANSACTION_DRAFT = 1,
    UMI_DESIGNER_TRANSACTION_APPLIED = 2,
    UMI_DESIGNER_TRANSACTION_UNDONE = 3,
    UMI_DESIGNER_TRANSACTION_FAILED = 4
} UmiDesignerTransactionState;

typedef enum UmiDesignerPreviewHealth {
    UMI_DESIGNER_PREVIEW_IDLE = 1,
    UMI_DESIGNER_PREVIEW_CURRENT = 2,
    UMI_DESIGNER_PREVIEW_STALE = 3,
    UMI_DESIGNER_PREVIEW_INVALID = 4
} UmiDesignerPreviewHealth;

const char *umi_designer_binding_mode_text(UmiDesignerBindingMode mode);
const char *umi_designer_interaction_kind_text(
    UmiDesignerInteractionKind kind);
const char *umi_designer_drop_position_text(UmiDesignerDropPosition position);
const char *umi_designer_transaction_state_text(
    UmiDesignerTransactionState state);
const char *umi_designer_preview_health_text(UmiDesignerPreviewHealth health);

#ifdef __cplusplus
}
#endif

#endif
