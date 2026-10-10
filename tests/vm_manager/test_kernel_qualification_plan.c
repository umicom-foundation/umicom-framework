/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_kernel_qualification_plan.c
 *
 * PURPOSE:
 *   Verify native Kernel qualification option boundaries, CTest registration
 *   evidence and fail-fast ordering independently of a QEMU installation.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "kernel_qualification_plan.h"

#include <stdio.h>
#include <string.h>

static unsigned checks;
static unsigned failures;
#define CHECK(expression) do { \
    ++checks; \
    if (!(expression)) { \
        ++failures; \
        (void)fprintf(stderr, "CHECK failed at line %d: %s\n", __LINE__, #expression); \
    } \
} while (0)

static int Parse(int argc, const char *const *argv, UmicomKernelQualificationPlan *plan)
{
    char error[256];
    return UmicomKernelQualificationPlanParse(argc, argv, plan, error, sizeof error);
}

static void TestOptions(void)
{
    UmicomKernelQualificationPlan plan;
    static const char *const valid[] = {"--kernel-root", "/projects/umicom-kernel"};
    CHECK(Parse(2, valid, &plan));
    CHECK(strcmp(plan.kernelRoot, "/projects/umicom-kernel") == 0);
    CHECK(strcmp(plan.preset, "riscv64-clang-debug") == 0);
    CHECK(plan.scope == UMICOM_KERNEL_QUALIFY_BOTH);
    CHECK(plan.jobs == 2U);
    CHECK(plan.stepCount == 8U);
    CHECK(plan.steps[0] == UMICOM_KERNEL_QUALIFY_HOST_CONFIGURE);
    CHECK(plan.steps[7] == UMICOM_KERNEL_QUALIFY_GUEST_TEST);
    static const char *const win[] = {
        "--kernel-root", "C:\\umicom\\umicom-kernel",
        "--scope", "guest", "--preset", "riscv64-clang-debug", "--jobs", "16",
        "--full", "--dry-run"
    };
    CHECK(Parse(10, win, &plan));
    CHECK(plan.scope == UMICOM_KERNEL_QUALIFY_GUEST);
    CHECK(plan.stepCount == 4U);
    CHECK(plan.fullGuestSuite == 1 && plan.dryRun == 1 && plan.jobs == 16U);
    CHECK(plan.steps[0] == UMICOM_KERNEL_QUALIFY_GUEST_CONFIGURE);
    static const char *const host[] = {"--kernel-root", "/project", "--scope", "host"};
    CHECK(Parse(4, host, &plan));
    CHECK(plan.stepCount == 4U && plan.steps[3] == UMICOM_KERNEL_QUALIFY_HOST_TEST);
    char diagnostic[256];
    CHECK(!UmicomKernelQualificationPlanParse(0, NULL, &plan, diagnostic, sizeof diagnostic));
    CHECK(strstr(diagnostic, "required") != NULL);
    static const char *const duplicate[] = {"--kernel-root", "/project", "--jobs", "2", "--jobs", "3"};
    CHECK(!Parse(6, duplicate, &plan));
    static const char *const relative[] = {"--kernel-root", "../umicom-kernel"};
    CHECK(!Parse(2, relative, &plan));
    static const char *const odd[] = {"--kernel-root", "/project", "--preset"};
    CHECK(!Parse(3, odd, &plan));
    static const char *const injection[] = {"--kernel-root", "/project", "--preset", "foo;rm -rf"};
    CHECK(!Parse(4, injection, &plan));
    static const char *const jobs[] = {"--kernel-root", "/project", "--jobs", "99999999999"};
    CHECK(!Parse(4, jobs, &plan));
    static const char *const zero[] = {"--kernel-root", "/project", "--jobs", "0"};
    CHECK(!Parse(4, zero, &plan));
    static const char *const overlap[] = {"--kernel-root", "/project", "--scope", "host", "--full"};
    CHECK(!Parse(5, overlap, &plan));
    static const char *const unknown[] = {"--kernel-root", "/project", "--do-writes"};
    CHECK(!Parse(3, unknown, &plan));
    CHECK(!Parse(-1, valid, &plan));
    CHECK(!Parse(2, NULL, &plan));
    CHECK(!Parse(2, valid, NULL));
    char veryLong[1200];
    (void)memset(veryLong, 'a', sizeof veryLong);
    veryLong[0] = '/';
    veryLong[sizeof veryLong - 1U] = '\0';
    static const char *oversize[2] = {"--kernel-root", NULL};
    oversize[1] = veryLong;
    CHECK(!Parse(2, oversize, &plan));
    char control[] = "/project\n--foo";
    const char *const malformed[] = {"--kernel-root", control};
    CHECK(!Parse(2, malformed, &plan));
}

static void TestRegistration(void)
{
    static const char *const guest =
        "# add_test([=[kernel.riscv64.fake]=] fake)\n"
        "add_test([=[kernel.build.current]=] \"cmake\")\n"
        "add_test([=[kernel.riscv64.fat16_commit]=] \"cmake\")\n"
        "add_test([=[kernel.riscv64.fat16_commit_interrupted]=] \"cmake\")\n"
        "add_test([=[kernel.riscv64.fat16_commit_readback]=] \"cmake\")\n"
        "add_test([=[kernel.riscv64.fat16_commit_rejected]=] \"cmake\")\n";
    CHECK(UmicomKernelQualificationCTestHas(guest, "kernel.build.current"));
    CHECK(!UmicomKernelQualificationCTestHas(guest, "kernel.riscv64.fake"));
    CHECK(!UmicomKernelQualificationCTestHas(guest, "kernel.riscv64.fat16_commit_extra"));
    char missing[128];
    CHECK(UmicomKernelQualificationCTestRequired(guest, 1, missing, sizeof missing));
    CHECK(missing[0] == '\0');
    CHECK(!UmicomKernelQualificationCTestRequired("add_test([=[kernel.build.current]=] x)\n",
        1, missing, sizeof missing));
    CHECK(strcmp(missing, "kernel.riscv64.fat16_commit") == 0);
    static const char *const host =
        "add_test(kernel.host.fat16_current_binaries \"cmake\")\n"
        "add_test(\"kernel.host.fat16_interruption_model\" tool)\n"
        "  add_test([=[kernel.host.fat16_guest_evidence_model]=] tool)\n";
    CHECK(UmicomKernelQualificationCTestRequired(host, 0, missing, sizeof missing));
    CHECK(!UmicomKernelQualificationCTestHas("# add_test(kernel.host.fat16_current_binaries xxx)\n",
        "kernel.host.fat16_current_binaries"));
    CHECK(!UmicomKernelQualificationCTestHas("add_test(kernel.host.fat16_current_binaries_extra x)\n",
        "kernel.host.fat16_current_binaries"));
    CHECK(!UmicomKernelQualificationCTestHas(NULL, "foo"));
    CHECK(!UmicomKernelQualificationCTestHas("add_test(foo x)\n", NULL));
    CHECK(!UmicomKernelQualificationCTestHas("add_test(foo x)\n", ""));
    CHECK(!UmicomKernelQualificationCTestRequired("", 0, missing, sizeof missing));
    CHECK(strcmp(missing, "kernel.host.fat16_current_binaries") == 0);
}

typedef struct Recorder {
    UmicomKernelQualificationStep sequence[8];
    size_t count;
    UmicomKernelQualificationStep failAt;
} Recorder;
static int RecordStep(void *context, const UmicomKernelQualificationPlan *plan,
    UmicomKernelQualificationStep step)
{
    Recorder *record = context;
    CHECK(plan != NULL);
    CHECK(record->count < sizeof record->sequence / sizeof record->sequence[0]);
    if (record->count < sizeof record->sequence / sizeof record->sequence[0])
        record->sequence[record->count++] = step;
    return step == record->failAt ? 1 : 0;
}

static void TestExecution(void)
{
    UmicomKernelQualificationPlan plan;
    static const char *const valid[] = {"--kernel-root", "/project"};
    CHECK(Parse(2, valid, &plan));
    Recorder record = {0};
    UmicomKernelQualificationStep failed = 99;
    CHECK(UmicomKernelQualificationPlanRun(&plan, RecordStep, &record, &failed));
    CHECK(record.count == 8U && failed == 0);
    for (size_t index = 0U; index < plan.stepCount; ++index)
        CHECK(record.sequence[index] == plan.steps[index]);
    record.count = 0;
    record.failAt = UMICOM_KERNEL_QUALIFY_HOST_BUILD;
    CHECK(!UmicomKernelQualificationPlanRun(&plan, RecordStep, &record, &failed));
    CHECK(failed == UMICOM_KERNEL_QUALIFY_HOST_BUILD);
    CHECK(record.count == 3U);
    record.count = 0;
    record.failAt = UMICOM_KERNEL_QUALIFY_GUEST_REGISTRATION;
    CHECK(!UmicomKernelQualificationPlanRun(&plan, RecordStep, &record, &failed));
    CHECK(record.count == 6U && failed == UMICOM_KERNEL_QUALIFY_GUEST_REGISTRATION);
    CHECK(!UmicomKernelQualificationPlanRun(NULL, RecordStep, &record, &failed));
    CHECK(!UmicomKernelQualificationPlanRun(&plan, NULL, &record, &failed));
    CHECK(strcmp(UmicomKernelQualificationStepName(UMICOM_KERNEL_QUALIFY_HOST_TEST), "host.test") == 0);
    CHECK(strcmp(UmicomKernelQualificationStepName(99), "unknown") == 0);
}

int main(void)
{
    TestOptions();
    TestRegistration();
    TestExecution();
    if (failures) {
        (void)fprintf(stderr, "Kernel qualification plan: %u/%u checks failed.\n", failures, checks);
        return 1;
    }
    (void)printf("Kernel qualification plan: %u/%u checks passed.\n", checks, checks);
    return 0;
}
