/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_memory_transport.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "umicom/language_runtime/memory_transport.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Keep the original smoke exercise for review. The active entry point retains
 * it and adds boundary checks for the same public transport callbacks. */
#if 0
int main(void){UmiLanguageRuntimeMemoryTransport*m;UmiLanguageRuntimeTransport t;char b[16];size_t n=0;assert(umi_language_runtime_memory_transport_create(&m,&t)==UMI_STATUS_OK);assert(t.write(t.instance,"out",3)==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_written(m,b,sizeof(b),&n)==UMI_STATUS_OK);assert(strcmp(b,"out")==0);t.destroy(t.instance);return 0;}
#endif

static UmiStatus Append(UmiLanguageRuntimeMemoryTransport *memory,
    UmiLanguageRuntimeTransport *transport, int writing, const void *bytes, size_t count)
{
    return writing ? transport->write(transport->instance, bytes, count) :
        umi_language_runtime_memory_transport_push_read(memory, bytes, count);
}

int main(int argc, char **argv)
{
    if (argc > 2) return 2;
    const char *mode = argc == 1 ? "smoke" : argv[1];
    const char *known[] = {"smoke", "write-overflow", "read-overflow", "write-large",
        "read-large", "write-exact", "read-exact", "write-overfull", "read-overfull",
        "write-empty", "read-empty", "write-null", "read-null", "write-append",
        "read-append", "copy-small", "fragment-read"};
    int recognized = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0) recognized = 1;
    if (!recognized) return 2;
    UmiLanguageRuntimeMemoryTransport *memory = NULL;
    UmiLanguageRuntimeTransport transport = {0};
    assert(umi_language_runtime_memory_transport_create(&memory, &transport) == UMI_STATUS_OK);
    if (strcmp(mode, "smoke") == 0)
    {
        char bytes[16]; size_t count = 0U;
        assert(transport.write(transport.instance, "out", 3U) == UMI_STATUS_OK);
        assert(umi_language_runtime_memory_transport_written(memory, bytes, sizeof(bytes), &count) == UMI_STATUS_OK);
        assert(count == 3U && strcmp(bytes, "out") == 0);
    }
    else if (strcmp(mode, "copy-small") == 0)
    {
        char bytes[3] = {'x', 'y', 'z'}; size_t count = 99U;
        assert(transport.write(transport.instance, "out", 3U) == UMI_STATUS_OK);
        assert(umi_language_runtime_memory_transport_written(memory, bytes, sizeof(bytes), &count) ==
            UMI_STATUS_CAPACITY_EXCEEDED);
        assert(count == 99U && memcmp(bytes, "xyz", sizeof(bytes)) == 0);
    }
    else if (strcmp(mode, "fragment-read") == 0)
    {
        char bytes[4] = {0}; size_t count = 0U;
        assert(umi_language_runtime_memory_transport_push_read(memory, "abc", 3U) == UMI_STATUS_OK);
        assert(transport.read(transport.instance, bytes, 1U, 0U, &count) == UMI_STATUS_OK);
        assert(count == 1U && bytes[0] == 'a');
        assert(transport.read(transport.instance, bytes, sizeof(bytes), 0U, &count) == UMI_STATUS_OK);
        assert(count == 2U && memcmp(bytes, "bc", 2U) == 0);
        assert(transport.read(transport.instance, bytes, sizeof(bytes), 0U, &count) == UMI_STATUS_NOT_FOUND);
        assert(count == 0U);
    }
    else
    {
        int writing = strncmp(mode, "write-", 6U) == 0;
        const char *scenario = mode + (writing ? 6U : 5U);
        size_t capacity = UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY;
        unsigned char *input = malloc(capacity), *output = malloc(capacity);
        assert(input != NULL && output != NULL);
        memset(input, 'x', capacity);
        size_t expected = 1U, received = 0U;
        assert(Append(memory, &transport, writing, input, 1U) == UMI_STATUS_OK);
        if (strcmp(scenario, "overflow") == 0)
        {
            /* One existing byte makes 1 + SIZE_MAX wrap. The API must reject
             * the claimed span before reading beyond this one-byte source. */
            assert(Append(memory, &transport, writing, "y", SIZE_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        }
        else if (strcmp(scenario, "large") == 0)
            assert(Append(memory, &transport, writing, "y", capacity) == UMI_STATUS_CAPACITY_EXCEEDED);
        else if (strcmp(scenario, "empty") == 0)
            assert(Append(memory, &transport, writing, NULL, 0U) == UMI_STATUS_OK);
        else if (strcmp(scenario, "null") == 0)
            assert(Append(memory, &transport, writing, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        else if (strcmp(scenario, "append") == 0)
        {
            assert(Append(memory, &transport, writing, input, 2U) == UMI_STATUS_OK);
            expected = 3U;
        }
        else
        {
            /* The existing contract reserves one byte for a NUL when output
             * is inspected. Filling the remaining payload exactly is valid. */
            assert(Append(memory, &transport, writing, input, capacity - 2U) == UMI_STATUS_OK);
            expected = capacity - 1U;
            if (strcmp(scenario, "overfull") == 0)
                assert(Append(memory, &transport, writing, "y", 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        }
        memset(output, 0x39, capacity);
        if (writing)
        {
            assert(umi_language_runtime_memory_transport_written(memory, (char *)output,
                capacity, &received) == UMI_STATUS_OK);
            assert(output[expected] == 0U);
        }
        else
            assert(transport.read(transport.instance, output, capacity, 0U, &received) == UMI_STATUS_OK);
        assert(received == expected && memcmp(input, output, expected) == 0);
        free(output); free(input);
    }
    transport.destroy(transport.instance);
    return 0;
}
