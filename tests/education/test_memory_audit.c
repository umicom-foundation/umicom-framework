/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_memory_audit.c
 * PURPOSE: Verify page-rounded W^X findings, validation and atomic failure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/memory_audit.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(test) do { if (!(test)) { \
    (void)fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #test); \
    return 1; \
} } while (0)

static UmiEducationElfLoadPlan Example(void)
{
    UmiEducationElfLoadPlan plan = {0};
    plan.addressBits = 64U;
    plan.elfType = 2U;
    plan.machine = 243U;
    plan.segmentCount = 2U;
    plan.segments[0].virtualAddress = UINT64_C(0x80200000);
    plan.segments[0].fileBytes = UINT64_C(0x80);
    plan.segments[0].memoryBytes = UINT64_C(0x200);
    plan.segments[0].flags = UMI_EDUCATION_ELF_SEGMENT_READ | UMI_EDUCATION_ELF_SEGMENT_EXECUTE;
    plan.segments[1].virtualAddress = UINT64_C(0x80202000);
    plan.segments[1].fileBytes = UINT64_C(0x20);
    plan.segments[1].memoryBytes = UINT64_C(0x100);
    plan.segments[1].flags = UMI_EDUCATION_ELF_SEGMENT_READ | UMI_EDUCATION_ELF_SEGMENT_WRITE;
    return plan;
}

static int TestClean(void)
{
    UmiEducationElfLoadPlan plan = Example();
    UmiEducationMemoryAudit report = {0};
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_OK);
    CHECK(report.loadSegments == 2U && report.mappedPages == 2U);
    CHECK(report.readablePages == 2U && report.writablePages == 1U);
    CHECK(report.executablePages == 1U && report.writeExecutePages == 0U);
    CHECK(report.sharedPages == 0U && report.mixedPermissionPages == 0U);
    CHECK(report.machine == 243U && report.addressBits == 64U);
    return 0;
}

static int TestRoundedCollision(void)
{
    UmiEducationElfLoadPlan plan = Example();
    /* The two PT_LOAD ranges do not overlap as bytes, but become one page. */
    plan.segments[0].virtualAddress = UINT64_C(0x1000);
    plan.segments[0].memoryBytes = UINT64_C(0x600); /* [1000,1600) RX */
    plan.segments[1].virtualAddress = UINT64_C(0x1600);
    plan.segments[1].memoryBytes = UINT64_C(0x300); /* [1600,1900) RW */
    UmiEducationMemoryAudit report = {0};
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_OK);
    CHECK(report.mappedPages == 1U);
    CHECK(report.sharedPages == 1U && report.mixedPermissionPages == 1U);
    CHECK(report.writeExecutePages == 1U);
    CHECK(report.writableExecutableSegments == 0U);
    /* Aligned adjacent segments do not overlap after rounding. */
    plan.segments[0].virtualAddress = UINT64_C(0x1000);
    plan.segments[0].memoryBytes = UINT64_C(0x1000);
    plan.segments[1].virtualAddress = UINT64_C(0x2000);
    plan.segments[1].memoryBytes = UINT64_C(0x1000);
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_OK);
    CHECK(report.mappedPages == 2U && report.sharedPages == 0U);
    CHECK(report.writeExecutePages == 0U);
    return 0;
}

static int TestSuspiciousFlags(void)
{
    UmiEducationElfLoadPlan plan = Example();
    plan.segments[1].flags = 7U; /* One RWX segment: straightforward W^X case. */
    UmiEducationMemoryAudit report = {0};
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_OK);
    CHECK(report.writableExecutableSegments == 1U && report.writeExecutePages == 1U);
    plan.segments[1].flags |= UINT32_C(0x80000000); /* Unknown flags retained. */
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_OK);
    CHECK(report.unknownPermissionSegments == 1U);
    /* Zero-length PT_LOAD entries do not claim a page. */
    plan.segments[1].memoryBytes = 0U;
    plan.segments[1].fileBytes = 0U;
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_OK);
    CHECK(report.mappedPages == 1U && report.writeExecutePages == 0U);
    /* Even a huge declared range must not cause a proportional allocation. */
    plan.segmentCount = 1U;
    plan.segments[0].virtualAddress = 0U;
    plan.segments[0].fileBytes = 1U;
    plan.segments[0].memoryBytes = UINT64_MAX;
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_OK);
    CHECK(report.mappedPages == (UINT64_MAX / 4096U) + 1U);
    return 0;
}

static int TestFailures(void)
{
    UmiEducationElfLoadPlan plan = Example();
    UmiEducationMemoryAudit report;
    memset(&report, 0xa5, sizeof report);
    const UmiEducationMemoryAudit previous = report;
    CHECK(UmiEducationMemoryAuditPlan(NULL, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationMemoryAuditPlan(&plan, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    plan.segmentCount = 65U;
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    plan = Example();
    plan.segments[1].fileBytes = plan.segments[1].memoryBytes + 1U;
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_PARSE_ERROR);
    plan = Example();
    plan.addressBits = 32U;
    plan.segments[1].virtualAddress = UINT64_C(0x100000000);
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_PARSE_ERROR);
    plan = Example();
    plan.segments[0].virtualAddress = UINT64_MAX - 1U;
    CHECK(UmiEducationMemoryAuditPlan(&plan, &report) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiEducationMemoryAuditFile("no-such-umicom-image-723412", &report) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&report, &previous, sizeof report) == 0);
    return 0;
}

int main(void)
{
    CHECK(TestClean() == 0);
    CHECK(TestRoundedCollision() == 0);
    CHECK(TestSuspiciousFlags() == 0);
    CHECK(TestFailures() == 0);
    puts("[OK] ELF page-rounded permissions, shared pages, W^X, massive ranges, atomic failure.");
    return 0;
}
