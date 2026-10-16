/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/healthcare/synthetic_access.inc
 *
 * PURPOSE:
 *   Describe the existing fictional Healthcare access-request workspace for
 *   portable panel discovery, sizing and multi-instance ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* This presentation record follows the existing session-only component. It
 * advertises no clinical connection or patient-data processing capability. */
PANEL_SPEC("umicom.healthcare.synthetic-access", "umicom.icon.healthcare.synthetic-access",
           UMI_APPLICATION_PRESENTATION_SURFACE_SERVICE,
           UMI_APPLICATION_PRESENTATION_CHROME_SERVICE,
           UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 640U, 360U,
           UMI_APPLICATION_PRESENTATION_EMPTY_NONE,
           1, 1, 1, 1,
           UMI_APPLICATION_PRESENTATION_COMPACT_KEEP),
