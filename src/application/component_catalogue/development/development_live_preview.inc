/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_live_preview.inc
 *
 * PURPOSE:
 *   Define the reusable active-document preview component used by developer
 *   applications without embedding renderer policy in those applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.development.live-preview", "development", "Live Preview",
     "umicom.browser",
     "Revision-aware active-document preview with provider and trust gates.",
     UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
