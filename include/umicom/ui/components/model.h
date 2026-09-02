/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/model.h
 *
 * PURPOSE:
 *   Combine a component specification with common state and style for adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_MODEL_H
#define UMICOM_UI_COMPONENTS_MODEL_H
#include "umicom/ui/components/component.h"
#include "umicom/ui/components/state.h"
#include "umicom/ui/components/style.h"
/**
 * Represent the ui component model data shared with callers of this public contract.
 */
typedef struct UmiUiComponentModel { UmiUiComponentSpec spec; UmiUiComponentState state; UmiUiComponentStyle style; } UmiUiComponentModel;
/**
 * Provide the ui component model default operation used by this module and its client
 * applications.
 */
UmiUiComponentModel umi_ui_component_model_default(UmiUiComponentKind kind);
#endif
