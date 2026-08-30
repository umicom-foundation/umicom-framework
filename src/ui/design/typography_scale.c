/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/typography_scale.c
 *
 * PURPOSE:
 *   Maintain named typography levels for captions, body text, titles and display surfaces.
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

#include "umicom/ui/design/typography_scale.h"

#include <string.h>
UmiStatus umi_design_typography_scale_upsert(UmiDesignTypographyScale *scale,const char *level_id,const UmiDesignTypography *typography) { size_t i; UmiStatus s; if(scale==NULL||level_id==NULL||!umi_design_typography_valid(typography))return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<scale->count;++i)if(strcmp(scale->entries[i].level_id,level_id)==0){scale->entries[i].typography=*typography;return UMI_STATUS_OK;} if(scale->count>=UMI_DESIGN_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; s=umi_design_copy_text(scale->entries[scale->count].level_id,sizeof scale->entries[scale->count].level_id,level_id);if(s!=UMI_STATUS_OK)return s;scale->entries[scale->count].typography=*typography;++scale->count;return UMI_STATUS_OK;}
UmiStatus umi_design_typography_scale_find(const UmiDesignTypographyScale *scale,const char *level_id,UmiDesignTypography *out_typography){size_t i;if(scale==NULL||level_id==NULL||out_typography==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<scale->count;++i)if(strcmp(scale->entries[i].level_id,level_id)==0){*out_typography=scale->entries[i].typography;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
