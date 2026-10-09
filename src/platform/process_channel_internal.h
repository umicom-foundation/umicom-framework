/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_PROCESS_CHANNEL_INTERNAL_H
#define UMICOM_PROCESS_CHANNEL_INTERNAL_H
#include "umicom/platform/process_channel.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
struct UmiProcessChannel {
#ifdef _WIN32
    void *process,*job,*input,*output,*error;
#else
    int input,output,error;
    int process;
#endif
    UmiProcessChannelSnapshot snapshot;
};
void PcDiagnostic(UmiProcessChannel *channel,const char *data,size_t size);
UmiStatus PcValidate(const UmiProcessChannelRequest *request);
uint64_t PcMilliseconds(void);
/* Validate portable overrides before either native launch implementation reads them. */
UmiStatus PcEnvironmentValidate(const UmiEnvironmentVariable *environment, size_t count);
#endif
