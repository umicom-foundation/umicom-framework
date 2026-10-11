/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/memory_audit.h
 *
 * PURPOSE:
 *   Explain how independently declared ELF PT_LOAD byte ranges occupy the
 *   same 4 KiB virtual-memory pages, and identify pages with write-and-execute
 *   permissions before a real loader, process, or kernel is involved.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_MEMORY_AUDIT_H
#define UMICOM_EDUCATION_MEMORY_AUDIT_H

#include "umicom/education/elf_load_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_MEMORY_PAGE_BYTES UINT64_C(4096)

/* These are page-set counts, not quantities of resident RAM. Two segments
 * covering one page contribute ONE to mappedPages. A shared page is counted
 * once, regardless of how many segments reference it. */
typedef struct UmiEducationMemoryAudit {
    uint64_t mappedPages;
    uint64_t readablePages;
    uint64_t writablePages;
    uint64_t executablePages;
    uint64_t writeExecutePages;
    uint64_t sharedPages;
    uint64_t mixedPermissionPages;
    uint32_t loadSegments;
    uint32_t writableExecutableSegments;
    uint32_t unknownPermissionSegments;
    uint16_t elfType;
    uint16_t machine;
    uint8_t addressBits;
} UmiEducationMemoryAudit;

/* Construct a bounded, deterministic page-permission report for an ELF load
 * plan. Does NOT create an address space, grant access, load bytes, or execute
 * anything. Rounds byte ranges to 4 KiB, then sweeps at most 128 boundaries;
 * even enormous virtual ranges require no proportional allocation.
 *
 * A page shared by RX and RW segments is marked W+X because effective page
 * protections cannot honour both independent intentions simultaneously.
 * Unknown ELF permission bits are reported, not silently discarded.
 *
 * Invalid or inconsistent input leaves outReport completely unchanged.
 * This is a teaching-oriented conservative review, not OS loader approval.
 */
UmiStatus UmiEducationMemoryAuditPlan(const UmiEducationElfLoadPlan *plan,
                                      UmiEducationMemoryAudit *outReport);

/* Read-only ELF file wrapper built on the canonical Framework ELF load-plan
 * parser. Errors preserve outReport; the executable is never launched. */
UmiStatus UmiEducationMemoryAuditFile(const char *file,
                                      UmiEducationMemoryAudit *outReport);

#ifdef __cplusplus
}
#endif
#endif
