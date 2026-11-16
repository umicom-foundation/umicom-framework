/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/boot_report_json.c
 *
 * PURPOSE:
 *   Stage canonical boot-report JSON above platform and protocol to preserve the dependency direction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/platform/boot_report_json.h"
#include "umicom/protocol/json_writer.h"
#include <string.h>

/* Keep schema presentation above platform: protocol already consumes platform.
 * Both the SDK and OS use this one serializer, while validation and comparison
 * retain their existing platform implementation and public contracts. */
static void ReportJson(UmiJsonWriter *writer, const UmiBootReport *report)
{
    UmiJsonWriterRaw(writer, "{\"mode\":");
    UmiJsonWriterString(writer, report->mode == UMI_BOOT_REPORT_NORMAL ? "normal" : "recovery");
    UmiJsonWriterRaw(writer, ",\"state\":");
    UmiJsonWriterString(writer, UmiBootReportStateText(report->state));
    UmiJsonWriterRaw(writer, ",\"planned\":");
    UmiJsonWriterUInt64(writer, report->planned);
    UmiJsonWriterRaw(writer, ",\"completed\":");
    UmiJsonWriterUInt64(writer, report->completed);
    UmiJsonWriterRaw(writer, ",\"reason\":");
    UmiJsonWriterString(writer, report->reason);
    UmiJsonWriterRaw(writer, ",\"source\":");
    UmiJsonWriterString(writer, report->sourceId);
    UmiJsonWriterRaw(writer, "}");
}
static UmiStatus PublishJson(const UmiJsonWriter *writer,
    char *out, size_t capacity, size_t *out_length)
{
    if (writer->status != UMI_STATUS_OK) return writer->status;
    if (writer->length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, writer->buffer, writer->length + 1U);
    *out_length = writer->length;
    return UMI_STATUS_OK;
}
UmiStatus UmiBootReportJson(const UmiBootReport *report,
    char *out, size_t capacity, size_t *out_length)
{
    if (!out || !out_length || UmiBootReportValidate(report) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    char staged[UMI_BOOT_REPORT_JSON_CAPACITY];
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, staged, sizeof staged);
    UmiJsonWriterRaw(&writer, "{\"schema_version\":1,\"scope\":\"captured-boot-observation\",\"report\":");
    ReportJson(&writer, report);
    UmiJsonWriterRaw(&writer, "}");
    return PublishJson(&writer, out, capacity, out_length);
}
UmiStatus UmiBootReportComparisonJson(const UmiBootReport *before,
    const UmiBootReport *after, char *out, size_t capacity, size_t *out_length)
{
    if (!out || !out_length) return UMI_STATUS_INVALID_ARGUMENT;
    uint32_t changes = 0U;
    UmiStatus status = UmiBootReportCompare(before, after, &changes);
    if (status != UMI_STATUS_OK) return status;
    char staged[UMI_BOOT_REPORT_JSON_CAPACITY];
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, staged, sizeof staged);
    UmiJsonWriterRaw(&writer, "{\"schema_version\":1,\"scope\":\"captured-boot-comparison\",\"same_source\":");
    UmiJsonWriterBool(&writer, (changes & UMI_BOOT_REPORT_CHANGED_SOURCE) == 0U);
    UmiJsonWriterRaw(&writer, ",\"changed_fields\":[");
    static const struct { uint32_t mask; const char *name; } fields[] = {
        {UMI_BOOT_REPORT_CHANGED_MODE, "mode"}, {UMI_BOOT_REPORT_CHANGED_STATE, "state"},
        {UMI_BOOT_REPORT_CHANGED_PLANNED, "planned"}, {UMI_BOOT_REPORT_CHANGED_COMPLETED, "completed"},
        {UMI_BOOT_REPORT_CHANGED_REASON, "reason"}, {UMI_BOOT_REPORT_CHANGED_SOURCE, "source"}
    };
    int emitted = 0;
    for (size_t i = 0U; i < sizeof fields / sizeof fields[0]; ++i) {
        if ((changes & fields[i].mask) == 0U) continue;
        if (emitted) UmiJsonWriterRaw(&writer, ",");
        UmiJsonWriterString(&writer, fields[i].name);
        emitted = 1;
    }
    UmiJsonWriterRaw(&writer, "],\"before\":");
    ReportJson(&writer, before);
    UmiJsonWriterRaw(&writer, ",\"after\":");
    ReportJson(&writer, after);
    UmiJsonWriterRaw(&writer, "}");
    return PublishJson(&writer, out, capacity, out_length);
}
