/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/model.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
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
#include "umicom/ui/components/model.h"
UmiUiComponentModel umi_ui_component_model_default(UmiUiComponentKind k){UmiUiComponentModel m;m.spec=umi_ui_component_spec_default(k);m.state=umi_ui_component_state_default();m.style=umi_ui_component_style_default();return m;}
