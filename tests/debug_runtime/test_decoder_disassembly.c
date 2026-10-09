/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_disassembly.c
 *
 * PURPOSE:
 *   Verify the bounded disassembly DAP decoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/decoders/disassembly.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The original automatic fixture can exceed the native Windows stack before its first assertion. The replacement owns the same records on the heap; retain the earlier test and its comments for review. */
#if 0
int main(void)
{
    UmiDebugRuntimeDisassembly result;
    assert(umi_debug_runtime_decode_disassembly(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"disassemble\",\"body\":{\"instructions\":[{"
        "\"address\":\"0x1000\",\"instruction\":\"mov eax, 1\"}]}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(strcmp(result.items[0].address, "0x1000") == 0);
    return 0;
}

#endif
#include <stdlib.h>
#include <stdio.h>

/* Large bounded records belong to this explicit fixture owner, not the native
 * thread stack. Each process still creates a fresh independent model and runs
 * the original semantic assertions; allocation failure is a test failure. */
typedef struct FixtureStorage {
    UmiDebugRuntimeDisassembly result;
} FixtureStorage;

static int CheckFixture(FixtureStorage *state)
{
    assert(umi_debug_runtime_decode_disassembly(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"disassemble\",\"body\":{\"instructions\":[{"
        "\"address\":\"0x1000\",\"instruction\":\"mov eax, 1\"}]}}",
        &state->result) == UMI_STATUS_OK);
    assert(state->result.count == 1U);
    assert(strcmp(state->result.items[0].address, "0x1000") == 0);
    return 0;
}

int main(void)
{
    FixtureStorage *state = calloc(1U, sizeof *state);
    if (state == NULL) {
        fputs("Cannot allocate fixture storage\n", stderr);
        return 1;
    }
    int result = CheckFixture(state);
    free(state);
    return result;
}
