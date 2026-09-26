/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/product_workstation_main.c
 *
 * PURPOSE:
 *   Launch a thin product's canonical desktop layout without copying GUI code.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_product_application.h"
#ifdef UMICOM_PRODUCT_ENTERPRISE_WORKSPACE
#include "umicom/ui/gtk4/enterprise_workspace.h"
#endif

#ifdef UMICOM_PRODUCT_AI_WORKSPACE
#include "umicom/ui/gtk4/ai_workspace.h"
#endif

#ifdef UMICOM_PRODUCT_FINANCE_OPERATIONS
#include "umicom/ui/gtk4/finance_operations.h"
#endif

#ifndef UMICOM_PRODUCT_APPLICATION_ID
#error "The product build must supply its canonical application identifier"
#endif
#ifndef UMICOM_PRODUCT_TITLE
#error "The product build must supply its display name"
#endif

/* The common host supplies branding, layout controls and an honest preview
 * state. Products attach their domain controllers when those services exist. */
int main(int argc, char **argv)
{
#ifdef UMICOM_PRODUCT_ENTERPRISE_WORKSPACE
    /* Enterprise products reuse Framework-owned data, review and policy views.
     * Every earlier product path remains below, selected by its own build. */
    return UmiEnterpriseWorkspaceGtkRun(UMICOM_PRODUCT_APPLICATION_ID,
        UMICOM_PRODUCT_TITLE, argc, argv);
#else
#ifdef UMICOM_PRODUCT_AI_WORKSPACE
    /* The AI products share one Framework-owned source/job workspace. Their
     * previous catalogue layouts remain inside the same product host. */
    return UmiAiWorkspaceGtkRun(UMICOM_PRODUCT_APPLICATION_ID,
        UMICOM_PRODUCT_TITLE, argc, argv);
#else
#ifdef UMICOM_PRODUCT_FINANCE_OPERATIONS
    /* Shared services now back the finance command forms. The existing
     * workstation is composed into a Layouts page, not removed or re-created
     * in either application. Other products retain the complete preview path. */
    return UmiFinanceOperationsGtkRun(UMICOM_PRODUCT_APPLICATION_ID,
        UMICOM_PRODUCT_TITLE, argc, argv);
#else
    UmiApplicationProductGtk4WorkstationConfig config =
        umi_application_product_gtk4_workstation_config_default(
            UMICOM_PRODUCT_APPLICATION_ID, UMICOM_PRODUCT_TITLE, NULL, NULL);
    config.mode_badge = "Layout preview";
    return umi_application_product_gtk4_run(&config, argc, argv);
#endif
#endif /* UMICOM_PRODUCT_AI_WORKSPACE */
#endif /* UMICOM_PRODUCT_ENTERPRISE_WORKSPACE */
}

#ifdef _WIN32
#include <stdlib.h>
#include <windows.h>

/* The Windows GUI subsystem enters here; preserve the portable argument path. */
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance,
                   LPSTR command_line, int show_command)
{
    (void)instance;
    (void)previous_instance;
    (void)command_line;
    (void)show_command;
    return main(__argc, __argv);
}
#endif
