/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/media/media_delivery.inc
 *
 * PURPOSE:
 *   Define the umicom.media.delivery reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.media.delivery", "media", "Delivery Queue", "umicom.delivery",
         "Presets, render jobs, evidence and publishing destinations.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1,
                 "video audio image timeline"),
