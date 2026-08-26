/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/responsive_property_editor.c
 *
 * PURPOSE:
 *   Edit variant-scoped property overrides while preserving canonical base-property values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/responsive_property_editor.h"
#include <string.h>

/* Snapshot the base property so responsive overrides never mutate its persisted default. */
UmiStatus umi_designer_responsive_property_editor_init(UmiDesignerResponsivePropertyEditor *editor,
                                                       const UmiDesignerAdaptiveProperty *property)
{
    if(editor==NULL||property==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(editor,0,sizeof *editor); editor->property=*property; return UMI_STATUS_OK;
}
/* Store one explicit override and advance the property-editor revision. */
UmiStatus umi_designer_responsive_property_editor_override(UmiDesignerResponsivePropertyEditor *editor,
                                                           const char *variant_id,
                                                           const char *text)
{
    UmiStatus status;
    if(editor==NULL||variant_id==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_designer_adaptive_override_init(&editor->override_value,variant_id,editor->property.property_path,text,UMI_DESIGNER_ADAPTIVE_OVERRIDE);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    editor->has_override = 1;
    ++editor->revision;
    return UMI_STATUS_OK;
}
