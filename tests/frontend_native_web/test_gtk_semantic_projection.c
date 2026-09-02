/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_gtk_semantic_projection.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web gtk semantic projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/gtk_semantic_projection.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebGtkProjection p; CHECK(umi_native_web_gtk_semantic_projection("gtk4.workbench","Umicom.Ui.Workbench",&p)==UMI_STATUS_OK); CHECK(strcmp(p.web_css_class,"umicom-workbench")==0); CHECK(!p.native_widget_serialized);
    return 0;
}
