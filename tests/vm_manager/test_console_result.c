/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_console_result.c
 *
 * PURPOSE:
 *   Verify console exit, final observation and forced-stop reporting without starting QEMU.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vm_manager/manager.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static const char *scenario;
static unsigned observations, stops, reads;
static UmiStatus TestObserve(UmiVmSession *session, UmiVmSnapshot *out)
{
    (void)session;
    ++observations;
    if (strcmp(scenario, "observe-error") == 0 ||
        (strcmp(scenario, "final-error") == 0 && observations > 1U)) return UMI_STATUS_IO_ERROR;
    memset(out, 0, sizeof *out);
    out->processRunning = stops == 0U && (strcmp(scenario, "eof") == 0 ||
        strcmp(scenario, "stop-error") == 0 || strcmp(scenario, "force-stop") == 0);
    out->exitCode = strcmp(scenario, "exit-error") == 0 ? 23 : stops ? 137 : 0;
    strcpy(out->state, out->processRunning ? "paused" : "exited");
    return UMI_STATUS_OK;
}
static UmiStatus TestStop(UmiVmSession *session, UmiVmReport *report)
{
    (void)session; ++stops;
    memset(report, 0, sizeof *report);
    strcpy(report->detail, "Controlled stop fixture; no guest process was started.");
    return strcmp(scenario, "stop-error") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static char *TestLine(char *line, int capacity, FILE *input)
{
    (void)input; ++reads;
    if (strcmp(scenario, "force-stop") != 0 || capacity < 12) return NULL;
    memcpy(line, "force-stop\n", 12U); return line;
}
/* Compile the real CLI adapter against deterministic session observations.
 * Native process and QMP behavior has separate inert-child coverage. */
#define UmiVmObserve TestObserve
#define UmiVmForceStop TestStop
#define fgets TestLine
#include "../../src/vm_manager/cli.c"
#undef fgets
#undef UmiVmForceStop
#undef UmiVmObserve
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    scenario = argv[1];
    if (strcmp(scenario, "invalid-negative") == 0) return UmiVmMain(-1, NULL) == 2 ? 0 : 1;
    if (strcmp(scenario, "invalid-vector") == 0) return UmiVmMain(2, NULL) == 2 ? 0 : 1;
    if (strcmp(scenario, "invalid-element") == 0) {
        char *arguments[] = {"umicom-vm", NULL};
        return UmiVmMain(2, arguments) == 2 ? 0 : 1;
    }
    const char *known[] = {"exit-ok", "exit-error", "observe-error", "final-error", "eof", "stop-error", "force-stop"};
    int found = 0;
    for (size_t index = 0; index < sizeof known / sizeof known[0]; ++index)
        if (strcmp(scenario, known[index]) == 0) found = 1;
    if (!found) return 2;
    /* The mock never dereferences this opaque token. */
    int result = VmSessionConsole((UmiVmSession *)(uintptr_t)1U);
    int expected = strcmp(scenario, "exit-ok") == 0 ? 0 : 1;
    if (result != expected) return 1;
    if (strcmp(scenario, "eof") == 0 || strcmp(scenario, "stop-error") == 0 ||
        strcmp(scenario, "force-stop") == 0) {
        if (stops != 1U || reads != 1U) return 1;
    } else if (stops != 0U || reads != 0U) return 1;
    return observations >= 2U ? 0 : 1;
}
