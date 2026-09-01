/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_notebook.inc
 *
 * PURPOSE:
 *   Register an executable notebook document surface shared by language,
 *   research, education and development applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* The notebook owns presentation state only; a selected language runtime owns
 * cell execution, cancellation and generated output. */
COMP("umicom.development.notebook", "development", "Notebook",
     "umicom.language.notebook",
     "Executable cells, outputs, diagnostics and linked document context.",
     UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_FOUNDATION, 1, 1),
