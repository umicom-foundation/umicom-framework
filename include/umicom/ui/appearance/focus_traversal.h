/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/focus_traversal.h
 *
 * PURPOSE:
 *   Maintain deterministic keyboard focus order independently of toolkit widget hierarchy.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_FOCUS_TRAVERSAL_H
#define UMICOM_UI_APPEARANCE_FOCUS_TRAVERSAL_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceFocusTraversal { char ids[UMI_APPEARANCE_MAX_FOCUS_ITEMS][UMI_APPEARANCE_ID_CAPACITY]; size_t count; } UmiAppearanceFocusTraversal;
/* Initialise an empty logical focus order. */
UmiStatus umi_appearance_focus_traversal_init(UmiAppearanceFocusTraversal *order);
/* Append one stable focus target identity. */
UmiStatus umi_appearance_focus_traversal_add(UmiAppearanceFocusTraversal *order,const char *id);
/* Resolve the next focus identity with optional wrapping. */
const char *umi_appearance_focus_traversal_next(const UmiAppearanceFocusTraversal *order,const char *current_id,int wrap);

#ifdef __cplusplus
}
#endif
#endif
