/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/gtk4_product_application.h
 *
 * PURPOSE:
 *   Run a native product window using shared startup, layout and lifetime rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_PRODUCT_APPLICATION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_PRODUCT_APPLICATION_H

#include "umicom/application/suite_layout/gtk4_product_workstation.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Run the GTK event loop until the product window is closed.
 *
 * Configuration strings and controller context must remain alive until this
 * call returns. Each product receives its own application identity. A missing
 * controller registrar selects layout-preview mode: panels explain that their
 * product service is not connected, and no domain command is executed.
 * Returns zero on normal completion or a nonzero startup/application error.
 */
int umi_application_product_gtk4_run(
    const UmiApplicationProductGtk4WorkstationConfig *config,
    int argc,
    char **argv);

#ifdef __cplusplus
}
#endif
#endif
