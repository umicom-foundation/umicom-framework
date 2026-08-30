/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/style_rule.h
 *
 * PURPOSE:
 *   Bind semantic component kind, intent and interaction state to a bounded set of style values.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_STYLE_RULE_H
#define INCLUDE_UMICOM_UI_DESIGN_STYLE_RULE_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/style_value.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/interaction_state.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/style_value.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/interaction_state.h"
typedef struct UmiDesignStyleRule { UmiUiComponentKind component_kind; UmiDesignSemanticRole role; UmiDesignInteractionState state; UmiDesignStyleValue values[UMI_DESIGN_MAX_VALUES]; size_t count; } UmiDesignStyleRule;
/* Add or replace a style property inside a semantic component rule. */
UmiStatus umi_design_style_rule_upsert(UmiDesignStyleRule *rule, const UmiDesignStyleValue *value);
/* Resolve a style property from one rule. */
UmiStatus umi_design_style_rule_find(const UmiDesignStyleRule *rule, const char *property_id, UmiDesignStyleValue *out_value);

#ifdef __cplusplus
}
#endif

#endif
