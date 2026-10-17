/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_diagnostic_parser.c
 *
 * PURPOSE:
 *   Implement the test runtime diagnostic parser behavior for
 *   Umicom Framework.
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
#include "umicom/diagnostics/runtime_parser.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiOutputRecord output;
    UmiDiagnosticSnapshot item;
    int matched = 0;
    assert(umi_output_record_init(&output, "debug", "Debug", "runtime",
        UMI_OUTPUT_STREAM_DEBUG, "AddressSanitizer: heap-use-after-free") == UMI_STATUS_OK);
    assert(umi_runtime_diagnostic_parse(&output, &item, &matched, NULL) == UMI_STATUS_OK);
    assert(matched && item.kind == UMI_DIAGNOSTIC_KIND_RUNTIME);
    assert(strcmp(item.code, "address-sanitizer") == 0);

    /* Native toolkit output must reach Problems with its original severity;
     * a critical check does not by itself establish that the process died. */
    const struct {
        const char *text;
        const char *code;
        UmiDiagnosticSeverity severity;
    } native_cases[] = {
        {"(process:1): Gtk-WARNING **: settings property changed", "native-toolkit-warning", UMI_DIAGNOSTIC_WARNING},
        {"(process:2): Gtk-CRITICAL **: assertion failed", "native-toolkit-critical", UMI_DIAGNOSTIC_ERROR},
        {"GLib-GIO-CRITICAL **: index is outside the model", "native-toolkit-critical", UMI_DIAGNOSTIC_ERROR},
        {"GLib-GObject-WARNING **: invalid signal handler", "native-toolkit-warning", UMI_DIAGNOSTIC_WARNING},
        {"Gdk-WARNING **: surface is unavailable", "native-toolkit-warning", UMI_DIAGNOSTIC_WARNING},
        {"Gsk-CRITICAL **: renderer check failed", "native-toolkit-critical", UMI_DIAGNOSTIC_ERROR},
        {"GLib-CRITICAL **: source is already removed", "native-toolkit-critical", UMI_DIAGNOSTIC_ERROR},
        {"assertion failed", "assertion", UMI_DIAGNOSTIC_FATAL}
    };
    for (size_t index = 0U; index < sizeof native_cases / sizeof native_cases[0]; ++index) {
        assert(umi_output_record_init(&output, "native", "Native", "gtk",
            UMI_OUTPUT_STREAM_ERROR, native_cases[index].text) == UMI_STATUS_OK);
        output.correlation_id = 101U + index;
        output.timestamp_ns = 1001U + index;
        matched = 0;
        assert(umi_runtime_diagnostic_parse(&output, &item, &matched, NULL) == UMI_STATUS_OK);
        assert(matched && item.severity == native_cases[index].severity);
        assert(strcmp(item.code, native_cases[index].code) == 0);
        assert(strcmp(item.message, native_cases[index].text) == 0);
        assert(item.correlation_id == output.correlation_id && item.timestamp_ns == output.timestamp_ns);
    }
    const char *ordinary[] = {"Warning counts will be displayed here", "Checking GTK assertions", "Gtk-INFO: starting"};
    for (size_t index = 0U; index < sizeof ordinary / sizeof ordinary[0]; ++index) {
        assert(umi_output_record_init(&output, "native", "Native", "gtk",
            UMI_OUTPUT_STREAM_STANDARD, ordinary[index]) == UMI_STATUS_OK);
        unsigned char before[sizeof item];
        memcpy(before, &item, sizeof item);
        matched = 1;
        assert(umi_runtime_diagnostic_parse(&output, &item, &matched, NULL) == UMI_STATUS_OK);
        assert(!matched && memcmp(&item, before, sizeof item) == 0);
    }
    return 0;
}
