/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_style_value.c
 *
 * PURPOSE:
 *   Verify typed style-property construction.
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

#include "umicom/ui/design/style_value.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignToken t;UmiDesignStyleValue v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_number(&t,"space.2",8.0)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_style_value_init(&v,"padding",&t)!=UMI_STATUS_OK)return 2;return v.value.number==8.0?0:3;}
