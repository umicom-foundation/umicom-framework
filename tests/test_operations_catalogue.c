/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_operations_catalogue.c
 *
 * PURPOSE:
 *   Implement the test operations catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Operations catalogue tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/observability/operations_catalogue.h"
int main(void)
{
    const UmiOperationsViewDescriptor *profiler;
    assert(umi_operations_catalogue_count() == 10U);
    profiler = umi_operations_catalogue_find("profiler");
    assert(profiler != NULL);
    assert(profiler->kind == UMI_OPERATIONS_VIEW_PROFILER);
    assert(strcmp(profiler->title,"Profiler") == 0);
    assert(umi_operations_catalogue_at(10U) == NULL);
    return 0;
}
