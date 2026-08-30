/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_token_set.c
 *
 * PURPOSE:
 *   Verify design-token set replacement, lookup and revision tracking.
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

int main(void){UmiDesignTokenSet s;UmiDesignToken t,o;if(umi_design_token_set_init(&s,"base")!=UMI_STATUS_OK)return 1;if(umi_design_token_number(&t,"space",8.0)!=UMI_STATUS_OK)return 2;if(umi_design_token_set_upsert(&s,&t)!=UMI_STATUS_OK)return 3;if(umi_design_token_set_find(&s,"space",&o)!=UMI_STATUS_OK)return 4;return o.number==8.0&&s.revision==1U?0:5;}
