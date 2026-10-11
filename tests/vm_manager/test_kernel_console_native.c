/*-----------------------------------------------------------------------------
 * Umicom Framework - Native QMP console adapter acceptance tests
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 *
 * Only the QMP facade and clock are replaced by in-memory test doubles.
 * console_journey_native.c and console_journey.c are the real Framework C23
 * sources, compiled together into this test. No host process is spawned.
 *---------------------------------------------------------------------------*/
#include "umicom/vm_manager/boot.h"
#include "umicom/vm_manager/console_journey.h"

#include <stdio.h>
#include <string.h>

struct UmiVmSession {
    unsigned marker;
};

typedef struct QmpSimulation {
    unsigned char bytes[8192];
    size_t front, end;
    size_t written;
    size_t submitted;
    size_t reads;
    size_t largestReply;
    int failWrite;
} QmpSimulation;

static QmpSimulation mock;
static uint64_t clockValue;

uint64_t VmClock(void)
{
    return ++clockValue;
}

static void Feed(const char *text, size_t length)
{
    if (length <= sizeof mock.bytes - mock.end) {
        memcpy(mock.bytes + mock.end, text, length);
        mock.end += length;
    }
}

UmiStatus UmiVmControl(UmiVmSession *session, UmiVmCommand command,
    const void *input, size_t length, void *output, size_t capacity,
    size_t *outLength, UmiVmReport *report)
{
    (void)report;
    if (!session || session->marker != 0xC0DEU) return UMI_STATUS_INVALID_ARGUMENT;
    if (command == UMI_VM_CONSOLE_READ) {
        if (!output || !outLength || capacity < UMI_VM_CONSOLE_CHUNK)
            return UMI_STATUS_INVALID_ARGUMENT;
        size_t available = mock.end - mock.front;
        if (available > capacity) available = capacity;
        if (available > mock.largestReply) mock.largestReply = available;
        if (available) memcpy(output, mock.bytes + mock.front, available);
        mock.front += available;
        *outLength = available;
        ++mock.reads;
        return UMI_STATUS_OK;
    }
    if (command != UMI_VM_CONSOLE_WRITE || mock.failWrite || !input || length != 1U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const unsigned char byte = *(const unsigned char *)input;
    ++mock.written;
    if (byte != 13U) {
        Feed((const char *)&byte, 1U);
        return UMI_STATUS_OK;
    }
    static const char *const responses[] = {
        "\r\nUmicom Kernel development console\r\numicom> ",
        "\r\nfile README bytes=23\r\numicom> ",
        "\r\ndisk.mount-state=unused\r\numicom> "
    };
    if (mock.submitted >= sizeof responses / sizeof responses[0])
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *text = responses[mock.submitted++];
    Feed(text, strlen(text));
    return UMI_STATUS_OK;
}

int main(void)
{
    memset(&mock, 0, sizeof mock);
    clockValue = 0U;
    /* One QMP ringbuf-read returns a large burst. The native adapter must
     * retain the tail while its bounded protocol consumes 256-byte chunks. */
    char boot[1900];
    memset(boot, 'B', sizeof boot);
    Feed(boot, sizeof boot);
    Feed("\r\numicom> ", 10U);
    UmiVmSession session = {0xC0DEU};
    static const UmiVmConsoleJourneyStep steps[] = {
        {"cat /README", "Umicom Kernel development console"},
        {"ls /", "README"},
        {"mountinfo", "disk.mount-state=unused"}
    };
    UmiVmConsoleJourneyReport report;
    UmiVmConsoleJourneyStatus result = UmiVmConsoleJourneyRunNative(&session,
        steps, 3U, 45000U, &report);
    if (result != UMI_VM_CONSOLE_JOURNEY_OK || report.completedSteps != 3U ||
        report.failedStep != 3U || mock.submitted != 3U || mock.largestReply < 1800U) {
        fprintf(stderr, "QMP pending-buffer qualification failed: %s\n", report.detail);
        return 1;
    }
    /* A write error cannot trigger an automatic retry of a partially
     * delivered command; all resources stay owned by the caller. */
    memset(&mock, 0, sizeof mock);
    clockValue = 0U;
    Feed("umicom> ", 8U);
    mock.failWrite = 1;
    result = UmiVmConsoleJourneyRunNative(&session, steps, 3U, 45000U, &report);
    if (result != UMI_VM_CONSOLE_JOURNEY_TRANSPORT_FAILURE ||
        mock.written != 0U || report.completedSteps != 0U) {
        fprintf(stderr, "QMP failure admission regression: %s\n", report.detail);
        return 1;
    }
    puts("PASS native QMP pending-buffer retention and one-shot failure behaviour");
    return 0;
}
