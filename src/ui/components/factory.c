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

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/factory.h"
int umi_ui_component_factory_is_valid(const UmiUiComponentFactory*f){return f&&f->structure_size>=sizeof(*f)&&f->create&&f->destroy;}
