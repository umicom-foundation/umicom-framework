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
    UmiApplicationProductGtk4WorkstationConfig config =
        umi_application_product_gtk4_workstation_config_default(
            UMICOM_PRODUCT_APPLICATION_ID, UMICOM_PRODUCT_TITLE, NULL, NULL);
    config.mode_badge = "Layout preview";
    return umi_application_product_gtk4_run(&config, argc, argv);
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
