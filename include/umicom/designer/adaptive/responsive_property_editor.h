/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/responsive_property_editor.h
 *
 * PURPOSE:
 *   Edit variant-scoped property overrides while preserving canonical base-property values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_RESPONSIVE_PROPERTY_EDITOR_H
#define UMICOM_DESIGNER_ADAPTIVE_RESPONSIVE_PROPERTY_EDITOR_H
#include "umicom/designer/adaptive/adaptive_property.h"
#include "umicom/designer/adaptive/adaptive_override.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerResponsivePropertyEditor {
    UmiDesignerAdaptiveProperty property;
    UmiDesignerAdaptiveOverride override_value;
    int has_override;
    uint64_t revision;
} UmiDesignerResponsivePropertyEditor;
/* Initialise a responsive property editor from a base property. */
UmiStatus umi_designer_responsive_property_editor_init(UmiDesignerResponsivePropertyEditor *editor,
                                                       const UmiDesignerAdaptiveProperty *property);
/* Set a text override for one adaptive variant. */
UmiStatus umi_designer_responsive_property_editor_override(UmiDesignerResponsivePropertyEditor *editor,
                                                           const char *variant_id,
                                                           const char *text);

#ifdef __cplusplus
}
#endif
#endif
