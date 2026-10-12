/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/education_inspection/main.c
 * PURPOSE: Show how an application consumes the installed inspection library.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <umicom/education/archive_inspect.h>
#include <umicom/education/binary_inspect.h>
#include <umicom/education/elf_symbols.h>
#include <umicom/education/elf_load_plan.h>
#include <umicom/education/memory_audit.h>
#include <umicom/education/ipc_trace.h>
#include <umicom/education/schedule_trace.h>
#include <umicom/education/timer_trace.h>
#include <umicom/education/interrupt_trace.h>
#include <umicom/education/evidence_trace.h>
#include <umicom/education/device_io_trace.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    /* The caller chooses both the inspection and its input. No extension
     * guessing, directory scan, extraction or executable launch is needed. */
    if (argc != 3 || argv[2][0] == '\0') {
        fputs("Usage: umicom-education-inspect-example "
              "binary|archive|symbols|load-plan|memory-audit|ipc-trace FILE\n", stderr);
        fputs("Device requests: device-io-trace FILE\n", stderr);
        fputs("Teaching formats: schedule-trace|timer-trace|interrupt-trace|evidence-trace FILE\n", stderr);
        return 2;
    }
    const char *const operation = argv[1];
    const char *const path = argv[2];
    UmiStatus status;
    int reviewFailed = 0;
    if (strcmp(operation, "binary") == 0) {
        /* A successful header inspection describes metadata, not whether
         * a complete executable is safe or suitable for this host. */
        UmiEducationBinaryHeader report = {0};
        status = UmiEducationBinaryInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("Format: %s\nMachine: %s\nFile bytes: %" PRIu64 "\n",
                UmiEducationBinaryFormatText(report.format),
                UmiEducationBinaryMachineText(&report), report.fileBytes);
    } else if (strcmp(operation, "archive") == 0) {
        /* Archive members are counted in place. The library never extracts
         * their names as paths or runs the object files they contain. */
        UmiEducationArchiveReport report = {0};
        status = UmiEducationArchiveInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("Archive members: %" PRIu32 "\nObject members: %" PRIu32 "\n",
                report.memberCount, report.objectCount);
    } else if (strcmp(operation, "symbols") == 0) {
        /* Only a bounded selection of names is retained in the report;
         * the counters describe every accepted symbol table entry. */
        UmiEducationElfReport report = {0};
        status = UmiEducationElfInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("ELF symbols: %" PRIu64 "\nRelocations: %" PRIu64 "\n",
                report.symbolCount, report.relocationCount);
    } else if (strcmp(operation, "load-plan") == 0) {
        /* This is a read-only explanation of segment ranges. It does not
         * map memory or replace the independent Kernel's executable loader. */
        UmiEducationElfLoadPlan report = {0};
        status = UmiEducationElfLoadPlanInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("Load segments: %" PRIu32 "\nZero-fill bytes: %" PRIu64 "\n",
                report.segmentCount, report.zeroFillBytes);
    } else if (strcmp(operation, "memory-audit") == 0) {
        /* Valid metadata may still describe writable executable pages. Return
         * a failed review for that finding, without treating it as a parse error. */
        UmiEducationMemoryAudit report = {0};
        status = UmiEducationMemoryAuditFile(path, &report);
        if (status == UMI_STATUS_OK) {
            printf("Writable executable pages: %" PRIu64 "\nUnknown permission segments: %" PRIu32 "\n",
                report.writeExecutePages, report.unknownPermissionSegments);
            reviewFailed = report.writeExecutePages != 0U ||
                report.unknownPermissionSegments != 0U;
        }
    } else if (strcmp(operation, "ipc-trace") == 0) {
        /* This file format models message ownership for teaching. It does
         * not establish a live IPC connection or authenticate real processes. */
        UmiEducationIpcTraceReport report = {0};
        status = UmiEducationIpcTraceInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("IPC records: %" PRIu32 "\nPending messages: %" PRIu32 "\n",
                report.recordCount, report.pendingCount);
    } else if (strcmp(operation, "schedule-trace") == 0) {
        /* Teaching records describe simulated ownership. Inspection never
         * executes a task, changes hardware or approves real recovery. */
        UmiEducationScheduleReport report = {0};
        status = UmiEducationScheduleTraceInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("Schedule records: %" PRIu32 "\nReady tasks: %" PRIu32 "\n",
                report.recordCount, report.readyAtEnd);
    } else if (strcmp(operation, "timer-trace") == 0) {
        /* Teaching records describe simulated ownership. Inspection never
         * executes a task, changes hardware or approves real recovery. */
        UmiEducationTimerReport report = {0};
        status = UmiEducationTimerTraceInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("Timer records: %" PRIu32 "\nPending timers: %" PRIu32 "\n",
                report.recordCount, report.pendingAtEnd);
    } else if (strcmp(operation, "interrupt-trace") == 0) {
        /* Teaching records describe simulated ownership. Inspection never
         * executes a task, changes hardware or approves real recovery. */
        UmiEducationInterruptReport report = {0};
        status = UmiEducationInterruptTraceInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("Interrupt records: %" PRIu32 "\nOwned interrupt sources: %" PRIu32 "\n",
                report.recordCount, report.ownedAtEnd);
    } else if (strcmp(operation, "evidence-trace") == 0) {
        /* Teaching records describe simulated ownership. Inspection never
         * executes a task, changes hardware or approves real recovery. */
        UmiEducationEvidenceReport report = {0};
        status = UmiEducationEvidenceTraceInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("Evidence records: %" PRIu32 "\nLive driver records: %" PRIu32 "\n",
                report.recordCount, report.liveAtEnd);
    } else if (strcmp(operation, "device-io-trace") == 0) {
        /* Cancellation requests are intent; only completion records acknowledge
         * a result. This SDK consumer prints both without touching a device. */
        UmiEducationDeviceIoReport report = {0};
        status = UmiEducationDeviceIoTraceInspectFile(path, &report);
        if (status == UMI_STATUS_OK)
            printf("Device I/O records: %" PRIu32 "\nCancellation requests: %" PRIu32
                "\nAcknowledged cancellations: %" PRIu32 "\nOutstanding: %" PRIu32 "\n",
                report.recordCount, report.cancellationRequests,
                report.cancellationAcknowledgements, report.outstandingAtEnd);
    } else {
        fputs("Also supported: device-io-trace FILE\n", stderr);
        fputs("Additional teaching inspections: schedule-trace, timer-trace, interrupt-trace, evidence-trace.\n", stderr);
        fputs("Unknown inspection. Use binary, archive, symbols, load-plan, memory-audit or ipc-trace.\n",
            stderr);
        return 2;
    }
    /* Do not display a previous or partially populated report as new evidence.
     * Hosts can translate the public status code into their own diagnostic UI. */
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Inspection failed (status %d). Check the path, format and documented size limits.\n",
            (int)status);
        return 1;
    }
    return reviewFailed ? 1 : 0;
}
