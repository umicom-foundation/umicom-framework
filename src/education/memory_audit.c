/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/memory_audit.c
 *
 * PURPOSE:
 *   Evaluate executable-file page permissions without becoming a native ELF
 *   loader. The canonical ELF load-plan reader remains authoritative for
 *   headers, bounds, endianness and PT_LOAD validation.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/memory_audit.h"
#include <stdint.h>
#include <stdlib.h>

/* A page number, not an address, lets the exclusive end be represented even
 * near the highest possible unsigned address without rounding into overflow. */
typedef struct EducationPageSpan {
    uint64_t first;
    uint64_t afterLast;
    uint32_t flags;
} EducationPageSpan;

static int EducationComparePages(const void *left, const void *right)
{
    const uint64_t a = *(const uint64_t *)left;
    const uint64_t b = *(const uint64_t *)right;
    return (a > b) - (a < b);
}

static int EducationFitsVirtualAddress(uint64_t start, uint64_t length,
                                       uint8_t addressBits)
{
    if (start > UINT64_MAX - length) return 0;
    if (addressBits == 32U)
        return start + length <= (UINT64_C(1) << 32U);
    return addressBits == 64U;
}

/* Each interval between two consecutive page boundaries has a constant set of
 * owning PT_LOAD segments. We never create or visit individual pages; that
 * matters when a hostile ELF declares an enormous virtual-memory range. */
UmiStatus UmiEducationMemoryAuditPlan(const UmiEducationElfLoadPlan *plan,
                                      UmiEducationMemoryAudit *outReport)
{
    if (plan == NULL || outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if ((plan->addressBits != 32U && plan->addressBits != 64U) ||
        plan->segmentCount == 0U ||
        plan->segmentCount > UMI_EDUCATION_ELF_LOAD_MAX_SEGMENTS)
        return UMI_STATUS_INVALID_ARGUMENT;

    EducationPageSpan spans[UMI_EDUCATION_ELF_LOAD_MAX_SEGMENTS] = {{0}};
    uint64_t boundaries[2U * UMI_EDUCATION_ELF_LOAD_MAX_SEGMENTS] = {0};
    size_t spanCount = 0U;
    size_t boundaryCount = 0U;
    UmiEducationMemoryAudit report = {0};
    report.elfType = plan->elfType;
    report.machine = plan->machine;
    report.addressBits = plan->addressBits;
    report.loadSegments = plan->segmentCount;

    for (uint32_t i = 0U; i < plan->segmentCount; ++i) {
        const UmiEducationElfLoadSegment *part = &plan->segments[i];
        if (part->fileBytes > part->memoryBytes ||
            !EducationFitsVirtualAddress(part->virtualAddress, part->memoryBytes,
                                         plan->addressBits))
            return UMI_STATUS_PARSE_ERROR;
        if ((part->flags & ~UINT32_C(7)) != 0U)
            ++report.unknownPermissionSegments;
        if (part->memoryBytes == 0U) continue;
        if ((part->flags & (UMI_EDUCATION_ELF_SEGMENT_EXECUTE |
                            UMI_EDUCATION_ELF_SEGMENT_WRITE)) ==
                           (UMI_EDUCATION_ELF_SEGMENT_EXECUTE |
                            UMI_EDUCATION_ELF_SEGMENT_WRITE))
            ++report.writableExecutableSegments;
        const uint64_t end = part->virtualAddress + part->memoryBytes;
        const uint64_t first = part->virtualAddress / UMI_EDUCATION_MEMORY_PAGE_BYTES;
        const uint64_t last = end / UMI_EDUCATION_MEMORY_PAGE_BYTES +
                             (end % UMI_EDUCATION_MEMORY_PAGE_BYTES != 0U ? 1U : 0U);
        if (last <= first) return UMI_STATUS_PARSE_ERROR;
        spans[spanCount++] = (EducationPageSpan){first, last, part->flags};
        boundaries[boundaryCount++] = first;
        boundaries[boundaryCount++] = last;
    }
    if (spanCount == 0U) return UMI_STATUS_PARSE_ERROR;
    qsort(boundaries, boundaryCount, sizeof(boundaries[0]), EducationComparePages);

    for (size_t i = 1U; i < boundaryCount; ++i) {
        const uint64_t first = boundaries[i - 1U];
        const uint64_t after = boundaries[i];
        if (after == first) continue;
        const uint64_t pageCount = after - first;
        uint32_t combined = 0U;
        uint32_t firstFlags = 0U;
        unsigned owners = 0U;
        int mixed = 0;
        for (size_t j = 0U; j < spanCount; ++j) {
            if (first < spans[j].first || first >= spans[j].afterLast) continue;
            if (owners != 0U && firstFlags != spans[j].flags) mixed = 1;
            if (owners == 0U) firstFlags = spans[j].flags;
            combined |= spans[j].flags;
            ++owners;
        }
        if (owners == 0U) continue;
        /* A 64-bit address spans at most 2^52 pages. All reported page sets
         * are subsets of that finite address space and cannot overflow. */
        report.mappedPages += pageCount;
        if (combined & UMI_EDUCATION_ELF_SEGMENT_READ)
            report.readablePages += pageCount;
        if (combined & UMI_EDUCATION_ELF_SEGMENT_WRITE)
            report.writablePages += pageCount;
        if (combined & UMI_EDUCATION_ELF_SEGMENT_EXECUTE)
            report.executablePages += pageCount;
        if ((combined & (UMI_EDUCATION_ELF_SEGMENT_WRITE |
                         UMI_EDUCATION_ELF_SEGMENT_EXECUTE)) ==
                        (UMI_EDUCATION_ELF_SEGMENT_WRITE |
                         UMI_EDUCATION_ELF_SEGMENT_EXECUTE))
            report.writeExecutePages += pageCount;
        if (owners > 1U) report.sharedPages += pageCount;
        if (mixed) report.mixedPermissionPages += pageCount;
    }
    *outReport = report;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationMemoryAuditFile(const char *file,
                                      UmiEducationMemoryAudit *outReport)
{
    if (file == NULL || outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiEducationElfLoadPlan plan = {0};
    const UmiStatus status = UmiEducationElfLoadPlanInspectFile(file, &plan);
    if (status != UMI_STATUS_OK) return status;
    return UmiEducationMemoryAuditPlan(&plan, outReport);
}
