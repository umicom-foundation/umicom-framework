/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/design_token.c
 *
 * PURPOSE:
 *   Define typed design tokens that extend the existing generic Umicom theme-token model.
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

#include "umicom/ui/design/design_token.h"

#include <string.h>
/* Provide the init id operation used by this module and its client applications. */
static UmiStatus init_id(UmiDesignToken *token,const char *id,UmiDesignValueKind kind){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(token==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(token,0,sizeof *token);s=umi_design_copy_text(token->id,sizeof token->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;token->kind=kind;return UMI_STATUS_OK;}
/*
 * Provide the design token number operation used by this module and its client
 * applications.
 */
UmiStatus umi_design_token_number(UmiDesignToken *token,const char *id,double value){UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_design_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;s=init_id(token,id,UMI_DESIGN_VALUE_NUMBER);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;token->number=value;return UMI_STATUS_OK;}
/*
 * Provide the design token color operation used by this module and its client
 * applications.
 */
UmiStatus umi_design_token_color(UmiDesignToken *token,const char *id,UmiDesignRgba color){UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_design_color_valid(color))return UMI_STATUS_INVALID_ARGUMENT;s=init_id(token,id,UMI_DESIGN_VALUE_COLOR);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;token->color=color;return UMI_STATUS_OK;}
/* Provide the design token text operation used by this module and its client applications. */
UmiStatus umi_design_token_text(UmiDesignToken *token,const char *id,const char *value){UmiStatus s=init_id(token,id,UMI_DESIGN_VALUE_TEXT);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;return umi_design_copy_text(token->text,sizeof token->text,value);}
/* Check that design token satisfies its contract before another service relies on it. */
int umi_design_token_valid(const UmiDesignToken *token){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(token==NULL||token->id[0]=='\0')return 0;/* Select the behaviour associated with the requested command or state value. */ switch(token->kind){case UMI_DESIGN_VALUE_COLOR:return umi_design_color_valid(token->color);case UMI_DESIGN_VALUE_NUMBER:return umi_design_number_valid(token->number);case UMI_DESIGN_VALUE_TEXT:return token->text[0]!='\0';case UMI_DESIGN_VALUE_INTEGER:case UMI_DESIGN_VALUE_LENGTH:case UMI_DESIGN_VALUE_DURATION:return 1;default:return 0;}}
