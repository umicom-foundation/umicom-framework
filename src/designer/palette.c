/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/palette.c
 *
 * PURPOSE:
 *   Build designer palette views from semantic metadata instead of hard-coded GTK widget lists.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/palette.h"
#include <stdlib.h>
#include <string.h>
UmiStatus umi_designer_palette_build(const UmiDeclComponentRegistry *r,const char *filter,UmiDesignerPalette *out)
{
    size_t i;
    size_t matches=0U;
    if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out,0,sizeof(*out));
    for(i=0U;i<umi_decl_component_registry_count(r);++i){UmiDeclComponentDescriptor d;if(umi_decl_component_registry_at(r,i,&d)!=UMI_STATUS_OK)continue;if(filter==NULL||filter[0]=='\0'||strcmp(filter,d.category)==0)matches+=1U;}
    if(matches==0U)return UMI_STATUS_OK;
    out->items=(UmiDeclComponentDescriptor *)calloc(matches,sizeof(*out->items));
    if(out->items==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    for(i=0U;i<umi_decl_component_registry_count(r);++i){UmiDeclComponentDescriptor d;if(umi_decl_component_registry_at(r,i,&d)!=UMI_STATUS_OK)continue;if(filter!=NULL&&filter[0]!='\0'&&strcmp(filter,d.category)!=0)continue;out->items[out->count++]=d;}
    return UMI_STATUS_OK;
}

void umi_designer_palette_dispose(UmiDesignerPalette *palette)
{
    if(palette==NULL)return;
    free(palette->items);
    palette->items=NULL;
    palette->count=0U;
}
