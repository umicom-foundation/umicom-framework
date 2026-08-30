/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_edit_adapter.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration edit adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ide_integration/edit_adapter.h"
static UmiStatus revision(void *u,const char *d,uint64_t *r)
{(void)u;(void)d;*r=1U;return UMI_STATUS_OK;}
static UmiStatus insert(void *u,const char*d,uint32_t l,uint32_t c,const char*t,size_t n)
{(void)u;(void)d;(void)l;(void)c;(void)t;(void)n;return UMI_STATUS_OK;}
int main(void)
{
    UmiIdeEditorEditAdapter adapter = {revision, insert, NULL};
    assert(umi_ide_editor_edit_adapter_validate(&adapter) == UMI_STATUS_OK);
    adapter.insert_text = NULL;
    assert(umi_ide_editor_edit_adapter_validate(&adapter) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

