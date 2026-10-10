/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/kernel_qualification_plan.c
 *
 * PURPOSE:
 *   Prepare a native Kernel qualification sequence independently of the
 *   platform process runner. A C23 test may prove this policy without QEMU,
 *   a compiler toolchain or any test disk being present.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "kernel_qualification_plan.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int UmicomKernelQualificationFailure(char *diagnostic, size_t capacity,
    const char *message)
{
    if (diagnostic && capacity) {
        (void)snprintf(diagnostic, capacity, "%s", message);
    }
    return 0;
}

static int UmicomKernelQualificationAbsolute(const char *path)
{
    if (!path || !*path) return 0;
    if (path[0] == '/') return 1;
    return isalpha((unsigned char)path[0]) && path[1] == ':' &&
        (path[2] == '\\' || path[2] == '/');
}

static int UmicomKernelQualificationPathValid(const char *path)
{
    if (!UmicomKernelQualificationAbsolute(path)) return 0;
    size_t bytes = strlen(path);
    if (bytes > UMICOM_KERNEL_QUALIFICATION_PATH - 80U) return 0;
    for (size_t index = 0U; index < bytes; ++index) {
        unsigned char c = (unsigned char)path[index];
        if (c < 32U || c == 127U) return 0;
    }
    return 1;
}

static int UmicomKernelQualificationPresetValid(const char *preset)
{
    if (!preset || !*preset || strlen(preset) >= UMICOM_KERNEL_QUALIFICATION_PRESET) return 0;
    for (const char *p = preset; *p; ++p) {
        const unsigned char c = (unsigned char)*p;
        if (!(isalnum(c) || c == '-' || c == '_')) return 0;
    }
    return 1;
}

static int UmicomKernelQualificationJobs(const char *input, unsigned *jobs)
{
    if (!input || !*input) return 0;
    unsigned number = 0U;
    for (const char *p = input; *p; ++p) {
        if (*p < '0' || *p > '9') return 0;
        number = number * 10U + (unsigned)(*p - '0');
        if (number > 32U) return 0;
    }
    if (number == 0U) return 0;
    *jobs = number;
    return 1;
}

static void UmicomKernelQualificationAppend(UmicomKernelQualificationPlan *plan,
    UmicomKernelQualificationStep step)
{
    plan->steps[plan->stepCount++] = step;
}

int UmicomKernelQualificationPlanParse(int argc, const char *const *argv,
    UmicomKernelQualificationPlan *out, char *diagnostic, size_t diagnosticCapacity)
{
    if (!out || argc < 0 || (argc != 0 && !argv))
        return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
            "Missing qualification plan arguments.");
    UmicomKernelQualificationPlan result = {0};
    result.jobs = 2U;
    result.scope = UMICOM_KERNEL_QUALIFY_BOTH;
    (void)strcpy(result.preset, "riscv64-clang-debug");
    unsigned seen = 0U;
    for (int index = 0; index < argc; ++index) {
        const char *key = argv[index];
        if (!key) return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
            "NULL qualification argument.");
        unsigned bit;
        if (strcmp(key, "--kernel-root") == 0) bit = 1U;
        else if (strcmp(key, "--preset") == 0) bit = 2U;
        else if (strcmp(key, "--scope") == 0) bit = 4U;
        else if (strcmp(key, "--jobs") == 0) bit = 8U;
        else if (strcmp(key, "--dry-run") == 0) bit = 16U;
        else if (strcmp(key, "--full") == 0) bit = 32U;
        else return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
            "Unknown qualification option. Use --help.");
        if (seen & bit) return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
            "A qualification option was supplied more than once.");
        seen |= bit;
        if (bit == 16U) { result.dryRun = 1; continue; }
        if (bit == 32U) { result.fullGuestSuite = 1; continue; }
        if (++index >= argc || !argv[index])
            return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
                "Missing value after qualification option.");
        const char *value = argv[index];
        if (bit == 1U) {
            if (!UmicomKernelQualificationPathValid(value))
                return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
                    "--kernel-root must be an absolute, bounded directory path.");
            (void)strcpy(result.kernelRoot, value);
        } else if (bit == 2U) {
            if (!UmicomKernelQualificationPresetValid(value))
                return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
                    "--preset must contain only letters, digits, hyphens or underscores.");
            (void)strcpy(result.preset, value);
        } else if (bit == 4U) {
            if (strcmp(value, "both") == 0) result.scope = UMICOM_KERNEL_QUALIFY_BOTH;
            else if (strcmp(value, "host") == 0) result.scope = UMICOM_KERNEL_QUALIFY_HOST;
            else if (strcmp(value, "guest") == 0) result.scope = UMICOM_KERNEL_QUALIFY_GUEST;
            else return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
                "--scope must be both, host or guest.");
        } else if (bit == 8U && !UmicomKernelQualificationJobs(value, &result.jobs)) {
            return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
                "--jobs must be a decimal integer between 1 and 32.");
        }
    }
    if (!(seen & 1U)) return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
        "--kernel-root is required; no repository is selected implicitly.");
    if (result.fullGuestSuite && result.scope == UMICOM_KERNEL_QUALIFY_HOST)
        return UmicomKernelQualificationFailure(diagnostic, diagnosticCapacity,
            "--full requires a guest qualification scope.");
    if (result.scope != UMICOM_KERNEL_QUALIFY_GUEST) {
        UmicomKernelQualificationAppend(&result, UMICOM_KERNEL_QUALIFY_HOST_CONFIGURE);
        UmicomKernelQualificationAppend(&result, UMICOM_KERNEL_QUALIFY_HOST_REGISTRATION);
        UmicomKernelQualificationAppend(&result, UMICOM_KERNEL_QUALIFY_HOST_BUILD);
        UmicomKernelQualificationAppend(&result, UMICOM_KERNEL_QUALIFY_HOST_TEST);
    }
    if (result.scope != UMICOM_KERNEL_QUALIFY_HOST) {
        UmicomKernelQualificationAppend(&result, UMICOM_KERNEL_QUALIFY_GUEST_CONFIGURE);
        UmicomKernelQualificationAppend(&result, UMICOM_KERNEL_QUALIFY_GUEST_REGISTRATION);
        UmicomKernelQualificationAppend(&result, UMICOM_KERNEL_QUALIFY_GUEST_BUILD);
        UmicomKernelQualificationAppend(&result, UMICOM_KERNEL_QUALIFY_GUEST_TEST);
    }
    *out = result;
    if (diagnostic && diagnosticCapacity) diagnostic[0] = '\0';
    return 1;
}

const char *UmicomKernelQualificationStepName(UmicomKernelQualificationStep step)
{
    switch (step) {
        case UMICOM_KERNEL_QUALIFY_HOST_CONFIGURE: return "host.configure";
        case UMICOM_KERNEL_QUALIFY_HOST_REGISTRATION: return "host.registration";
        case UMICOM_KERNEL_QUALIFY_HOST_BUILD: return "host.build";
        case UMICOM_KERNEL_QUALIFY_HOST_TEST: return "host.test";
        case UMICOM_KERNEL_QUALIFY_GUEST_CONFIGURE: return "guest.configure";
        case UMICOM_KERNEL_QUALIFY_GUEST_REGISTRATION: return "guest.registration";
        case UMICOM_KERNEL_QUALIFY_GUEST_BUILD: return "guest.build";
        case UMICOM_KERNEL_QUALIFY_GUEST_TEST: return "guest.test";
        default: return "unknown";
    }
}

/* CMake emits either add_test([=[name]=] ...) or add_test(name ...).
 * An exact opening argument is required. Test dependencies alone, commented
 * examples and similarly named tests cannot satisfy a registration gate. */
int UmicomKernelQualificationCTestHas(const char *definitions, const char *exactName)
{
    if (!definitions || !exactName || !*exactName) return 0;
    const size_t nameLength = strlen(exactName);
    const char *line = definitions;
    while (*line) {
        const char *end = strchr(line, '\n');
        if (!end) end = line + strlen(line);
        const char *at = line;
        while (at < end && (*at == ' ' || *at == '\t')) ++at;
        static const char prefix[] = "add_test(";
        if ((size_t)(end - at) > sizeof prefix - 1U &&
            memcmp(at, prefix, sizeof prefix - 1U) == 0) {
            at += sizeof prefix - 1U;
            while (at < end && (*at == ' ' || *at == '\t')) ++at;
            if (end - at >= 3 && memcmp(at, "[=[", 3U) == 0) {
                at += 3;
                if ((size_t)(end - at) >= nameLength + 3U &&
                    memcmp(at, exactName, nameLength) == 0 &&
                    memcmp(at + nameLength, "]=]", 3U) == 0) return 1;
            } else if (end - at >= 1 && *at == '"') {
                ++at;
                if ((size_t)(end - at) >= nameLength + 1U &&
                    memcmp(at, exactName, nameLength) == 0 &&
                    at[nameLength] == '"') return 1;
            } else if ((size_t)(end - at) > nameLength &&
                       memcmp(at, exactName, nameLength) == 0 &&
                       (at[nameLength] == ' ' || at[nameLength] == '\t' ||
                        at[nameLength] == ')')) return 1;
        }
        line = *end ? end + 1 : end;
    }
    return 0;
}

int UmicomKernelQualificationCTestRequired(const char *definitions, int guest,
    char *missing, size_t missingCapacity)
{
    static const char *const hostNames[] = {
        "kernel.host.fat16_current_binaries",
        "kernel.host.fat16_interruption_model",
        "kernel.host.fat16_guest_evidence_model"
    };
    static const char *const guestNames[] = {
        "kernel.build.current",
        "kernel.riscv64.fat16_commit",
        "kernel.riscv64.fat16_commit_interrupted",
        "kernel.riscv64.fat16_commit_readback",
        "kernel.riscv64.fat16_commit_rejected"
    };
    const char *const *names = guest ? guestNames : hostNames;
    const size_t count = guest ? sizeof guestNames / sizeof guestNames[0] :
        sizeof hostNames / sizeof hostNames[0];
    for (size_t index = 0U; index < count; ++index) {
        if (!UmicomKernelQualificationCTestHas(definitions, names[index])) {
            if (missing && missingCapacity) {
                (void)snprintf(missing, missingCapacity, "%s", names[index]);
            }
            return 0;
        }
    }
    if (missing && missingCapacity) missing[0] = '\0';
    return 1;
}

int UmicomKernelQualificationPlanRun(const UmicomKernelQualificationPlan *plan,
    UmicomKernelQualificationRunStep runStep, void *context,
    UmicomKernelQualificationStep *failedStep)
{
    if (failedStep) *failedStep = 0;
    if (!plan || !runStep || !plan->stepCount ||
        plan->stepCount > UMICOM_KERNEL_QUALIFICATION_MAX_STEPS) return 0;
    for (size_t index = 0U; index < plan->stepCount; ++index) {
        if (runStep(context, plan, plan->steps[index]) != 0) {
            if (failedStep) *failedStep = plan->steps[index];
            return 0;
        }
    }
    return 1;
}
