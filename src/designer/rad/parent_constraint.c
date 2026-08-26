/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/parent_constraint.c
 *
 * PURPOSE:
 *   Describe which semantic component families a parent slot accepts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/parent_constraint.h"
#include <string.h>
UmiStatus umi_rad_parent_constraint_init(UmiRadParentConstraint *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_type, sizeof item->parent_type, "parent_constraint");
    (void)umi_rad_copy_text(item->child_family, sizeof item->child_family, "parent_constraint");
    item->accepted = true;
    return UMI_STATUS_OK;
}
int umi_rad_parent_constraint_is_valid(const UmiRadParentConstraint *item){if(item==NULL)return 0;return umi_rad_id_valid(item->parent_type) && umi_rad_id_valid(item->child_family);}
