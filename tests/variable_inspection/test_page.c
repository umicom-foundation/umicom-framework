/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_inspection/test_page.c
 * PURPOSE: Exercise owned nested pages, duplicate display names, cycles and full depth/capacity limits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0}; Open(&f); PopulateVariables(&f);
    UmiDebugVariableTarget *target = NULL; OK(UmiDebugVariableTargetCapture(f.workspace, 0U, &target));
    UmiDebugVariableChildren *children = MakeChildren(3U); UmiDebugVariablePage *page = NULL;
    if (strcmp(name, "invalid") == 0) {
        memset(children->items[0].value, 'x', sizeof children->items[0].value);
        CHECK(UmiDebugVariablePageCreate(target, children, &page) == UMI_STATUS_INVALID_ARGUMENT && page == NULL);
        children->items[0].value[0] = '\0'; children->items[0].indexed_variables = UINT32_MAX;
        CHECK(UmiDebugVariablePageCreate(target, children, &page) == UMI_STATUS_CAPACITY_EXCEEDED && page == NULL);
    } else if (strcmp(name, "depth") == 0) {
        for (size_t i = 0U; i < UMI_DEBUG_VARIABLE_INSPECTION_DEPTH; ++i) {
            children->items[0].variables_reference = (uint64_t)i + 3U;
            OK(UmiDebugVariablePageCreate(target, children, &page));
            UmiDebugVariableTarget *next = NULL; OK(UmiDebugVariablePageTarget(page, 0U, &next));
            UmiDebugVariablePageDestroy(page); page = NULL; UmiDebugVariableTargetDestroy(target); target = next;
        }
        CHECK(UmiDebugVariableTargetExpandable(target) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiDebugVariablePageCreate(target, children, &page) == UMI_STATUS_CAPACITY_EXCEEDED && page == NULL);
    } else {
        if (strcmp(name, "empty") == 0) children->count = 0U;
        else if (strcmp(name, "cycle") == 0) children->items[0].variables_reference = 2U;
        else if (strcmp(name, "names") == 0) {
            children->count = 2U; children->items[1] = children->items[0];
            strcpy(children->items[1].value, "different"); children->items[1].variables_reference = 4U;
        } else if (strcmp(name, "capacity") == 0) {
            children->count = UMI_DEBUG_VARIABLE_CHILDREN_CAPACITY + 1U;
            CHECK(UmiDebugVariablePageCreate(target, children, &page) == UMI_STATUS_CAPACITY_EXCEEDED && page == NULL);
            children->count--; for (size_t i = 1U; i < children->count; ++i) children->items[i] = children->items[0];
        } else CHECK(strcmp(name, "owned") == 0 || strcmp(name, "bounds") == 0 || strcmp(name, "stale-child") == 0);
        OK(UmiDebugVariablePageCreate(target, children, &page));
        CHECK(UmiDebugVariablePageCount(page) == children->count);
        UmiDebugRuntimeVariable value;
        CHECK(UmiDebugVariablePageAt(page, children->count, &value) == UMI_STATUS_NOT_FOUND);
        UmiDebugVariableTarget *next = NULL;
        CHECK(UmiDebugVariablePageTarget(page, children->count, &next) == UMI_STATUS_NOT_FOUND && next == NULL);
        if (children->count != 0U) {
            OK(UmiDebugVariablePageTarget(page, strcmp(name, "names") == 0 ? 1U : 0U, &next));
            if (strcmp(name, "cycle") == 0) CHECK(UmiDebugVariableTargetExpandable(next) == UMI_STATUS_ALREADY_EXISTS);
            else OK(UmiDebugVariableTargetExpandable(next));
            if (strcmp(name, "stale-child") == 0) {
                UmiDebugVariableSnapshot root; OK(umi_debug_variable_registry_find(umi_debug_service_variable(f.service), "root", &root));
                OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &root));
                CHECK(UmiDebugVariableTargetValidate(f.workspace, next) == UMI_STATUS_BUSY);
            }
            strcpy(children->items[0].value, "mutated input");
            UmiDebugVariablePageDestroy(page); page = NULL; Close(&f);
            OK(UmiDebugVariableTargetRead(next, &value));
            CHECK(strcmp(value.value, strcmp(name, "names") == 0 ? "different" : "3") == 0);
            UmiDebugVariableTargetDestroy(next);
        }
    }
    if (f.workspace != NULL) Close(&f);
    UmiDebugVariablePageDestroy(page); UmiDebugVariableTargetDestroy(target); free(children); return 0;
}
