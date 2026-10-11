/*-----------------------------------------------------------------------------
 * Umicom Framework - Native VM console journey regression tests
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 *
 * Tests use a deterministic QMP-like serial callback. No QEMU, PowerShell,
 * host disk, toolchain discovery or guest process is needed. These tests cover
 * the bounded read/write protocol; an actual QEMU run is a separate gate.
 *---------------------------------------------------------------------------*/
#include "umicom/vm_manager/console_journey.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_OUTPUT_CAPACITY 98304U

typedef struct FakeConsole {
    char queued[TEST_OUTPUT_CAPACITY];
    size_t front;
    size_t end;
    size_t writes;
    size_t submitted;
    size_t reads;
    unsigned chunk;
    uint64_t clock;
    int errorRead;
    int errorWrite;
    int badEcho;
    int missingExpectation;
    int unknownCommand;
    int guestPanic;
    int missingPrompt;
    int timeouts;
    int rollback;
    int stoppedClock;
    int outputLimit;
    int staleBanner;
} FakeConsole;

static int FakeQueue(FakeConsole *console, const char *text, size_t length)
{
    if (length > TEST_OUTPUT_CAPACITY - console->end) return 0;
    memcpy(console->queued + console->end, text, length);
    console->end += length;
    return 1;
}

static int FakeWrite(void *context, const void *bytes, size_t length)
{
    FakeConsole *console = context;
    if (console->errorWrite) return 0;
    if (!length || length != 1U) return 0;
    const char byte = *(const char *)bytes;
    ++console->writes;
    if (byte != 13) {
        const char echoed = console->badEcho ? '!' : byte;
        return FakeQueue(console, &echoed, 1U);
    }
    const size_t index = console->submitted++;
    if (!FakeQueue(console, "\r\n", 2U)) return 0;
    if (console->unknownCommand) {
        const char *message = "unknown command or incorrect arguments; use help\r\n";
        if (!FakeQueue(console, message, strlen(message))) return 0;
    } else if (console->guestPanic) {
        const char *message = "UMICOM_KERNEL_FAIL\r\n";
        if (!FakeQueue(console, message, strlen(message))) return 0;
    } else if (console->outputLimit) {
        char pad[256];
        memset(pad, 'X', sizeof pad);
        for (unsigned i = 0U; i < 160U; ++i)
            if (!FakeQueue(console, pad, sizeof pad)) return 0;
    } else if (index == 0U) {
        const char *text = console->missingExpectation ? "missing\r\n" :
            "Umicom Kernel development console\r\n";
        if (!FakeQueue(console, text, strlen(text))) return 0;
    } else if (index == 1U) {
        const char *message = "file README bytes=24\r\n";
        if (!FakeQueue(console, message, strlen(message))) return 0;
    } else {
        const char *message = "disk.mount-state=unused\r\n";
        if (!FakeQueue(console, message, strlen(message))) return 0;
    }
    return console->missingPrompt ? 1 : FakeQueue(console, "umicom> ", 8U);
}

static int FakeRead(void *context, void *output, size_t capacity, size_t *received)
{
    FakeConsole *console = context;
    if (console->errorRead) return 0;
    ++console->reads;
    const size_t available = console->end - console->front;
    size_t count = available;
    if (count > capacity) count = capacity;
    if (count > console->chunk) count = console->chunk;
    if (count) {
        memcpy(output, console->queued + console->front, count);
        console->front += count;
    }
    *received = count;
    return 1;
}

static uint64_t FakeClock(void *context)
{
    FakeConsole *console = context;
    if (console->rollback && console->writes > 0U) return 0U;
    if (console->stoppedClock) return 1U;
    console->clock += console->timeouts ? 500U : 1U;
    return console->clock;
}

static void FakeIdle(void *context)
{
    (void)context;
}

static int Check(const char *name, int condition)
{
    if (!condition) fprintf(stderr, "FAILED %s\n", name);
    return condition;
}

static int Case(const char *name)
{
    static const UmiVmConsoleJourneyStep steps[] = {
        {"cat /README", "Umicom Kernel development console"},
        {"ls /", "README"},
        {"mountinfo", "disk.mount-state=unused"}
    };
    FakeConsole *console = calloc(1U, sizeof *console);
    if (!console) return 1;
    console->chunk = 5U;
    const char *boot = "normal QEMU boot\r\numicom> ";
    if (strcmp(name,"stale_boot") == 0) {
        boot = "README disk.mount-state=unused\r\numicom> ";
        console->missingExpectation = 1;
    }
    if (strcmp(name,"early_failure") == 0) boot = "UMICOM_KERNEL_FAIL\r\numicom> ";
    if (strcmp(name,"missing_prompt") == 0 ||
        strcmp(name,"clock_stalled") == 0) boot = "normal QEMU boot\r\n";
    if (!FakeQueue(console, boot, strlen(boot))) {free(console);return 1;}
    if (!strcmp(name,"fragmented")) console->chunk = 1U;
    if (!strcmp(name,"bad_echo")) console->badEcho = 1;
    if (!strcmp(name,"missing_expect")) console->missingExpectation = 1;
    if (!strcmp(name,"unknown")) console->unknownCommand = 1;
    if (!strcmp(name,"panic")) console->guestPanic = 1;
    if (!strcmp(name,"read_error")) console->errorRead = 1;
    if (!strcmp(name,"no_retry")) console->errorWrite = 1;
    if (!strcmp(name,"write_error")) console->errorWrite = 1;
    if (!strcmp(name,"timeout")) console->timeouts = 1;
    if (!strcmp(name,"clock_rollback")) console->rollback = 1;
    if (!strcmp(name,"clock_stalled")) console->stoppedClock = 1;
    if (!strcmp(name,"output_limit")) console->outputLimit = 1;
    UmiVmConsoleJourneyTransport io = {
        console, FakeWrite, FakeRead, FakeClock, FakeIdle
    };
    UmiVmConsoleJourneyReport report;
    const UmiVmConsoleJourneyStep emptyCommand[] = {{"", "valid"}};
    const UmiVmConsoleJourneyStep badAscii[] = {{"a\nb", "valid"}};
    const UmiVmConsoleJourneyStep missingExpected[] = {{"valid", ""}};
    if (!strcmp(name,"invalid_transport")) io.read = NULL;
    UmiVmConsoleJourneyStatus expected = UMI_VM_CONSOLE_JOURNEY_OK;
    if (!strcmp(name,"stale_boot") || !strcmp(name,"missing_expect"))
        expected = UMI_VM_CONSOLE_JOURNEY_EXPECTATION_MISSING;
    if (!strcmp(name,"unknown") || !strcmp(name,"panic") || !strcmp(name,"early_failure"))
        expected = UMI_VM_CONSOLE_JOURNEY_GUEST_FAILURE;
    if (!strcmp(name,"bad_echo")) expected = UMI_VM_CONSOLE_JOURNEY_PROTOCOL_FAILURE;
    if (!strcmp(name,"read_error") || !strcmp(name,"write_error") ||
        !strcmp(name,"no_retry"))
        expected = UMI_VM_CONSOLE_JOURNEY_TRANSPORT_FAILURE;
    if (!strcmp(name,"timeout") || !strcmp(name,"missing_prompt"))
        expected = UMI_VM_CONSOLE_JOURNEY_TIMEOUT;
    if (!strcmp(name,"clock_rollback")) expected = UMI_VM_CONSOLE_JOURNEY_CLOCK_ROLLBACK;
    if (!strcmp(name,"clock_stalled")) expected = UMI_VM_CONSOLE_JOURNEY_CLOCK_STALLED;
    if (!strcmp(name,"output_limit")) expected = UMI_VM_CONSOLE_JOURNEY_OUTPUT_LIMIT;
    if (!strncmp(name,"invalid_",8U) || !strcmp(name,"no_input_empty"))
        expected = UMI_VM_CONSOLE_JOURNEY_INVALID_ARGUMENT;
    UmiVmConsoleJourneyStatus got;
    if (!strcmp(name,"invalid_report")) {
        got = UmiVmConsoleJourneyRun(&io, steps, 3U, 45000U, NULL);
    } else if (!strcmp(name,"invalid_count")) {
        got = UmiVmConsoleJourneyRun(&io, steps, 0U, 45000U, &report);
    } else if (!strcmp(name,"invalid_many")) {
        got = UmiVmConsoleJourneyRun(&io, steps,
            UMI_VM_CONSOLE_JOURNEY_COMMAND_LIMIT + 1U, 45000U, &report);
    } else if (!strcmp(name,"invalid_step")) {
        got = UmiVmConsoleJourneyRun(&io, emptyCommand, 1U, 45000U, &report);
    } else if (!strcmp(name,"invalid_ascii")) {
        got = UmiVmConsoleJourneyRun(&io, badAscii, 1U, 45000U, &report);
    } else if (!strcmp(name,"no_input_empty")) {
        got = UmiVmConsoleJourneyRun(&io, missingExpected, 1U, 45000U, &report);
    } else if (!strcmp(name,"invalid_timeout")) {
        got = UmiVmConsoleJourneyRun(&io, steps, 3U, 0U, &report);
    } else {
        got = UmiVmConsoleJourneyRun(&io, steps, 3U,
            console->timeouts ? 100U : 45000U, &report);
    }
    if (!strcmp(name,"invalid_report")) expected = UMI_VM_CONSOLE_JOURNEY_INVALID_ARGUMENT;
    int okay = Check(name, got == expected);
    if (strcmp(name,"invalid_report"))
        okay &= Check("report status", report.status == expected);
    if (got == UMI_VM_CONSOLE_JOURNEY_OK) {
        okay &= Check("accepted steps", report.completedSteps == 3U);
        okay &= Check("successful failure sentinel", report.failedStep == 3U);
        okay &= Check("no stray writes", console->submitted == 3U);
        okay &= Check("expected read bytes", report.receivedBytes > 30U);
    }
    if (!strncmp(name,"invalid_",8U) || !strcmp(name,"no_input_empty"))
        okay &= Check("invalid preflight cannot write", console->writes == 0U);
    if (!strcmp(name,"no_retry")) {
        okay &= Check("unconfirmed byte not retried", console->writes == 0U);
        okay &= Check("no command was submitted", console->submitted == 0U);
        okay &= Check("no command falsely passed", report.completedSteps == 0U);
    }
    if (!strcmp(name,"stale_boot"))
        okay &= Check("boot text not a result", report.completedSteps == 0U);
    free(console);
    return okay ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const int status = Case(argv[1]);
    if (!status) printf("PASS %s\n",argv[1]);
    return status;
}
