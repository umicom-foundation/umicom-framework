/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/quality/quality_automated_tests.inc
 *
 * PURPOSE:
 *   Give the automated-test runner enough room for scenarios, step results and
 *   evidence while retaining normal dock, float and layout controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.quality.automated-tests",
           "umicom.icon.quality.automated-tests",
           UMI_APPLICATION_PRESENTATION_SURFACE_DASHBOARD,
           UMI_APPLICATION_PRESENTATION_CHROME_STANDARD,
           UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 560U, 300U,
           UMI_APPLICATION_PRESENTATION_EMPTY_CREATE,
           1, 1, 1, 1),
