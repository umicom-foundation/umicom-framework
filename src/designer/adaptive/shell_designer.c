/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/shell_designer.c
 *
 * PURPOSE:
 *   Own designer selection and dirty-state for adaptive application-shell regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/shell_designer.h"
#include <string.h>

/* Copy the profile by value so edit sessions can remain transactional. */
UmiStatus umi_designer_shell_designer_init(UmiDesignerShellDesigner *designer,
                                           const UmiAdaptiveShellProfile *profile)
{
    if(designer==NULL||profile==NULL||!umi_adaptive_shell_profile_valid(profile))return UMI_STATUS_INVALID_ARGUMENT;
    memset(designer,0,sizeof *designer); designer->profile=*profile; return UMI_STATUS_OK;
}
/* Select only regions that exist in the authored shell layout. */
UmiStatus umi_designer_shell_designer_select_region(UmiDesignerShellDesigner *designer,
                                                    const char *region_id)
{
    if(designer==NULL||region_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_adaptive_shell_layout_find(&designer->profile.layout,region_id)==NULL)return UMI_STATUS_NOT_FOUND;
    if(umi_designer_adaptive_copy_text(designer->selected_region_id,sizeof designer->selected_region_id,region_id)!=UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++designer->revision; designer->dirty=1; return UMI_STATUS_OK;
}
