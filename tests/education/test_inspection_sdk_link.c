/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_inspection_sdk_link.c
 * PURPOSE: Link every inspector family through the installed SDK interface.
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
#include <stdio.h>

/* Rejected arguments are intentional: this small consumer proves that all six
 * implementation families can be linked using only installed public headers.
 * Their valid-input behaviour is covered by the shared native parser tests. */
int main(void)
{
    /* Include the classroom trace readers in the installed-link proof. */
    const UmiStatus statuses[] = {
        UmiEducationScheduleTraceInspectFile(NULL, NULL),
        UmiEducationTimerTraceInspectFile(NULL, NULL),
        UmiEducationInterruptTraceInspectFile(NULL, NULL),
        UmiEducationEvidenceTraceInspectFile(NULL, NULL),
        UmiEducationBinaryInspectFile(NULL, NULL),
        UmiEducationArchiveInspectFile(NULL, NULL),
        UmiEducationElfInspectFile(NULL, NULL),
        UmiEducationElfLoadPlanInspectFile(NULL, NULL),
        UmiEducationMemoryAuditFile(NULL, NULL),
        UmiEducationIpcTraceInspectFile(NULL, NULL)
    };
    for (size_t index = 0U; index < sizeof(statuses) / sizeof(statuses[0]); ++index) {
        if (statuses[index] != UMI_STATUS_INVALID_ARGUMENT) {
            fprintf(stderr, "Inspector %zu rejected invalid arguments with unexpected status %d.\n",
                index, (int)statuses[index]);
            return 1;
        }
    }
    puts("[OK] Installed education inspection interfaces linked.");
    return 0;
}
