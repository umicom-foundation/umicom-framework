/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/responsive_property_editor.c
 *
 * PURPOSE:
 *   Edit variant-scoped property overrides while preserving canonical base-property values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/designer/adaptive/responsive_property_editor.h"
#include <string.h>

/* Snapshot the base property so responsive overrides never mutate its persisted default. */
UmiStatus umi_designer_responsive_property_editor_init(UmiDesignerResponsivePropertyEditor *editor,
                                                       const UmiDesignerAdaptiveProperty *property)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(editor==NULL||property==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(editor,0,sizeof *editor); editor->property=*property; return UMI_STATUS_OK;
}
/* Store one explicit override and advance the property-editor revision. */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_designer_responsive_property_editor_override(UmiDesignerResponsivePropertyEditor *editor,
                                                           const char *variant_id,
                                                           const char *text)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(editor==NULL||variant_id==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_designer_adaptive_override_init(&editor->override_value,variant_id,editor->property.property_path,text,UMI_DESIGNER_ADAPTIVE_OVERRIDE);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    editor->has_override = 1;
    ++editor->revision;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_designer_responsive_property_editor_override(UmiDesignerResponsivePropertyEditor *editor,
                                                           const char *variant_id,
                                                           const char *text)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (editor != NULL && editor->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(editor==NULL||variant_id==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_designer_adaptive_override_init(&editor->override_value,variant_id,editor->property.property_path,text,UMI_DESIGNER_ADAPTIVE_OVERRIDE);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    editor->has_override = 1;
    ++editor->revision;
    return UMI_STATUS_OK;
}
