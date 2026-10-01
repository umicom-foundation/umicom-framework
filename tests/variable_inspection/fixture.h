/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_inspection/fixture.h
 * PURPOSE: Provide a retained stopped frame and compound root using the real debugger registries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VARIABLE_INSPECTION_FIXTURE_H
#define UMICOM_VARIABLE_INSPECTION_FIXTURE_H
#include "../watch_edit/fixture.h"
#include "umicom/debug_runtime/variable_inspection.h"
static inline void PopulateVariables(Fixture *f)
{
    UmiDebugThreadSnapshot thread = {0}; strcpy(thread.id, "thread"); strcpy(thread.session_id, "session");
    thread.native_id = 1U; thread.stopped = 1; thread.current = 1;
    OK(umi_debug_thread_registry_upsert(umi_debug_service_thread(f->service), &thread));
    UmiDebugStackFrameSnapshot frame = {0}; strcpy(frame.id, "0"); strcpy(frame.thread_id, "thread");
    OK(umi_debug_stack_frame_registry_upsert(umi_debug_service_stack_frame(f->service), &frame));
    UmiDebugScopeSnapshot scope = {0}; strcpy(scope.id, "scope"); strcpy(scope.frame_id, "0");
    scope.variables_reference = 1U;
    OK(umi_debug_scope_registry_upsert(umi_debug_service_scope(f->service), &scope));
    UmiDebugVariableSnapshot variable = {0}; strcpy(variable.id, "root"); strcpy(variable.scope_id, "scope");
    strcpy(variable.name, "notes"); strcpy(variable.value, "{...}"); strcpy(variable.type, "Notebook");
    strcpy(variable.evaluate_name, "notes"); variable.variables_reference = 2U;
    OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f->service), &variable));
}
static inline UmiDebugVariableChildren *MakeChildren(uint64_t reference)
{
    UmiDebugVariableChildren *children = calloc(1, sizeof *children); CHECK(children != NULL);
    children->count = 1U; strcpy(children->items[0].name, "item"); strcpy(children->items[0].value, "3");
    children->items[0].variables_reference = reference; return children;
}
#endif
