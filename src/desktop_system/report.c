/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/report.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/monitor.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct Writer { char *out; size_t capacity, used; int failed; } Writer;
static void Bytes(Writer *w, const char *text, size_t length)
{
    if (length > SIZE_MAX - w->used) { w->failed = 1; return; }
    if (w->out && w->used < w->capacity && length < w->capacity - w->used)
        memcpy(w->out + w->used, text, length);
    w->used += length;
}
static void Text(Writer *w, const char *text) { Bytes(w, text, strlen(text)); }
static void Format(Writer *w, const char *format, ...)
{
    char text[160]; va_list args; va_start(args, format);
    int n = vsnprintf(text, sizeof text, format, args); va_end(args);
    if (n < 0 || (size_t)n >= sizeof text) { w->failed = 1; return; }
    Bytes(w, text, (size_t)n);
}
static int Terminated(const char *s, size_t capacity) { return memchr(s, 0, capacity) != NULL; }
static void String(Writer *w, const char *text)
{
    Text(w, "\""); const unsigned char *p = (const unsigned char *)text;
    while (*p) {
        unsigned c = *p++;
        if (c == '"' || c == '\\') { char pair[] = {'\\', (char)c}; Bytes(w, pair, 2); }
        else if (c < 32 || c == 127) Format(w, "\\u%04x", c);
        else if (c < 128) { char ch = (char)c; Bytes(w, &ch, 1); }
        else {
            /* Decode UTF-8 before escaping, so a Unicode path retains its
             * meaning. Invalid byte sequences are shown as U+FFFD, never JSON
             * syntax or an invalid string. */
            unsigned cp = 0, rest = 0, minimum = 0;
            if (c >= 0xc2 && c <= 0xdf) { cp = c & 31U; rest = 1; minimum = 0x80; }
            else if (c >= 0xe0 && c <= 0xef) { cp = c & 15U; rest = 2; minimum = 0x800; }
            else if (c >= 0xf0 && c <= 0xf4) { cp = c & 7U; rest = 3; minimum = 0x10000; }
            unsigned i = 0; const unsigned char *q = p;
            for (; i < rest && *q && (*q & 0xc0U) == 0x80U; ++i, ++q) cp = (cp << 6) | (*q & 63U);
            if (!rest || i != rest || cp < minimum || cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) Text(w, "\\ufffd");
            else {
                p = q;
                if (cp <= 0xffff) Format(w, "\\u%04x", cp);
                else { cp -= 0x10000; Format(w, "\\u%04x\\u%04x", 0xd800U + (cp >> 10), 0xdc00U + (cp & 1023U)); }
            }
        }
    }
    Text(w, "\"");
}
static int Valid(const UmiDesktopSystemSnapshot *s)
{
    if (!s || !Terminated(s->source, sizeof s->source) || !Terminated(s->storage.path, sizeof s->storage.path) ||
        !Terminated(s->boot.reason, sizeof s->boot.reason) || !Terminated(s->boot.sourceId, sizeof s->boot.sourceId) ||
        s->processCount > UMI_DESKTOP_SYSTEM_PROCESS_LIMIT || s->interfaceCount > UMI_DESKTOP_SYSTEM_INTERFACE_LIMIT) return 0;
    for (size_t i = 0; i < s->processCount; ++i) if (!Terminated(s->processes[i].name, sizeof s->processes[i].name)) return 0;
    for (size_t i = 0; i < s->interfaceCount; ++i) if (!Terminated(s->interfaces[i].name, sizeof s->interfaces[i].name)) return 0;
    return 1;
}
static void Render(Writer *w, const UmiDesktopSystemSnapshot *s)
{
    Text(w, "{\"schema\":\"umicom.desktop-system\",\"source\":"); String(w, s->source);
    Format(w, ",\"fixture\":%s,\"capturedMilliseconds\":\"%" PRIu64 "\",\"elapsedMilliseconds\":\"%" PRIu64 "\",",
        s->fixture ? "true" : "false", s->capturedMilliseconds, s->elapsedMilliseconds);
    Format(w, "\"memory\":{\"status\":%d,\"totalBytes\":\"%" PRIu64 "\",\"availableBytes\":\"%" PRIu64 "\"},",
        (int)s->memoryStatus, s->memory.totalBytes, s->memory.availableBytes);
    Format(w, "\"cpu\":{\"status\":%d,\"ticks\":[", (int)s->cpuStatus);
    for (size_t i = 0; i < 8; ++i) Format(w, "%s\"%" PRIu64 "\"", i ? "," : "", s->cpu.ticks[i]);
    Format(w, "]},\"processes\":{\"status\":%d,\"seen\":%zu,\"unreadable\":%zu,\"listed\":%zu,\"items\":[",
        (int)s->processStatus, s->processesSeen, s->processesUnreadable, s->processCount);
    for (size_t i = 0; i < s->processCount; ++i) {
        const UmiDesktopSystemProcess *p = &s->processes[i]; if (i) Text(w, ",");
        Format(w, "{\"pid\":\"%" PRIu64 "\",\"parentPid\":\"%" PRIu64 "\",\"startTicks\":", p->pid, p->parentPid);
        if (p->startKnown) Format(w, "\"%" PRIu64 "\"", p->startTicks); else Text(w, "null");
        Text(w, ",\"residentBytes\":");
        if (p->residentKnown) Format(w, "\"%" PRIu64 "\"", p->residentBytes); else Text(w, "null");
        Text(w, ",\"name\":"); String(w, p->name); Text(w, ",\"state\":");
        char state[] = {p->state ? p->state : '?', 0}; String(w, state); Text(w, "}");
    }
    Format(w, "]},\"network\":{\"status\":%d,\"seen\":%zu,\"listed\":%zu,\"items\":[",
        (int)s->networkStatus, s->interfacesSeen, s->interfaceCount);
    for (size_t i = 0; i < s->interfaceCount; ++i) {
        const UmiDesktopSystemInterface *n = &s->interfaces[i]; if (i) Text(w, ",");
        Text(w, "{\"name\":"); String(w, n->name);
        Format(w, ",\"receivedBytes\":\"%" PRIu64 "\",\"transmittedBytes\":\"%" PRIu64 "\"}", n->receivedBytes, n->transmittedBytes);
    }
    Format(w, "]},\"storage\":{\"status\":%d,\"path\":", (int)s->storageStatus); String(w, s->storage.path);
    Format(w, ",\"totalBytes\":\"%" PRIu64 "\",\"availableBytes\":\"%" PRIu64 "\"},\"boot\":{\"status\":%d,\"state\":",
        s->storage.totalBytes, s->storage.availableBytes, (int)s->bootStatus);
    if (s->bootStatus == UMI_STATUS_OK) String(w, UmiBootReportStateText(s->boot.state)); else Text(w, "null");
    Format(w, ",\"planned\":%u,\"completed\":%u,\"reason\":", s->boot.planned, s->boot.completed);
    String(w, s->boot.reason); Text(w, ",\"sourceId\":"); String(w, s->boot.sourceId); Text(w, "}}\n");
}
UmiStatus UmiDesktopSystemJson(const UmiDesktopSystemSnapshot *snapshot,
    char *outText, size_t capacity, size_t *outRequired)
{
    if (outText && capacity) outText[0] = 0;
    if (!outRequired || !Valid(snapshot) || (!outText && capacity)) return UMI_STATUS_INVALID_ARGUMENT;
    Writer measure = {0}; Render(&measure, snapshot);
    if (measure.failed || measure.used == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outRequired = measure.used + 1;
    if (!outText && capacity == 0) return UMI_STATUS_OK;
    if (capacity <= measure.used) return UMI_STATUS_CAPACITY_EXCEEDED;
    Writer write = {outText, capacity, 0, 0}; Render(&write, snapshot);
    if (write.failed || write.used != measure.used) { outText[0] = 0; return UMI_STATUS_INTERNAL_ERROR; }
    outText[write.used] = 0; return UMI_STATUS_OK;
}
