/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/boot_plan.c
 * PURPOSE: Build an owned argument vector without invoking a shell or touching files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "boot_internal.h"

/* Store each argument independently. Quoting belongs to the platform process
 * channel, while commas inside -drive belong to QEMU's own option grammar. */
static UmiStatus BootArgument(UmiVmBootPlan *plan, const char *text)
{
    if (plan->argumentCount == UMI_CHANNEL_MAX_ARGUMENTS ||
        strlen(text) >= VM_BOOT_ARGUMENT_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(plan->arguments[plan->argumentCount++], text);
    return UMI_STATUS_OK;
}

static UmiStatus BootDrive(UmiVmBootPlan *plan, const char *path, int optical)
{
    VmText drive;
    VmTextInit(&drive);
    VmTextPrint(&drive, optical
        ? "if=none,id=umicom_boot_cd,format=raw,media=cdrom,readonly=on,file="
        : "if=none,id=umicom_boot_disk,format=raw,snapshot=on,file=");
    /* Doubling commas prevents a file name from introducing a second QEMU
     * property. Spaces and quotes remain literal bytes in this argv element. */
    for (size_t index = 0; path[index]; ++index) {
        if (path[index] == ',') VmTextPrint(&drive, ",,");
        else VmTextPrint(&drive, "%c", path[index]);
    }
    UmiStatus status = drive.status;
    if (status == UMI_STATUS_OK) status = BootArgument(plan, "-drive");
    if (status == UMI_STATUS_OK) status = BootArgument(plan, drive.data);
    VmTextFree(&drive);
    return status;
}

static UmiStatus BootArguments(UmiVmBootPlan *plan)
{
    const UmiVmBootRequest *request = &plan->request;
    int pc = request->target == UMI_VM_BOOT_LINUX_X86_64 ||
        request->target == UMI_VM_BOOT_PC_ISO || request->target == UMI_VM_BOOT_PC_DISK;
    char memory[24], processors[24];
    (void)snprintf(memory, sizeof memory, "%u", request->memoryMiB);
    (void)snprintf(processors, sizeof processors, "%u", request->processors);
    UmiStatus status;
#define BOOT_ARG(text) do { status = BootArgument(plan, (text)); \
    if (status != UMI_STATUS_OK) return status; } while (0)
    /* Explicit devices avoid QEMU's default network card and monitor. Software
     * emulation also allows a guest ISA different from the host's ISA. */
    BOOT_ARG("-no-user-config");
    BOOT_ARG("-nodefaults");
    BOOT_ARG("-machine");
    BOOT_ARG(pc ? "q35" : request->target == UMI_VM_BOOT_UMICOM_KERNEL
        ? "virt,aclint=off" : "virt");
    BOOT_ARG("-accel"); BOOT_ARG("tcg");
    if (request->target == UMI_VM_BOOT_LINUX_AARCH64) {
        BOOT_ARG("-cpu"); BOOT_ARG("cortex-a57");
    }
    BOOT_ARG("-m"); BOOT_ARG(memory);
    BOOT_ARG("-smp"); BOOT_ARG(processors);
    BOOT_ARG("-S");
    BOOT_ARG("-no-reboot");
    BOOT_ARG("-display"); BOOT_ARG("none");
    BOOT_ARG("-monitor"); BOOT_ARG("none");
    BOOT_ARG("-nic"); BOOT_ARG("none");
    BOOT_ARG("-qmp"); BOOT_ARG("stdio");
    BOOT_ARG("-chardev"); BOOT_ARG("ringbuf,id=console,size=65536");
    BOOT_ARG("-serial"); BOOT_ARG("chardev:console");
    if (request->firmware[0]) {
        BOOT_ARG("-bios"); BOOT_ARG(request->firmware);
    } else if (request->target == UMI_VM_BOOT_LINUX_RISCV64) {
        BOOT_ARG("-bios"); BOOT_ARG("default");
    }
    if (request->kernel[0]) {
        BOOT_ARG("-kernel"); BOOT_ARG(request->kernel);
    }
    if (request->initrd[0]) {
        BOOT_ARG("-initrd"); BOOT_ARG(request->initrd);
    }
    if (request->commandLine[0]) {
        BOOT_ARG("-append"); BOOT_ARG(request->commandLine);
    }
    if (request->disk[0]) {
        status = BootDrive(plan, request->disk, 0);
        if (status != UMI_STATUS_OK) return status;
        BOOT_ARG("-device");
        BOOT_ARG(pc ? "virtio-blk-pci,drive=umicom_boot_disk"
                    : "virtio-blk-device,drive=umicom_boot_disk");
    }
    if (request->iso[0]) {
        status = BootDrive(plan, request->iso, 1);
        if (status != UMI_STATUS_OK) return status;
        /* q35 supplies its SATA controller, but nodefaults omits its CD-ROM.
         * Bind the optical device explicitly and select it as the boot source. */
        BOOT_ARG("-device");
        BOOT_ARG("ide-cd,bus=ide.0,drive=umicom_boot_cd,bootindex=1");
        BOOT_ARG("-boot"); BOOT_ARG("order=d");
    } else if (request->target == UMI_VM_BOOT_PC_DISK) {
        BOOT_ARG("-boot"); BOOT_ARG("order=c");
    }
#undef BOOT_ARG
    return UMI_STATUS_OK;
}

UmiStatus UmiVmBootPlanCreate(const UmiVmBootRequest *request, UmiVmBootPlan **outPlan)
{
    if (!outPlan) return UMI_STATUS_INVALID_ARGUMENT;
    *outPlan = NULL;
    UmiStatus status = UmiVmBootRequestValidate(request);
    if (status != UMI_STATUS_OK) return status;
    /* Large argv storage lives on the heap, never on a UI or kernel-sized stack.
     * Copy inputs before returning so editing a dialog cannot change its plan. */
    UmiVmBootPlan *plan = calloc(1, sizeof *plan);
    if (!plan) return UMI_STATUS_OUT_OF_MEMORY;
    plan->request = *request;
    status = BootArguments(plan);
    if (status != UMI_STATUS_OK) {
        free(plan);
        return status;
    }
    *outPlan = plan;
    return UMI_STATUS_OK;
}

void UmiVmBootPlanDestroy(UmiVmBootPlan *plan)
{
    free(plan);
}

const char *UmiVmBootPlanProgram(const UmiVmBootPlan *plan)
{
    return plan ? plan->request.executable : NULL;
}

const char *UmiVmBootPlanDirectory(const UmiVmBootPlan *plan)
{
    return plan ? plan->request.workingDirectory : NULL;
}

size_t UmiVmBootPlanArgumentCount(const UmiVmBootPlan *plan)
{
    return plan ? plan->argumentCount : 0U;
}

const char *UmiVmBootPlanArgument(const UmiVmBootPlan *plan, size_t index)
{
    return plan && index < plan->argumentCount ? plan->arguments[index] : NULL;
}
