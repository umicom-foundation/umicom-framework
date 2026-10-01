/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/breakpoint_edit/test_reply.c
 * PURPOSE: Verify complete reply mapping, disabled-row isolation and atomic rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/debug_runtime/breakpoint_sync.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0}; Open(&f);
    UmiDebugBreakpointSnapshot requested[2]; requested[0] = Read(&f);
    UmiDebugBreakpointSnapshot next = requested[0]; strcpy(next.id, "second"); next.line = 20; next.verified = 0;
    strcpy(next.condition, "x > 3"); strcpy(next.log_message, "x={x}");
    OK(umi_debug_breakpoint_registry_upsert(f.registry, &next)); OK(umi_debug_breakpoint_registry_find(f.registry, "second", &requested[1]));
    next.enabled = 0; strcpy(next.id, "disabled"); OK(umi_debug_breakpoint_registry_upsert(f.registry, &next));
    UmiDebugRuntimeBreakpointList *reply = calloc(1U, sizeof *reply); CHECK(reply != NULL);
    reply->count = 2U; reply->items[0].verified = 1; reply->items[0].line = 14; reply->items[1].verified = 0;
    uint64_t generation = umi_debug_breakpoint_registry_revision(f.registry);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "short") == 0) { reply->count = 1; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(name, "extra") == 0) { reply->count = 3; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(name, "late-invalid") == 0) { reply->items[1].line = UINT32_MAX; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(name, "stale") == 0) { OK(umi_debug_breakpoint_registry_upsert(f.registry, &next)); expected = UMI_STATUS_BUSY; }
    else if (strcmp(name, "changed-request") == 0) { requested[1].line = 1; expected = UMI_STATUS_BUSY; }
    else if (strcmp(name, "duplicate") == 0) { requested[1] = requested[0]; expected = UMI_STATUS_ALREADY_EXISTS; }
    else if (strcmp(name, "disabled-request") == 0) { requested[1].enabled = 0; expected = UMI_STATUS_BUSY; }
    else CHECK(strcmp(name, "mapping") == 0);
    uint64_t before = umi_debug_breakpoint_registry_revision(f.registry);
    CHECK(UmiDebugRuntimeApplyBreakpointReply(f.registry, generation, requested, 2U, reply) == expected);
    if (expected == UMI_STATUS_OK) {
        CHECK(Read(&f).line == 14 && Read(&f).verified);
        OK(umi_debug_breakpoint_registry_find(f.registry, "second", &next));
        CHECK(!next.verified && next.line == 20 && strcmp(next.condition, "x > 3") == 0 && strcmp(next.log_message, "x={x}") == 0);
        OK(umi_debug_breakpoint_registry_find(f.registry, "disabled", &next)); CHECK(!next.enabled && next.revision <= generation);
    } else CHECK(Read(&f).line == 13 && umi_debug_breakpoint_registry_revision(f.registry) == before);
    free(reply); Close(&f); return 0;
}
