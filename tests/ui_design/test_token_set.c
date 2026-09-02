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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignTokenSet s;UmiDesignToken t,o;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_set_init(&s,"base")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_number(&t,"space",8.0)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_set_upsert(&s,&t)!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_set_find(&s,"space",&o)!=UMI_STATUS_OK)return 4;return o.number==8.0&&s.revision==1U?0:5;}
