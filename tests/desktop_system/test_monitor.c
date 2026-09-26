/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_system/test_monitor.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/monitor.h"
#include "umicom/desktop_system/linux_parse.h"
#include "umicom/desktop_system/session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return 1; } } while (0)
static const char *MEMORY = "MemTotal: 1000 kB\nMemFree: 5 kB\nMemAvailable: 400 kB\n";
static int Memory(const char *which)
{
    UmiDesktopSystemMemory value = {17, 19}, original = value; UmiStatus status;
    if (!strcmp(which, "memory-valid")) { CHECK(UmiDesktopSystemParseMemory(MEMORY, strlen(MEMORY), &value) == UMI_STATUS_OK); CHECK(value.totalBytes == 1024000 && value.availableBytes == 409600); return 0; }
    const char *text = NULL;
    if (!strcmp(which, "memory-missing")) text = "MemTotal: 1000 kB\nMemFree: 400 kB\n";
    if (!strcmp(which, "memory-duplicate")) text = "MemTotal: 1000 kB\nMemAvailable: 200 kB\nMemAvailable: 100 kB\n";
    if (!strcmp(which, "memory-units")) text = "MemTotal: 1000 MB\nMemAvailable: 400 kB\n";
    if (!strcmp(which, "memory-range")) text = "MemTotal: 100 kB\nMemAvailable: 400 kB\n";
    if (!strcmp(which, "memory-overflow")) text = "MemTotal: 18446744073709551615 kB\nMemAvailable: 1 kB\n";
    if (!strcmp(which, "memory-negative")) text = "MemTotal: -100 kB\nMemAvailable: 1 kB\n";
    if (!strcmp(which, "memory-zero")) text = "MemTotal: 0 kB\nMemAvailable: 0 kB\n";
    CHECK(text); status = UmiDesktopSystemParseMemory(text, strlen(text), &value); CHECK(status != UMI_STATUS_OK);
    CHECK(!memcmp(&value, &original, sizeof value)); return 0;
}
static int Cpu(const char *which)
{
    UmiDesktopSystemCpu a = {{0}}, b = {{0}}; uint32_t result = 777;
    if (!strcmp(which, "cpu-guest")) {
        const char *text = "cpu  10 20 30 40 50 60 70 80 900 901\ncpu0 1 2 3 4\n";
        CHECK(UmiDesktopSystemParseCpu(text, strlen(text), &a) == UMI_STATUS_OK);
        for (size_t i = 0; i < 8; ++i) { CHECK(a.ticks[i] == (i + 1) * 10); }
        return 0;
    }
    if (!strcmp(which, "cpu-bad")) {
        const char *bad[] = {"cpu0 1 2 3 4", "cpu 1 2 3", "cpu 1 2 3 -4", "cpu 1 2 3 4x", "cpu 1 2 3 4 5 6 7 8 9 10 11", "cpu 18446744073709551616 2 3 4"};
        for (size_t i = 0; i < sizeof bad / sizeof bad[0]; ++i) { CHECK(UmiDesktopSystemParseCpu(bad[i], strlen(bad[i]), &a) != UMI_STATUS_OK); }
        return 0;
    }
    if (!strcmp(which, "cpu-ratio")) { b.ticks[0] = 30; b.ticks[3] = 60; b.ticks[4] = 10; CHECK(UmiDesktopSystemCpuUsage(&a, &b, &result) == UMI_STATUS_OK); CHECK(result == 3000); return 0; }
    if (!strcmp(which, "cpu-extreme")) { b.ticks[0] = UINT64_MAX - 1; b.ticks[3] = 1; CHECK(UmiDesktopSystemCpuUsage(&a, &b, &result) == UMI_STATUS_OK); CHECK(result == 9999); return 0; }
    if (!strcmp(which, "cpu-reset")) { a.ticks[4] = 10; b.ticks[0] = 50; b.ticks[4] = 9; CHECK(UmiDesktopSystemCpuUsage(&a, &b, &result) == UMI_STATUS_INVALID_STATE); }
    else if (!strcmp(which, "cpu-zero")) CHECK(UmiDesktopSystemCpuUsage(&a, &b, &result) == UMI_STATUS_UNAVAILABLE);
    else if (!strcmp(which, "cpu-overflow")) { b.ticks[0] = UINT64_MAX; b.ticks[1] = 1; CHECK(UmiDesktopSystemCpuUsage(&a, &b, &result) == UMI_STATUS_CAPACITY_EXCEEDED); }
    else CHECK(0);
    CHECK(result == 777); return 0;
}
static void ProcessText(char *text, size_t capacity, const char *name, const char *rss)
{
    (void)snprintf(text, capacity, "123 (%s) S 1 2 3 4 5 6 7 8 9 10 11 12 13 -14 15 16 17 18 777 888 %s 0 0\n", name, rss);
}
static int Process(const char *which)
{
    char text[1024]; UmiDesktopSystemProcess value = {0}, original;
    ProcessText(text, sizeof text, "Umicom (Notes) helper)", "3");
    if (!strcmp(which, "process-valid")) {
        CHECK(UmiDesktopSystemParseProcess(text, strlen(text), 4096, &value) == UMI_STATUS_OK);
        CHECK(value.pid == 123 && value.parentPid == 1 && value.startTicks == 777 && value.residentBytes == 12288 && value.residentKnown);
        CHECK(!strcmp(value.name, "Umicom (Notes) helper)")); return 0;
    }
    if (!strcmp(which, "process-controls")) {
        ProcessText(text, sizeof text, "note\033[31m\n", "1");
        CHECK(UmiDesktopSystemParseProcess(text, strlen(text), 4096, &value) == UMI_STATUS_OK);
        CHECK(!strchr(value.name, '\033') && !strchr(value.name, '\n')); return 0;
    }
    original = value;
    if (!strcmp(which, "process-overflow")) ProcessText(text, sizeof text, "notes", "18446744073709551615");
    else if (!strcmp(which, "process-negative")) ProcessText(text, sizeof text, "notes", "-1");
    else if (!strcmp(which, "process-short")) strcpy(text, "123 (notes) S 1 2 3");
    else if (!strcmp(which, "process-nul")) { text[6] = 0; CHECK(UmiDesktopSystemParseProcess(text, 50, 4096, &value) != UMI_STATUS_OK); return 0; }
    else CHECK(0);
    CHECK(UmiDesktopSystemParseProcess(text, strlen(text), 4096, &value) != UMI_STATUS_OK);
    CHECK(!memcmp(&value, &original, sizeof value)); return 0;
}
static int Network(const char *which)
{
    const char *text = " eth0: 1000 2 3 4 5 6 7 8 9000 10 11 12 13 14 15 16";
    UmiDesktopSystemInterface value = {0};
    CHECK(UmiDesktopSystemParseInterface(text, strlen(text), &value) == UMI_STATUS_OK);
    CHECK(!strcmp(value.name, "eth0") && value.receivedBytes == 1000 && value.transmittedBytes == 9000);
    if (!strcmp(which, "network-valid")) return 0;
    UmiDesktopSystemInterface original = value;
    const char *bad[] = {"eth0: 1 2", "a b: 1 2 3", " : 1 2 3", "eth0: -1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16", "eth0: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17"};
    for (size_t i = 0; i < sizeof bad / sizeof bad[0]; ++i) { CHECK(UmiDesktopSystemParseInterface(bad[i], strlen(bad[i]), &value) != UMI_STATUS_OK); CHECK(!memcmp(&value, &original, sizeof value)); }
    return 0;
}
static int Json(const char *which)
{
    UmiDesktopSystemSnapshot *s = calloc(1, sizeof *s); CHECK(s); strcpy(s->source, "Source \"quoted\"\n\033 /\xc3\xa9");
    strcpy(s->storage.path, "/"); s->bootStatus = UMI_STATUS_NOT_FOUND; s->processCount = 1;
    s->processes[0].pid = UINT64_MAX; strcpy(s->processes[0].name, "Umicom \\ Notes");
    size_t required = 0; CHECK(UmiDesktopSystemJson(s, NULL, 0, &required) == UMI_STATUS_OK);
    char *text = malloc(required + 1); CHECK(text); memset(text, '!', required + 1);
    if (!strcmp(which, "json-capacity")) {
        CHECK(UmiDesktopSystemJson(s, text, required - 1, &required) == UMI_STATUS_CAPACITY_EXCEEDED); CHECK(text[0] == 0 && text[required] == '!');
    } else if (!strcmp(which, "json-invalid")) {
        s->processCount = UMI_DESKTOP_SYSTEM_PROCESS_LIMIT + 1;
        CHECK(UmiDesktopSystemJson(s, text, required, &required) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        CHECK(UmiDesktopSystemJson(s, text, required, &required) == UMI_STATUS_OK);
        CHECK(strlen(text) + 1 == required && text[required] == '!');
        CHECK(strstr(text, "\\u00e9") && strstr(text, "\\u001b") && strstr(text, "18446744073709551615"));
        CHECK(strstr(text, "\"residentBytes\":null"));
        if (!strcmp(which, "json-output")) fputs(text, stdout);
    }
    free(text); free(s); return 0;
}
static int Session(const char *which)
{
    UmiDesktopSessionRequest r = {"/opt/umicom/bin/umicom-desk", "/home/learner", "/run/user/1000", "wayland-0", NULL, NULL, "unix:path=/run/user/1000/bus", "en_GB.UTF-8"};
    UmiDesktopSessionCheck check;
    if (!strcmp(which, "session-wayland")) { CHECK(UmiDesktopSessionValidate(&r, &check) == UMI_STATUS_OK); CHECK(check.backend == UMI_DESKTOP_SESSION_WAYLAND); return 0; }
    if (!strcmp(which, "session-x11")) { r.waylandDisplay = NULL; r.display = ":0.1"; CHECK(UmiDesktopSessionValidate(&r, &check) == UMI_STATUS_OK); CHECK(check.backend == UMI_DESKTOP_SESSION_X11); return 0; }
    if (!strcmp(which, "session-path")) r.deskPath = "./umicom-desk";
    else if (!strcmp(which, "session-socket")) r.waylandDisplay = "../../socket";
    else if (!strcmp(which, "session-remote-x11")) { r.waylandDisplay = NULL; r.display = "remote:0"; }
    else if (!strcmp(which, "session-remote-bus")) r.sessionBus = "tcp:host=localhost,port=42";
    else if (!strcmp(which, "session-injection")) r.home = "/home/a\nDISPLAY=:1";
    else if (!strcmp(which, "session-missing-display")) { r.waylandDisplay = NULL; r.display = NULL; }
    else CHECK(0);
    CHECK(UmiDesktopSessionValidate(&r, &check) != UMI_STATUS_OK && check.explanation[0]); return 0;
}
static int RatioStream(void)
{
    uint64_t part, total;
    while (scanf("%" SCNu64 " %" SCNu64, &part, &total) == 2) {
        if (part > total) return 2;
        UmiDesktopSystemCpu a = {{0}}, b = {{0}}; b.ticks[0] = part; b.ticks[3] = total - part;
        uint32_t ratio = 777; UmiStatus status = UmiDesktopSystemCpuUsage(&a, &b, &ratio);
        printf("%d %u\n", (int)status, ratio);
    }
    return ferror(stdin) ? 1 : 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *which = argv[1];
    if (!strcmp(which, "skip")) return 77;
    if (!strcmp(which, "ratio-stream")) return RatioStream();
    if (!strncmp(which, "memory-", 7)) return Memory(which);
    if (!strncmp(which, "cpu-", 4)) return Cpu(which);
    if (!strncmp(which, "process-", 8)) return Process(which);
    if (!strncmp(which, "network-", 8)) return Network(which);
    if (!strncmp(which, "json-", 5)) return Json(which);
    if (!strncmp(which, "session-", 8)) return Session(which);
    if (!strcmp(which, "invalid-capture")) {
        UmiDesktopSystemOptions options = {"relative", NULL, NULL};
        UmiDesktopSystemSnapshot *s = calloc(1, sizeof *s); CHECK(s); s->capturedMilliseconds = 123;
        CHECK(UmiDesktopSystemCapture(&options, s) == UMI_STATUS_INVALID_ARGUMENT); CHECK(s->capturedMilliseconds == 123); free(s); return 0;
    }
    fprintf(stderr, "Unknown case: %s\n", which); return 2;
}
