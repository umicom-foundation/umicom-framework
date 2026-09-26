/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/desktop_system/test_adversarial.c
 * Exercise bounded parser inputs without running commands or writing storage.
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop_system/linux_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t randomState = UINT32_C(200926);
static uint32_t Next(void)
{
    randomState ^= randomState << 13;
    randomState ^= randomState >> 17;
    randomState ^= randomState << 5;
    return randomState;
}
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "Invariant failed at line %d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    static const char *const examples[] = {
        "MemTotal: 1000 kB\nMemAvailable: 400 kB\n",
        "cpu 1 2 3 4 5 6 7 8 9 10\n",
        "9 (notes (draft)) S 1 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 777 23 3\n",
        " eth0: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16\n"
    };
    size_t successful = 0;
    for (unsigned iteration = 0; iteration < 10000; ++iteration) {
        const char *seed = examples[iteration % 4];
        size_t length = strlen(seed); unsigned mode = iteration % 5;
        if (mode == 1) length = (size_t)Next() % (length + 1);
        if (mode == 4) length = (size_t)Next() % 1025;
        /* Exact-size storage lets ASan catch even a one-byte read past input. */
        char *bytes = malloc(length ? length : 1); CHECK(bytes != NULL);
        if (mode == 4) { for (size_t i = 0; i < length; ++i) bytes[i] = (char)(Next() & 255U); }
        else if (length) {
            memcpy(bytes, seed, length);
            if (mode == 2) bytes[(size_t)Next() % length] ^= (char)(1U << (Next() % 8));
            if (mode == 3) bytes[(size_t)Next() % length] = 0;
        }
        UmiDesktopSystemMemory memory, memoryBefore;
        UmiDesktopSystemCpu cpu, cpuBefore;
        UmiDesktopSystemProcess process, processBefore;
        UmiDesktopSystemInterface network, networkBefore;
        memset(&memory, 0xa5, sizeof memory); memcpy(&memoryBefore, &memory, sizeof memory);
        memset(&cpu, 0xa5, sizeof cpu); memcpy(&cpuBefore, &cpu, sizeof cpu);
        memset(&process, 0xa5, sizeof process); memcpy(&processBefore, &process, sizeof process);
        memset(&network, 0xa5, sizeof network); memcpy(&networkBefore, &network, sizeof network);
        UmiStatus status = UmiDesktopSystemParseMemory(bytes, length, &memory);
        if (status == UMI_STATUS_OK) { ++successful; CHECK(memory.totalBytes > 0 && memory.availableBytes <= memory.totalBytes); }
        else CHECK(memcmp(&memory, &memoryBefore, sizeof memory) == 0);
        status = UmiDesktopSystemParseCpu(bytes, length, &cpu);
        if (status == UMI_STATUS_OK) ++successful;
        else CHECK(memcmp(&cpu, &cpuBefore, sizeof cpu) == 0);
        status = UmiDesktopSystemParseProcess(bytes, length, 4096, &process);
        if (status == UMI_STATUS_OK) {
            ++successful; CHECK(process.pid > 0 && process.startKnown == 1 && process.residentKnown == 1);
            CHECK(memchr(process.name, 0, sizeof process.name) != NULL);
        } else CHECK(memcmp(&process, &processBefore, sizeof process) == 0);
        status = UmiDesktopSystemParseInterface(bytes, length, &network);
        if (status == UMI_STATUS_OK) { ++successful; CHECK(network.name[0] && memchr(network.name, 0, sizeof network.name) != NULL); }
        else CHECK(memcmp(&network, &networkBefore, sizeof network) == 0);
        free(bytes);
    }
    CHECK(successful >= 2000);
    printf("10000 deterministic inputs, 40000 parser calls; %zu accepted results. Failure leaves output unchanged.\n", successful);
    return 0;
}
