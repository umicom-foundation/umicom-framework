/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/quality/quality_automated_tests.inc
 *
 * PURPOSE:
 *   Publish Umicom Automated Tests as a reusable Framework component. Any
 *   Umicom application can host this surface instead of building its own test
 *   runner and evidence viewer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.quality.automated-tests", "quality", "Umicom Automated Tests",
         "umicom.ui.automation",
         "Desktop user journeys, control wiring, assertions and acceptance evidence.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
