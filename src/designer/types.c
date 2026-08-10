/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/types.c
 *
 * PURPOSE:
 *   Implement small designer enum helpers shared by history, inspection and Studio reporting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/types.h"
const char *umi_designer_operation_kind_text(UmiDesignerOperationKind k){switch(k){case UMI_DESIGNER_OP_ADD_COMPONENT:return "add-component";case UMI_DESIGNER_OP_REMOVE_COMPONENT:return "remove-component";case UMI_DESIGNER_OP_SET_PROPERTY:return "set-property";case UMI_DESIGNER_OP_MOVE_COMPONENT:return "move-component";default:return "unknown";}}
