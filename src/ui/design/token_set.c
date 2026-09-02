/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/token_set.c
 *
 * PURPOSE:
 *   Maintain a bounded ordered set of typed design tokens with deterministic replacement semantics.
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

#include "umicom/ui/design/token_set.h"

#include <string.h>
/*
 * Initialise design token set from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_token_set_init(UmiDesignTokenSet *set,const char *id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(set,0,sizeof *set);s=umi_design_copy_text(set->id,sizeof set->id,id);return s;}
/*
 * Provide the design token set upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_design_token_set_upsert(UmiDesignTokenSet *set,const UmiDesignToken *token){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||!umi_design_token_valid(token))return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<set->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(set->tokens[i].id,token->id)==0){set->tokens[i]=*token;++set->revision;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(set->count>=UMI_DESIGN_MAX_TOKENS)return UMI_STATUS_CAPACITY_EXCEEDED;set->tokens[set->count]=*token;++set->count;++set->revision;return UMI_STATUS_OK;}
/*
 * Find design token set while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_design_token_set_find(const UmiDesignTokenSet *set,const char *token_id,UmiDesignToken *out_token){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||token_id==NULL||out_token==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<set->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(set->tokens[i].id,token_id)==0){*out_token=set->tokens[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
