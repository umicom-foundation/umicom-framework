/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/vm_manager/boot_plan_lesson.c
 * PURPOSE: Demonstrate an owned boot plan without starting an emulator.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vm_manager/boot.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiVmBootRequest request;
    if (UmiVmBootRequestInit(&request, UMI_VM_BOOT_UMICOM_KERNEL) != UMI_STATUS_OK) return 1;
#ifdef _WIN32
    strcpy(request.executable, "C:/Tools/QEMU/qemu-system-riscv64.exe");
    strcpy(request.workingDirectory, "C:/VMs/practice");
    strcpy(request.firmware, "C:/Kernel/build/bin/umicom-system.elf");
#else
    strcpy(request.executable, "/usr/bin/qemu-system-riscv64");
    strcpy(request.workingDirectory, "/home/developer/vms/practice");
    strcpy(request.firmware, "/home/developer/kernel/build/bin/umicom-system.elf");
#endif
    /* Planning copies these values and does no I/O. A real application presents
     * the arguments, reviews input files on a worker, then requests a start. */
    UmiVmBootPlan *plan = NULL;
    if (UmiVmBootPlanCreate(&request, &plan) != UMI_STATUS_OK) return 1;
    printf("Program: %s\n", UmiVmBootPlanProgram(plan));
    for (size_t index = 0; index < UmiVmBootPlanArgumentCount(plan); ++index)
        printf("Argument %zu: %s\n", index + 1U, UmiVmBootPlanArgument(plan, index));
    UmiVmBootPlanDestroy(plan);
    puts("Only a plan was created. No file was read and no guest was started.");
    return 0;
}
