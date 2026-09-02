/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_typography_scale.c
 *
 * PURPOSE:
 *   Verify named typography scale registration and lookup.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignTypographyScale s={0};UmiDesignTypography t,o;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_design_typography_init(&t,"Sans",16.0,600U,1.5)!=UMI_STATUS_OK)return 1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_design_typography_scale_upsert(&s,"body",&t)!=UMI_STATUS_OK)return 2;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_design_typography_scale_find(&s,"body",&o)!=UMI_STATUS_OK)return 3;return o.size==16.0?0:4;}
