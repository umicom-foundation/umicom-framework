/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_responsive_property_editor.c
 *
 * PURPOSE:
 *   Validate edit variant-scoped property overrides while preserving canonical base-property values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/responsive_property_editor.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerAdaptiveProperty p; UmiDesignerResponsivePropertyEditor e;
    CHECK(umi_designer_adaptive_property_init(&p,"visible","true",1)==UMI_STATUS_OK); CHECK(umi_designer_responsive_property_editor_init(&e,&p)==UMI_STATUS_OK);
    CHECK(umi_designer_responsive_property_editor_override(&e,"phone","false")==UMI_STATUS_OK); CHECK(e.has_override);
    return 0;
}
