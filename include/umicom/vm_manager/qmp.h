/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bounded QMP messages, not a general command interpreter. QMP field order is
 * irrelevant; duplicates, malformed JSON and oversized frames are rejected. */
#ifndef UMICOM_VM_MANAGER_QMP_H
#define UMICOM_VM_MANAGER_QMP_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_VM_QMP_FRAME 65536U
#define UMI_VM_CONSOLE_CHUNK 2048U
    typedef enum UmiVmQmpKind {
        UMI_VM_QMP_GREETING=1,UMI_VM_QMP_RETURN=2,UMI_VM_QMP_ERROR=3,UMI_VM_QMP_EVENT=4
    }
    UmiVmQmpKind;
    typedef enum UmiVmCommand {
        UMI_VM_QUERY=0,UMI_VM_PAUSE=1,UMI_VM_RESUME=2,UMI_VM_POWERDOWN=3,UMI_VM_QUIT=4,UMI_VM_CONSOLE_READ=5,UMI_VM_CONSOLE_WRITE=6,UMI_VM_NEGOTIATE=7
    }
    UmiVmCommand;
    typedef struct UmiVmQmpMessage {
        UmiVmQmpKind kind;
        uint64_t id;
        int returnIsString,returnIsObject;
        int hasId,hasRunning,running;
        unsigned versionMajor,versionMinor,versionMicro;
        char state[64],event[80],errorClass[96],description[512];
        unsigned char console[UMI_VM_CONSOLE_CHUNK];
        size_t consoleLength;
    }
    UmiVmQmpMessage;
    /** Parse exactly one JSON frame. No allocation is retained. Greeting version,
                         * status and base64 ring buffer data are interpreted; other valid fields may
                         * be ignored for forward compatibility. Maximum depth 24 and 512 tokens. */
    UmiStatus UmiVmQmpDecode(const void *bytes,size_t length,UmiVmQmpMessage *outMessage);
    /** Construct only a named supported command. IDs are 1..INT64_MAX. Console
                         * bytes are base64 encoded; they cannot inject an additional QMP command. */
    UmiStatus UmiVmQmpEncode(UmiVmCommand command,uint64_t id,const void *console,     size_t consoleLength,char *outFrame,size_t capacity,size_t *outLength);
#ifdef __cplusplus
}
#endif
#endif
