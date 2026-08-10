/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/declarative_renderer_headless.c
 *
 * PURPOSE:
 *   Provide a deterministic headless declarative renderer useful for tests, CI and non-GUI application inspection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The headless adapter turns semantic state into deterministic testable output
 * without requiring a graphical display or GTK event loop.
 */

#include "umicom/declarative/renderer.h"

/* The public helper already renders plans deterministically; this translation unit keeps the
 * headless adapter boundary explicit for future richer view-tree inspection. */
UmiStatus umi_decl_headless_adapter_render(const UmiDeclApplicationPlan *plan,char *out_text,size_t capacity)
{
    return umi_decl_headless_render(plan,out_text,capacity);
}
