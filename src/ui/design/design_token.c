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
static UmiStatus init_id(UmiDesignToken *token,const char *id,UmiDesignValueKind kind){UmiStatus s;if(token==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(token,0,sizeof *token);s=umi_design_copy_text(token->id,sizeof token->id,id);if(s!=UMI_STATUS_OK)return s;token->kind=kind;return UMI_STATUS_OK;}
UmiStatus umi_design_token_number(UmiDesignToken *token,const char *id,double value){UmiStatus s;if(!umi_design_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;s=init_id(token,id,UMI_DESIGN_VALUE_NUMBER);if(s!=UMI_STATUS_OK)return s;token->number=value;return UMI_STATUS_OK;}
UmiStatus umi_design_token_color(UmiDesignToken *token,const char *id,UmiDesignRgba color){UmiStatus s;if(!umi_design_color_valid(color))return UMI_STATUS_INVALID_ARGUMENT;s=init_id(token,id,UMI_DESIGN_VALUE_COLOR);if(s!=UMI_STATUS_OK)return s;token->color=color;return UMI_STATUS_OK;}
UmiStatus umi_design_token_text(UmiDesignToken *token,const char *id,const char *value){UmiStatus s=init_id(token,id,UMI_DESIGN_VALUE_TEXT);if(s!=UMI_STATUS_OK)return s;return umi_design_copy_text(token->text,sizeof token->text,value);}
int umi_design_token_valid(const UmiDesignToken *token){if(token==NULL||token->id[0]=='\0')return 0;switch(token->kind){case UMI_DESIGN_VALUE_COLOR:return umi_design_color_valid(token->color);case UMI_DESIGN_VALUE_NUMBER:return umi_design_number_valid(token->number);case UMI_DESIGN_VALUE_TEXT:return token->text[0]!='\0';case UMI_DESIGN_VALUE_INTEGER:case UMI_DESIGN_VALUE_LENGTH:case UMI_DESIGN_VALUE_DURATION:return 1;default:return 0;}}
