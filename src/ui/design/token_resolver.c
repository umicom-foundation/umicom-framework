/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/token_resolver.c
 *
 * PURPOSE:
 *   Resolve design tokens through component, application and system layers with explicit precedence.
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

#include "umicom/ui/design/token_resolver.h"

UmiStatus umi_design_token_resolver_resolve(const UmiDesignTokenResolver *resolver,const char *token_id,UmiDesignToken *out_token,uint8_t *out_layer){UmiStatus s;if(resolver==NULL||token_id==NULL||out_token==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(resolver->component_tokens!=NULL){s=umi_design_token_set_find(resolver->component_tokens,token_id,out_token);if(s==UMI_STATUS_OK){if(out_layer!=NULL)*out_layer=3U;return s;}}if(resolver->application_tokens!=NULL){s=umi_design_token_set_find(resolver->application_tokens,token_id,out_token);if(s==UMI_STATUS_OK){if(out_layer!=NULL)*out_layer=2U;return s;}}if(resolver->system_tokens!=NULL){s=umi_design_token_set_find(resolver->system_tokens,token_id,out_token);if(s==UMI_STATUS_OK){if(out_layer!=NULL)*out_layer=1U;return s;}}return UMI_STATUS_NOT_FOUND;}
