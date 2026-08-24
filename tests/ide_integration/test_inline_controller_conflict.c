/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_inline_controller_conflict.c
 *
 * PURPOSE:
 *   Integration regression coverage for inline controller conflict.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/inline_controller.h"
static UmiStatus revision(void *u,const char *d,uint64_t *r)
{(void)u;(void)d;*r=2U;return UMI_STATUS_OK;}
static UmiStatus insert(void *u,const char*d,uint32_t l,uint32_t c,const char*t,size_t n)
{(void)u;(void)d;(void)l;(void)c;(void)t;(void)n;return UMI_STATUS_OK;}
int main(void)
{
    UmiIdeInlineController controller;
    UmiIdeInlineExecutor executor = {0};
    UmiIdeEditorEditAdapter edits = {revision, insert, NULL};
    UmiIdeInlineSuggestion snapshot;
    executor.runtime = (UmiAiRuntime *)1;
    (void)strcpy(executor.provider_id, "provider");
    (void)strcpy(executor.model_id, "model");
    assert(umi_ide_inline_controller_init(
        &controller, &executor, &edits) == UMI_STATUS_OK);
    (void)strcpy(controller.active.suggestion_id, "inline.1");
    (void)strcpy(controller.active.document_id, "doc");
    (void)strcpy(controller.active.text, "x");
    controller.active.document_revision = 1U;
    controller.active.state = UMI_IDE_INLINE_READY;
    assert(umi_ide_inline_controller_accept(&controller) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_ide_inline_controller_snapshot(
        &controller, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state == UMI_IDE_INLINE_FAILED);
    umi_ide_inline_controller_deinit(&controller);
    return 0;
}

