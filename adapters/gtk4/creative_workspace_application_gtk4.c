/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_workspace_application_gtk4.c
 * PURPOSE: Run creative products through the existing branded Framework product lifecycle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/creative_workspace.h"
#include "umicom/application/suite_layout/gtk4_product_content_extension.h"
typedef struct CreativeApplication {UmiCreativeGtkSurface * surface;
    const char * profile;
} CreativeApplication;
static void Release(void * data)
{
    CreativeApplication * application = data;
    UmiCreativeGtkSurfaceDestroy(application -> surface);
    application -> surface = NULL;
}

static UmiStatus Build(GtkWindow * window, UmiApplicationProductGtk4Workstation * workstation, GtkWidget * original, void * data, GtkWidget * * out)
{
    CreativeApplication * application = data;
    (void)window;
    (void)workstation;
    * out = NULL;
    UmiStatus status = UmiCreativeGtkSurfaceCreate(original, application -> profile, & application -> surface);
    if (status == UMI_STATUS_OK) * out = UmiCreativeGtkSurfaceWidget(application -> surface);
    return status;
}

int UmiCreativeGtkRun(const char * applicationId, const char * title, const char * profile, int argc, char * * argv)
{
    CreativeApplication application = {NULL, profile};
    UmiApplicationProductGtk4WorkstationConfig config = umi_application_product_gtk4_workstation_config_default(applicationId, title, NULL, NULL);
    UmiApplicationProductGtk4ContentExtension extension = {Build, Release, & application};
    config.mode_badge = "Creative workbench";
    int result = umi_application_product_gtk4_run_with_content( & config, & extension, argc, argv);
    Release( & application);
    return result;
}

