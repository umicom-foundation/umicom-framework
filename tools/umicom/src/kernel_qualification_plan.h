/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/kernel_qualification_plan.h
 *
 * PURPOSE:
 *   Describe a bounded, portable qualification journey for the independent
 *   Umicom Kernel. Framework orchestrates an existing build/test contract;
 *   neither the Kernel nor Umicom OS depends on this developer tool.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOL_KERNEL_QUALIFICATION_PLAN_H
#define UMICOM_TOOL_KERNEL_QUALIFICATION_PLAN_H

#include <stddef.h>

#define UMICOM_KERNEL_QUALIFICATION_PATH 1024U
#define UMICOM_KERNEL_QUALIFICATION_PRESET 80U
#define UMICOM_KERNEL_QUALIFICATION_MAX_STEPS 8U

typedef enum UmicomKernelQualificationScope {
    UMICOM_KERNEL_QUALIFY_BOTH = 1,
    UMICOM_KERNEL_QUALIFY_HOST,
    UMICOM_KERNEL_QUALIFY_GUEST
} UmicomKernelQualificationScope;

typedef enum UmicomKernelQualificationStep {
    UMICOM_KERNEL_QUALIFY_HOST_CONFIGURE = 1,
    UMICOM_KERNEL_QUALIFY_HOST_BUILD,
    UMICOM_KERNEL_QUALIFY_HOST_REGISTRATION,
    UMICOM_KERNEL_QUALIFY_HOST_TEST,
    UMICOM_KERNEL_QUALIFY_GUEST_CONFIGURE,
    UMICOM_KERNEL_QUALIFY_GUEST_REGISTRATION,
    UMICOM_KERNEL_QUALIFY_GUEST_BUILD,
    UMICOM_KERNEL_QUALIFY_GUEST_TEST
} UmicomKernelQualificationStep;

typedef struct UmicomKernelQualificationPlan {
    char kernelRoot[UMICOM_KERNEL_QUALIFICATION_PATH];
    char preset[UMICOM_KERNEL_QUALIFICATION_PRESET];
    unsigned jobs;
    UmicomKernelQualificationScope scope;
    int dryRun;
    int fullGuestSuite;
    UmicomKernelQualificationStep steps[UMICOM_KERNEL_QUALIFICATION_MAX_STEPS];
    size_t stepCount;
} UmicomKernelQualificationPlan;

/* No files, processes or repositories are touched while parsing. Missing or
 * duplicate options fail rather than silently using another repository. */
int UmicomKernelQualificationPlanParse(int argc, const char *const *argv,
    UmicomKernelQualificationPlan *out, char *diagnostic, size_t diagnosticCapacity);
const char *UmicomKernelQualificationStepName(UmicomKernelQualificationStep step);

/* Find an actual add_test() definition in a CMake-generated CTestTestfile,
 * not a mention inside a comment, another test name or unrelated prose. */
int UmicomKernelQualificationCTestHas(const char *definitions, const char *exactName);

/* Report missing mandatory test registrations explicitly. This prevents a
 * successful CTest exit after QEMU was absent during CMake configuration. */
int UmicomKernelQualificationCTestRequired(const char *definitions, int guest,
    char *missing, size_t missingCapacity);

/* Execute the plan through a caller-owned adapter. A failed step stops all
 * following work; no skipped or stale executable counts as successful. */
typedef int (*UmicomKernelQualificationRunStep)(
    void *context, const UmicomKernelQualificationPlan *plan,
    UmicomKernelQualificationStep step);
int UmicomKernelQualificationPlanRun(const UmicomKernelQualificationPlan *plan,
    UmicomKernelQualificationRunStep runStep, void *context,
    UmicomKernelQualificationStep *failedStep);

#endif /* UMICOM_TOOL_KERNEL_QUALIFICATION_PLAN_H */
