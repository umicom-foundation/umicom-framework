/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/llm/llm_runtime.inc
 *
 * PURPOSE:
 *   Define the umicom.llm.runtime reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.llm.runtime", "llm", "Runtime Manager", "umicom.llm",
         "Load, unload, schedule and monitor inference runtimes.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_FOUNDATION, 1, 1),
