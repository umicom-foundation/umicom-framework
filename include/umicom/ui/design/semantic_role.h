/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/semantic_role.h
 *
 * PURPOSE:
 *   Define visual intent roles shared by controls, analytics surfaces and application brands.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_SEMANTIC_ROLE_H
#define INCLUDE_UMICOM_UI_DESIGN_SEMANTIC_ROLE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignSemanticRole {
    UMI_DESIGN_ROLE_NEUTRAL = 0,
    UMI_DESIGN_ROLE_PRIMARY = 1,
    UMI_DESIGN_ROLE_SECONDARY = 2,
    UMI_DESIGN_ROLE_INFO = 3,
    UMI_DESIGN_ROLE_SUCCESS = 4,
    UMI_DESIGN_ROLE_WARNING = 5,
    UMI_DESIGN_ROLE_DANGER = 6,
    UMI_DESIGN_ROLE_ACCENT = 7
} UmiDesignSemanticRole;

/* Return a stable semantic name for the design enumeration value. */
const char *umi_design_semantic_role_name(UmiDesignSemanticRole value);

#ifdef __cplusplus
}
#endif

#endif
