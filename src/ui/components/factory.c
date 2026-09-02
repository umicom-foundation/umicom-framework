/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/factory.c
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
#include "umicom/ui/components/factory.h"
/*
 * Check that ui component factory satisfies its contract before another service relies on
 * it.
 */
int umi_ui_component_factory_is_valid(const UmiUiComponentFactory*f){return f&&f->structure_size>=sizeof(*f)&&f->create&&f->destroy;}
