/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_token_resolver.c
 *
 * PURPOSE:
 *   Verify layered token resolution honours component precedence.
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

int main(void){UmiDesignTokenSet base,app;UmiDesignToken a,b,o;UmiDesignTokenResolver r;uint8_t layer=0U;if(umi_design_token_set_init(&base,"base")!=UMI_STATUS_OK)return 1;if(umi_design_token_set_init(&app,"app")!=UMI_STATUS_OK)return 2;if(umi_design_token_number(&a,"gap",4.0)!=UMI_STATUS_OK)return 3;if(umi_design_token_number(&b,"gap",8.0)!=UMI_STATUS_OK)return 4;if(umi_design_token_set_upsert(&base,&a)!=UMI_STATUS_OK||umi_design_token_set_upsert(&app,&b)!=UMI_STATUS_OK)return 5;r.system_tokens=&base;r.application_tokens=&app;r.component_tokens=NULL;if(umi_design_token_resolver_resolve(&r,"gap",&o,&layer)!=UMI_STATUS_OK)return 6;return o.number==8.0&&layer==2U?0:7;}
