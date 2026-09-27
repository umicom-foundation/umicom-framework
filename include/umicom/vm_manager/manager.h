/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * VM profiles and supervised QEMU sessions. GUI and CLI hosts use this C ABI;
 * neither owns a second state machine, QMP parser, database or shell launcher.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VM_MANAGER_MANAGER_H
#define UMICOM_VM_MANAGER_MANAGER_H
#include "umicom/vm_manager/qmp.h"
#include "umicom/data/data_server.h"
#include "umicom/platform/process_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_VM_PATH 1024U
#define UMI_VM_MAX_PROFILES 64U
#define UMI_VM_MAX_RUNTIME_FILES 1024U
    typedef enum UmiVmArchitecture {
        UMI_VM_X86_64=1,UMI_VM_RISCV64=2
    }
    UmiVmArchitecture;
    typedef struct UmiVmProfile {
        char id[64],name[128];
        char runtimeDirectory[UMI_VM_PATH];
        char imageBundle[UMI_VM_PATH];
        char diskDirectory[UMI_VM_PATH];
        /* Empty means no virtual disk. */
        UmiVmArchitecture architecture;
        unsigned memoryMiB,processors;
        int recovery;
        uint64_t revision;
    }
    UmiVmProfile;
    typedef struct UmiVmReport {
        UmiStatus status;
        int outputCreated,processLaunched;
        char fingerprint[65];
        char detail[384];
    }
    UmiVmReport;
    typedef struct UmiVmSession UmiVmSession;
    typedef struct UmiVmSnapshot {
        int processRunning,controlAvailable,shutdownRequested;
        int guestRunning;
        /* QMP's observation, not OS health. */
        int exitCode;
        uint64_t processId,commands,events;
        char state[64],qemuVersion[64],lastEvent[80],lastError[512];
        char diagnostics[UMI_CHANNEL_DIAGNOSTIC_CAPACITY];
    }
    UmiVmSnapshot;
    typedef UmiStatus (*UmiVmProfileVisitor)(const UmiVmProfile *profile,void *context);
    void UmiVmProfileInit(UmiVmProfile *profile);
    UmiStatus UmiVmProfileValidate(const UmiVmProfile *profile);
    /** Data Server is borrowed, must have no caller transaction, and must not be
                         * concurrently operated through the same connection. expectedRevision=0 creates
                         * a missing ID; an update requires the current revision. No save starts a VM.
                         * A failed commit is rolled back; a rollback failure reports INVALID_STATE. */
    UmiStatus UmiVmProfileSave(UmiDataServer *server,const UmiVmProfile *profile,     uint64_t expectedRevision,uint64_t *outRevision);
    UmiStatus UmiVmProfileLoad(const UmiDataServer *server,const char *id,UmiVmProfile *outProfile);
    /** Visitor receives a borrowed profile valid only during its call. It must
     * not re-enter this Data Server connection or mutate the current catalogue. */
    UmiStatus UmiVmProfileVisit(const UmiDataServer *server,UmiVmProfileVisitor visitor,void *context);
    UmiStatus UmiVmProfileRemove(UmiDataServer *server,const char *id,uint64_t expectedRevision);
    /** Package an explicitly enumerated, publisher-supplied native runtime into a
                         * NEW directory. Inventory and provenance notes are checked, not legal sufficiency
                         * or authenticity. No downloads or self-extraction. RuntimeVerify never executes. */
    UmiStatus UmiVmRuntimePack(const char *source,const char *inventory,const char *destination,UmiVmReport *report);
    UmiStatus UmiVmRuntimeVerify(const char *root,UmiVmReport *report);
    /** Emit native packer records assigning this sealed runtime to the optional
                         * VM-manager application. Output is a NEW data file, not executable code. */
    UmiStatus UmiVmRuntimeComponent(const char *root,const char *output,UmiVmReport *report);
    /** Review exact runtime/image identities, resource choices and a NEW run
                         * directory. A digest is change detection, not authenticated consent. */
    UmiStatus UmiVmReview(const UmiVmProfile *profile,const char *runDirectory,UmiVmReport *report);
    /** Revalidate the reviewed fingerprint, acquire an optional disk lease, create
                         * a new run directory and start QEMU PAUSED. Only Resume starts guest execution.
                         * All operations are blocking worker calls; one caller at a time per session.
     * On Linux the creating thread must outlive the session's child (see the
     * process-channel contract). Windows uses process-owned job lifetime. */
    UmiStatus UmiVmStart(const UmiVmProfile *profile,const char *runDirectory,     const char *expectedFingerprint,UmiVmSession **outSession,UmiVmReport *report);
    UmiStatus UmiVmControl(UmiVmSession *session,UmiVmCommand command,     const void *input,size_t inputLength,void *outConsole,size_t consoleCapacity,     size_t *outConsoleLength,UmiVmReport *report);
    UmiStatus UmiVmObserve(UmiVmSession *session,UmiVmSnapshot *outSnapshot);
    /** Force stops only the owned process. Ask before use; this is not guest shutdown. */
    UmiStatus UmiVmForceStop(UmiVmSession *session,UmiVmReport *report);
    /** Destroys all owned state; force stops a still-running child. */
    void UmiVmSessionDestroy(UmiVmSession *session);
    /** Creates a blank standalone qcow2 in a NEW owned directory. Never a raw device.
                         * A cold checkpoint is an offline byte copy to a NEW directory, not RAM state
                         * or a filesystem-consistency guarantee. Neither operation replaces a disk. */
    UmiStatus UmiVmDiskCreate(const char *runtime,const char *destination,     uint64_t virtualBytes,UmiVmReport *report);
    UmiStatus UmiVmDiskCheckpoint(const char *runtime,const char *diskDirectory,     const char *destination,UmiVmReport *report);
    int UmiVmMain(int argc,char **argv);
#ifdef __cplusplus
}
#endif
#endif
