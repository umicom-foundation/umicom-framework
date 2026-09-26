/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: include/umicom/setup_centre/virtual_machine.h
 * Purpose: Explicit QEMU invocation and media-source preparation in native C.
 * Architecture: A VM is an external provider, not another Umicom hypervisor.
 * Plans are data. The canonical process runner owns actual child lifetimes.
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SETUP_CENTRE_VIRTUAL_MACHINE_H
#define UMICOM_SETUP_CENTRE_VIRTUAL_MACHINE_H
#include "umicom/setup_centre/setup.h"
#ifdef __cplusplus
extern "C" {

#endif
#define UMI_SETUP_VM_MAX_ARGUMENTS 40U
#define UMI_SETUP_VM_ARGUMENT_CAPACITY 4096U
    /** This profile intentionally attaches no host disk, shared directory, guest
     * network interface, monitor, USB passthrough or management listening socket.
     * TCG is the explicit baseline for cross-architecture operation. */

    typedef struct UmiSetupVmConfig {

        const char *qemu;

        const char *kernel;

        const char *initrd;

        const char *architecture;
        /* x86_64 or riscv64 */
        unsigned memoryMiB;
        /* 256..8192 */
        unsigned processors;
        /* 1..8 */
        int recovery;

        int interactive;
        /* QEMU's graphical serial console, not a desktop */
    }
    UmiSetupVmConfig;

    typedef struct UmiSetupVmPlan {

        char program[UMI_SETUP_PATH_CAPACITY];

        char arguments[UMI_SETUP_VM_MAX_ARGUMENTS][UMI_SETUP_VM_ARGUMENT_CAPACITY];

        size_t argumentCount;

        char fingerprint[65];

        char kernelHash[65];

        char initrdHash[65];

        char programHash[65];

        UmiSetupVmConfig config;
        /* pointers refer inside this plan; do not copy it */
        char kernel[UMI_SETUP_PATH_CAPACITY];

        char initrd[UMI_SETUP_PATH_CAPACITY];

        char architecture[16];

    }
    UmiSetupVmPlan;

    /** Inspect bounded regular files and recognised kernel headers. Does NOT prove
     * that an initramfs is complete, an emulator is authentic, or a guest boots.
     * The selected host QEMU distribution/firmware must be trusted and compatible. */

    UmiStatus UmiSetupVmReview(const UmiSetupVmConfig *config, UmiSetupVmPlan *outPlan,
    UmiSetupReport *report);

    /** The adapter must execute argv directly, never through a shell. It must own
     * descendants until return. A nonzero child exit is not success. */

    typedef UmiStatus (*UmiSetupExecute)(const char *program, const char *const *arguments,
    size_t count, const char *workingDirectory, void *context, int *exitCode);

    UmiStatus UmiSetupVmRun(const UmiSetupVmPlan *plan, const char *expectedFingerprint,
    UmiSetupExecute execute, void *context, UmiSetupReport *report);

    /** Create a NEW x86-64 media tree containing boot/kernel, boot/initrd,
     * boot/grub/grub.cfg and a preparation receipt. It is NOT a bootable image.
     * A reviewed GRUB ISO producer must supply BIOS/UEFI loader code afterwards.
     * No block device or optical drive is opened. RISC-V media is not implied. */

    UmiStatus UmiSetupMediaPrepare(const char *kernel, const char *initrd,
    const char *destination, UmiSetupReport *report);

    /** Execute a trusted native grub-mkrescue against a prepared tree into a NEW
     * output directory. Linux host only. Checks ISO9660/El Torito structure after
     * completion, not actual BIOS, UEFI, Secure Boot or physical-machine booting.
     * Never writes directly to a USB drive. Failure retains partial output. */

    UmiStatus UmiSetupMediaBuildIso(const char *preparedRoot, const char *grubTool,
    const char *destination, UmiSetupExecute execute, void *context,
    UmiSetupReport *report);

#ifdef __cplusplus
}

#endif
#endif
