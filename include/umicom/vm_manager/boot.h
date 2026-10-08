/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vm_manager/boot.h
 * PURPOSE: Plan and supervise standalone operating-system boots through QEMU.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VM_MANAGER_BOOT_H
#define UMICOM_VM_MANAGER_BOOT_H
#include "umicom/vm_manager/manager.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Targets describe a guest boot protocol, not the architecture of the host.
 * Add a target to the shared catalogue and its validation rules together; a GUI
 * can enumerate this catalogue without maintaining a second machine table. */
typedef enum UmiVmBootTarget {
    UMI_VM_BOOT_UMICOM_KERNEL = 1,
    UMI_VM_BOOT_LINUX_X86_64,
    UMI_VM_BOOT_LINUX_RISCV64,
    UMI_VM_BOOT_LINUX_AARCH64,
    UMI_VM_BOOT_PC_ISO,
    UMI_VM_BOOT_PC_DISK
} UmiVmBootTarget;

typedef struct UmiVmBootTargetInfo {
    UmiVmBootTarget target;
    const char *name;
    const char *architecture;
    const char *emulator;
    const char *description;
} UmiVmBootTargetInfo;

#define UMI_VM_BOOT_COMMAND_LINE 2048U
typedef struct UmiVmBootRequest {
    UmiVmBootTarget target;
    char executable[UMI_VM_PATH];
    char workingDirectory[UMI_VM_PATH];
    char firmware[UMI_VM_PATH];
    char kernel[UMI_VM_PATH];
    char initrd[UMI_VM_PATH];
    char disk[UMI_VM_PATH];
    char iso[UMI_VM_PATH];
    char commandLine[UMI_VM_BOOT_COMMAND_LINE];
    unsigned memoryMiB;
    unsigned processors;
} UmiVmBootRequest;

typedef struct UmiVmBootPlan UmiVmBootPlan;

/* Catalogue strings are immutable and remain owned by Framework. */
size_t UmiVmBootTargetCount(void);
const UmiVmBootTargetInfo *UmiVmBootTargetAt(size_t index);
const UmiVmBootTargetInfo *UmiVmBootTargetFind(const char *name);
/* Initialisation supplies resource and serial-console defaults only. All file
 * paths must be explicitly supplied as absolute paths. No file is opened. */
UmiStatus UmiVmBootRequestInit(UmiVmBootRequest *request, UmiVmBootTarget target);
UmiStatus UmiVmBootRequestValidate(const UmiVmBootRequest *request);

/* Plans own all copied strings and may be read concurrently while kept alive.
 * Creation is a pure operation: it does not probe a file or start a process.
 * Disk inputs are RAW regular files, attached through temporary QEMU overlays.
 * No writable base image, host device, network, share or monitor is exposed.
 * The plan is not an emulator sandbox or proof that a guest will boot. */
UmiStatus UmiVmBootPlanCreate(const UmiVmBootRequest *request, UmiVmBootPlan **outPlan);
void UmiVmBootPlanDestroy(UmiVmBootPlan *plan);
const char *UmiVmBootPlanProgram(const UmiVmBootPlan *plan);
const char *UmiVmBootPlanDirectory(const UmiVmBootPlan *plan);
size_t UmiVmBootPlanArgumentCount(const UmiVmBootPlan *plan);
const char *UmiVmBootPlanArgument(const UmiVmBootPlan *plan, size_t index);

/* Review reads the native executable and each selected input file. The digest
 * binds their bytes and the complete argument vector. It is change detection,
 * not a publisher signature, dependency inventory or race-free file lease.
 * Keep trusted inputs unchanged until the session ends. Firmware found by QEMU
 * itself and its shared libraries are outside this digest.
 *
 * Start repeats review and requires that exact digest before opening a child.
 * Only a successfully negotiated, observed paused session is returned. Resume,
 * console and shutdown use the existing UmiVmControl service. Calls block and
 * belong on a persistent worker in graphical applications; one caller at a
 * time per session. Closing a running session force-stops its owned process. */
UmiStatus UmiVmBootReview(const UmiVmBootPlan *plan, UmiVmReport *report);
UmiStatus UmiVmBootStart(const UmiVmBootPlan *plan, const char *expectedFingerprint,
                       UmiVmSession **outSession, UmiVmReport *report);
/* Arguments exclude the command name: targets, plan, review or run. */
int UmiVmBootMain(int argc, char **argv);

#ifdef __cplusplus
}
#endif
#endif
