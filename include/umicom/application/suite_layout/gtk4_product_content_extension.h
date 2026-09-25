/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/gtk4_product_content_extension.h
 *
 * PURPOSE:
 *   Extend a product surface while retaining the canonical startup, branding, layouts and shutdown lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_PRODUCT_CONTENT_EXTENSION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_PRODUCT_CONTENT_EXTENSION_H
#include "umicom/application/suite_layout/gtk4_product_application.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Build receives the existing unparented workstation widget. On success it
 * returns content which retains access to that widget; on failure it leaves the
 * original widget unparented. The context outlives the complete run call. */
typedef UmiStatus (*UmiApplicationProductGtk4ContentBuild)(GtkWindow *window,
    UmiApplicationProductGtk4Workstation *workstation, GtkWidget *originalContent,
    void *context, GtkWidget **outContent);
/** Release is idempotent and may run before the first Build. It runs after
 * windows detach their content and before the workstation is destroyed. It
 * must detach originalContent from any retained extension container. */
typedef void (*UmiApplicationProductGtk4ContentRelease)(void *context);
typedef struct UmiApplicationProductGtk4ContentExtension {
    UmiApplicationProductGtk4ContentBuild build;
    UmiApplicationProductGtk4ContentRelease release;
    void *context;
} UmiApplicationProductGtk4ContentExtension;
/** Use the same splash, alias resolution, window fitting, checkpoint storage,
 * titlebar and shutdown helpers as the existing run API. That API and its
 * configuration structure remain unchanged. */
int umi_application_product_gtk4_run_with_content(
    const UmiApplicationProductGtk4WorkstationConfig *config,
    const UmiApplicationProductGtk4ContentExtension *extension,
    int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
