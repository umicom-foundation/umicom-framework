/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/web/web_preview.inc
 *
 * PURPOSE:
 *   Define the umicom.web.preview reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.web.preview", "web", "Web Preview", "umicom.frontend",
         "Live responsive preview with logs and reload state.", UMI_APPLICATION_COMPONENT_VIEW,
         UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "browser html css javascript"),
