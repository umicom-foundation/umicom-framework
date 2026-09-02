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
/*
 * Provide the design typography scale upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_design_typography_scale_upsert(UmiDesignTypographyScale *scale,const char *level_id,const UmiDesignTypography *typography) { size_t i; UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(scale==NULL||level_id==NULL||!umi_design_typography_valid(typography))return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<scale->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(scale->entries[i].level_id,level_id)==0){scale->entries[i].typography=*typography;return UMI_STATUS_OK;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(scale->count>=UMI_DESIGN_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; s=umi_design_copy_text(scale->entries[scale->count].level_id,sizeof scale->entries[scale->count].level_id,level_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;scale->entries[scale->count].typography=*typography;++scale->count;return UMI_STATUS_OK;}
/*
 * Find design typography scale while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_design_typography_scale_find(const UmiDesignTypographyScale *scale,const char *level_id,UmiDesignTypography *out_typography){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(scale==NULL||level_id==NULL||out_typography==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<scale->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(scale->entries[i].level_id,level_id)==0){*out_typography=scale->entries[i].typography;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
