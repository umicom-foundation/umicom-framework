/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_gtk4_widget_catalogue.c
 *
 * PURPOSE:
 *   Keep GTK4 widget coverage complete and inspectable in headless builds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/ui/gtk4/widget_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiGtk4CoverageReport report = umi_gtk4_widget_coverage();
    const UmiGtk4WidgetDefinition *window =
        umi_gtk4_widget_catalogue_find("GtkApplicationWindow");
    const UmiGtk4WidgetDefinition *graphics =
        umi_gtk4_widget_catalogue_find("GtkGraphicsOffload");
    const UmiGtk4WidgetDefinition *legacy =
        umi_gtk4_widget_catalogue_find("GtkTreeView");
    assert(report.complete);
    assert(report.widget_count >= 90U);
    assert(report.semantic_factory_count >= 20U);
    assert(report.builder_native_count > report.semantic_factory_count);
    assert(report.minimum_major == 4U && report.minimum_minor == 10U);
    assert(window != NULL &&
           window->exposure == UMI_GTK4_EXPOSURE_SEMANTIC_FACTORY);
    assert(graphics != NULL && graphics->introduced_minor == 14U);
    assert(legacy != NULL && legacy->deprecated_minor == 10U);
    return 0;
}
