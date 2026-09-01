/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_profiler.inc
 *
 * PURPOSE:
 *   Register the reusable performance-profiler surface used by development
 *   and operational applications without embedding a profiler in either app.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* This component projects read-only performance evidence; sampling providers
 * remain replaceable Framework adapters rather than UI-owned processes. */
COMP("umicom.development.profiler", "development", "Performance Profiler",
     "umicom.observability.performance",
     "CPU, memory, call-tree, hotspot and regression evidence.",
     UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_FOUNDATION, 0, 1),
