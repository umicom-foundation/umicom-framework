#ifndef UMICOM_MESSAGING_MESSAGE_H
#define UMICOM_MESSAGING_MESSAGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiMessageKind {
    UMI_MESSAGE_COMMAND = 1,
    UMI_MESSAGE_EVENT = 2,
    UMI_MESSAGE_QUERY = 3,
    UMI_MESSAGE_REPLY = 4
} UmiMessageKind;

typedef struct UmiMessageEnvelope {
    uint32_t structure_size;
    uint32_t schema_version;
    UmiMessageKind kind;
    uint64_t sequence;
    uint64_t correlation_id;
    const char *name;
    const char *payload;
} UmiMessageEnvelope;

#ifdef __cplusplus
}
#endif

#endif
