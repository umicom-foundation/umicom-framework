/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_kernel_qualification.c
 *
 * PURPOSE:
 *   Qualify the independent native Kernel from the Framework-owned `umicom`
 *   developer command. Reuse the existing toolchain, CMake/CTest fixture and
 *   QEMU guest journeys instead of implementing another process runner or
 *   a PowerShell automation script.
 *
 * ARCHITECTURE:
 *   The CLI is a consumer, not a Kernel dependency. The kernel source and
 *   Umicom OS distribution remain independently buildable. A successful
 *   compile never stands in for a skipped or unregistered guest test.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"
#include "kernel_qualification_plan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void UmicomKernelQualificationHelp(void)
{
    (void)puts(
        "Usage: umicom qemu kernel-qualify --kernel-root ABSOLUTE_PATH\n"
        "          [--scope both|host|guest] [--preset NAME] [--jobs 1..32]\n"
        "          [--full] [--dry-run]\n\n"
        "Default: run the complete FAT16 host audit and existing RV64 QEMU\n"
        "FAT16 Stage/Finish, readback, interrupted/dirty rejection journeys.\n"
        "--full also runs all registered Kernel guest tests.\n"
        "The Kernel owns its own tests and disposable media. The Framework\n"
        "reuses the normal native configure/build/test services; no script,\n"
        "host disk, OS distribution or application is modified by this CLI.\n"
        "--dry-run prints the plan without creating build files or running QEMU.\n"
        "A missing QEMU registration is an error, not a skipped success."
    );
}

typedef struct UmicomKernelQualificationContext {
    UmiCliContext cli;
    char hostSource[UMI_PATH_CAPACITY];
    char hostBuild[UMI_PATH_CAPACITY];
    char guestBuild[UMI_PATH_CAPACITY];
    char hostCTest[UMI_PATH_CAPACITY];
    char guestCTest[UMI_PATH_CAPACITY];
} UmicomKernelQualificationContext;

static int UmicomKernelQualificationJoined(char *out, size_t capacity,
    const char *root, const char *name)
{
    if (umi_fs_join(out, capacity, root, name) != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Kernel qualification path is too long: %s\n", name);
        return 0;
    }
    return 1;
}

static int UmicomKernelQualificationPreflight(const UmicomKernelQualificationPlan *plan,
    UmicomKernelQualificationContext *ctx)
{
    char path[UMI_PATH_CAPACITY];
    char guestPresetPath[UMICOM_KERNEL_QUALIFICATION_PRESET + 16U];
    const char *const required[] = {
        "CMakeLists.txt", "CMakePresets.json",
        "cmake/Fat16Commit.cmake", "tools/fat16-recovery-audit/CMakeLists.txt"
    };
    for (size_t index = 0U; index < sizeof required / sizeof required[0]; ++index) {
        if (!UmicomKernelQualificationJoined(path, sizeof path, plan->kernelRoot, required[index]))
            return 0;
        if (!umi_fs_is_file(path)) {
            (void)fprintf(stderr, "Kernel qualification input not found: %s\n", path);
            return 0;
        }
    }
    if (!UmicomKernelQualificationJoined(ctx->hostSource, sizeof ctx->hostSource,
            plan->kernelRoot, "tools/fat16-recovery-audit") ||
        !UmicomKernelQualificationJoined(ctx->hostBuild, sizeof ctx->hostBuild,
            plan->kernelRoot, "build/fat16-recovery-audit") ||
        !UmicomKernelQualificationJoined(ctx->hostCTest, sizeof ctx->hostCTest,
            ctx->hostBuild, "CTestTestfile.cmake")) return 0;
    (void)snprintf(guestPresetPath, sizeof guestPresetPath, "build/%s", plan->preset);
    if (!UmicomKernelQualificationJoined(ctx->guestBuild, sizeof ctx->guestBuild,
            plan->kernelRoot, guestPresetPath) ||
        !UmicomKernelQualificationJoined(ctx->guestCTest, sizeof ctx->guestCTest,
            ctx->guestBuild, "CTestTestfile.cmake")) return 0;
    return 1;
}

static int UmicomKernelQualificationRegistered(const char *ctestPath, int guest)
{
    char *definitions = NULL;
    if (umi_fs_read_text(ctestPath, &definitions, NULL) != UMI_STATUS_OK || !definitions) {
        (void)fprintf(stderr, "CTest registration unavailable: %s\n", ctestPath);
        return 1;
    }
    char missing[128];
    const int valid = UmicomKernelQualificationCTestRequired(
        definitions, guest, missing, sizeof missing);
    umi_fs_free_text(definitions);
    if (!valid) {
        (void)fprintf(stderr,
            "Required %s CTest was not registered: %s\n"
            "Check QEMU discovery and reconfigure; no unexecuted tests are credited.\n",
            guest ? "guest" : "host", missing);
        return 1;
    }
    return 0;
}

static int UmicomKernelQualificationBuild(UmicomKernelQualificationContext *ctx,
    const UmicomKernelQualificationPlan *plan, UmiBuildAction action, int guest)
{
    char jobText[16];
    (void)snprintf(jobText, sizeof jobText, "%u", plan->jobs);
    /* The shared build entry point does not mutate its argv. Literal flags and
     * the plan's stable strings are therefore borrowed only for this call. */
    char *hostArguments[] = {
        "--source", ctx->hostSource,
        "--build", ctx->hostBuild,
        "--jobs", jobText
    };
    char *guestArguments[] = {
        "--source", (char *)plan->kernelRoot,
        "--preset", (char *)plan->preset,
        "--jobs", jobText,
        "--tests", "^kernel[.]riscv64[.]fat16_commit"
    };
    const int guestCount = action == UMI_BUILD_TEST && plan->fullGuestSuite ? 6 : 8;
    /* Do not select an absent old ELF. CTest's current-image and disk-copy
     * fixtures are included by its normal test dependency graph. */
    return umi_cli_command_build(&ctx->cli, action,
        guest ? guestCount : 6, guest ? guestArguments : hostArguments);
}

static int UmicomKernelQualificationRunOne(void *context,
    const UmicomKernelQualificationPlan *plan,
    UmicomKernelQualificationStep step)
{
    UmicomKernelQualificationContext *ctx = context;
    (void)printf("[kernel] %s\n", UmicomKernelQualificationStepName(step));
    if (step == UMICOM_KERNEL_QUALIFY_HOST_REGISTRATION)
        return UmicomKernelQualificationRegistered(ctx->hostCTest, 0);
    if (step == UMICOM_KERNEL_QUALIFY_GUEST_REGISTRATION)
        return UmicomKernelQualificationRegistered(ctx->guestCTest, 1);
    if (step == UMICOM_KERNEL_QUALIFY_HOST_CONFIGURE ||
        step == UMICOM_KERNEL_QUALIFY_GUEST_CONFIGURE)
        return UmicomKernelQualificationBuild(ctx, plan, UMI_BUILD_CONFIGURE,
            step == UMICOM_KERNEL_QUALIFY_GUEST_CONFIGURE);
    if (step == UMICOM_KERNEL_QUALIFY_HOST_BUILD ||
        step == UMICOM_KERNEL_QUALIFY_GUEST_BUILD)
        return UmicomKernelQualificationBuild(ctx, plan, UMI_BUILD_COMPILE,
            step == UMICOM_KERNEL_QUALIFY_GUEST_BUILD);
    if (step == UMICOM_KERNEL_QUALIFY_HOST_TEST ||
        step == UMICOM_KERNEL_QUALIFY_GUEST_TEST)
        return UmicomKernelQualificationBuild(ctx, plan, UMI_BUILD_TEST,
            step == UMICOM_KERNEL_QUALIFY_GUEST_TEST);
    (void)fprintf(stderr, "Invalid Kernel qualification phase.\n");
    return 1;
}

int umi_cli_command_kernel_qualification(int argc, char **argv)
{
    if (argc == 1 && (!strcmp(argv[0], "help") || !strcmp(argv[0], "--help"))) {
        UmicomKernelQualificationHelp();
        return 0;
    }
    UmicomKernelQualificationPlan plan;
    char error[256];
    if (!UmicomKernelQualificationPlanParse(argc, (const char *const *)argv,
            &plan, error, sizeof error)) {
        (void)fprintf(stderr, "Kernel qualification: %s\n", error);
        UmicomKernelQualificationHelp();
        return 2;
    }
    UmicomKernelQualificationContext *ctx = calloc(1U, sizeof *ctx);
    if (!ctx) {
        (void)fputs("Kernel qualification could not allocate context.\n", stderr);
        return 1;
    }
    if (!UmicomKernelQualificationPreflight(&plan, ctx)) {
        free(ctx);
        return 1;
    }
    (void)printf("Kernel source: %s\nHost audit: %s\nGuest preset: %s\n",
        plan.kernelRoot, ctx->hostSource, plan.preset);
    if (plan.dryRun) {
        for (size_t index = 0U; index < plan.stepCount; ++index)
            (void)printf("[plan] %s\n", UmicomKernelQualificationStepName(plan.steps[index]));
        (void)puts("Dry run: no build, test or emulator process started.");
        free(ctx);
        return 0;
    }
    UmicomKernelQualificationStep failed = 0;
    const int passed = UmicomKernelQualificationPlanRun(&plan,
        UmicomKernelQualificationRunOne, ctx, &failed);
    free(ctx);
    if (!passed) {
        (void)fprintf(stderr, "Kernel qualification FAILED at %s.\n",
            UmicomKernelQualificationStepName(failed));
        return 1;
    }
    if (plan.scope == UMICOM_KERNEL_QUALIFY_HOST)
        (void)puts("Kernel HOST qualification PASSED: registered FAT16 audit tests executed.");
    else if (plan.scope == UMICOM_KERNEL_QUALIFY_GUEST)
        (void)puts("Kernel GUEST qualification PASSED: registered QEMU FAT16 tests executed.");
    else
        (void)puts("Kernel qualification PASSED: host audit and QEMU FAT16 tests executed.");
    (void)puts("This qualifies the selected tests, not physical storage power-loss safety.");
    return 0;
}
