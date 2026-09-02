/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/style_sheet.h
 *
 * PURPOSE:
 *   Maintain a bounded stylesheet of semantic component rules for renderer-independent style resolution.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_STYLE_SHEET_H
#define INCLUDE_UMICOM_UI_DESIGN_STYLE_SHEET_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/style_rule.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/style_rule.h"
/**
 * Represent the design style sheet data shared with callers of this public contract.
 */
typedef struct UmiDesignStyleSheet { char id[UMI_DESIGN_ID_CAPACITY]; UmiDesignStyleRule rules[UMI_DESIGN_MAX_ITEMS]; size_t count; } UmiDesignStyleSheet;
/* Append a validated component style rule to the bounded stylesheet. */
UmiStatus umi_design_style_sheet_add(UmiDesignStyleSheet *sheet, const UmiDesignStyleRule *rule);
/* Find the first exact semantic component rule. */
UmiStatus umi_design_style_sheet_find(const UmiDesignStyleSheet *sheet, UmiUiComponentKind kind, UmiDesignSemanticRole role, UmiDesignInteractionState state, UmiDesignStyleRule *out_rule);

#ifdef __cplusplus
}
#endif

#endif
