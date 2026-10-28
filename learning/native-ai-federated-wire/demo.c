/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-federated-wire/demo.c
 *
 * PURPOSE:
 *   Demonstrate separate native coordinator/worker processes exchanging
 *   synthetic, checksum-protected federated frames through local files.
 *   Use the canonical Framework job admission and FedAvg arithmetic owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_wire.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This is a LOCAL file-exchange experiment, not a training, networking,
 * authentication, blockchain or secure aggregation implementation. */
static UmiAiTrainingFederatedWireRecord MakeAssignment(unsigned worker)
{
    UmiAiTrainingFederatedWireRecord message = {0};
    message.kind = UMI_AI_FEDERATED_WIRE_ASSIGNMENT;
    message.roundId = UINT64_C(41);
    message.sequence = (uint64_t)worker;
    message.assignment.contributorId[0] = (uint8_t)worker;
    message.assignment.baseCheckpointSha256[0] = 88U;
    message.assignment.datasetCommitmentSha256[0] = (uint8_t)(100U + worker);
    message.assignment.dataPolicySha256[0] = 99U;
    message.assignment.distinctSampleCount = worker == 1U ? UINT64_C(100) :
        (worker == 2U ? UINT64_C(250) : UINT64_C(150));
    message.assignment.localSteps = UINT64_C(2);
    message.assignment.declaredTransferBytes = 256U;
    return message;
}
static UmiAiTrainingFederatedJobConfig MakeConfig(void)
{
    UmiAiTrainingFederatedJobConfig config = {0};
    config.round.parameterCount = 1U;
    config.round.maximumContributors = 3U;
    config.round.maximumDeltaL2Norm = 1.0F;
    config.round.baseCheckpointSha256[0] = 88U;
    config.roundId = UINT64_C(41);
    config.maximumLocalSteps = UINT64_C(8);
    config.maximumSamplesPerAssignment = UINT64_C(1000);
    config.maximumTransferBytesPerAssignment = 1024U;
    config.maximumReservedTransferBytes = 1024U;
    config.dataPolicySha256[0] = 99U;
    return config;
}
static int ReadFile(const char *path,uint8_t *data,size_t *written)
{
    FILE *file = fopen(path,"rb");
    if (file == NULL) return 0;
    size_t count = fread(data,1U,UMI_AI_FEDERATED_WIRE_MAX_BYTES,file);
    int extra = fgetc(file);
    int okay = ferror(file) == 0 && extra == EOF &&
        count >= UMI_AI_FEDERATED_WIRE_HEADER_BYTES + 32U;
    if (fclose(file) != 0) okay = 0;
    if (okay) *written = count;
    return okay;
}
static int WriteFile(const char *path,const uint8_t *data,size_t bytes)
{
    FILE *file = fopen(path,"wb");
    if (file == NULL) return 0;
    int okay = fwrite(data,1U,bytes,file) == bytes;
    if (fflush(file) != 0) okay = 0;
    if (fclose(file) != 0) okay = 0;
    return okay;
}
static int Emit(unsigned worker,const char *destination)
{
    if (worker < 1U || worker > 3U) return 1;
    UmiAiTrainingFederatedWireRecord assignment = MakeAssignment(worker);
    uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
    size_t count = 0U;
    if (UmiAiTrainingFederatedWireEncode(&assignment,frame,
        sizeof(frame),&count) != UMI_STATUS_OK ||
        !WriteFile(destination,frame,count)) return 1;
    printf("Coordinator: staged worker %u assignment (%zu bytes)\n",worker,count);
    return 0;
}
static int Worker(const char *input,const char *destination)
{
    uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
    size_t received = 0U;
    UmiAiTrainingFederatedWireRecord assignment = {0};
    if (!ReadFile(input,frame,&received) ||
        UmiAiTrainingFederatedWireDecode(frame,received,&assignment,
            NULL,0U) != UMI_STATUS_OK ||
        assignment.kind != UMI_AI_FEDERATED_WIRE_ASSIGNMENT) return 1;
    const unsigned worker = (unsigned)assignment.assignment.contributorId[0];
    if (worker < 1U || worker > 3U) return 1;
    /* Synthetic arithmetic, NOT a trained gradient. Real workers must first
     * authenticate the coordinator and independently authorise the shard. */
    const float deltas[3] = {0.05F,-0.01F,0.08F};
    float scalar = deltas[worker-1U];
    assignment.kind = UMI_AI_FEDERATED_WIRE_RESULT;
    assignment.delta = &scalar;
    assignment.deltaCount = 1U;
    size_t output = 0U;
    if (UmiAiTrainingFederatedWireEncode(&assignment,frame,
        sizeof(frame),&output) != UMI_STATUS_OK ||
        !WriteFile(destination,frame,output)) return 1;
    printf("Worker %u: synthetic delta %.3f, frame %zu bytes\n",
           worker,(double)scalar,output);
    return 0;
}
static int Finish(const char *const paths[3])
{
    UmiAiTrainingFederatedJob *job = NULL;
    UmiAiTrainingFederatedJobConfig config = MakeConfig();
    if (UmiAiTrainingFederatedJobCreate(&config,&job) != UMI_STATUS_OK)
        return 1;
    int okay = 1;
    for (unsigned worker = 1U; worker <= 3U; ++worker) {
        UmiAiTrainingFederatedWireRecord assignment = MakeAssignment(worker);
        UmiAiTrainingFederatedTicket ticket = {0};
        if (UmiAiTrainingFederatedJobAdmit(job,&assignment.assignment,
            &ticket) != UMI_STATUS_OK) {okay=0;break;}
        uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
        size_t received = 0U;
        float delta[1] = {0.0F};
        UmiAiTrainingFederatedWireRecord result = {0};
        UmiAiTrainingFederatedContribution contribution = {0};
        if (!ReadFile(paths[worker-1U],frame,&received) ||
            UmiAiTrainingFederatedWireDecode(frame,received,&result,
                delta,1U) != UMI_STATUS_OK ||
            UmiAiTrainingFederatedWireMatchResult(&result,
                &assignment.assignment,41U,ticket.serial) != UMI_STATUS_OK ||
            UmiAiTrainingFederatedWireContribution(&result,
                &contribution) != UMI_STATUS_OK ||
            UmiAiTrainingFederatedJobComplete(job,
                ticket,&contribution) != UMI_STATUS_OK) {okay=0;break;}
    }
    float aggregate = 0.0F;
    UmiAiTrainingFederatedRoundReport report = {0};
    if (okay &&
        UmiAiTrainingFederatedJobFinalise(job,&aggregate,1U,
            &report) != UMI_STATUS_OK) okay=0;
    if (okay && (report.acceptedContributors != 3U ||
                 report.totalDistinctSamples != UINT64_C(500) ||
                 fabsf(aggregate-0.029F)>0.000001F)) okay=0;
    if (okay) printf("Three distinct worker processes: synthetic weighted delta %.3f\n",
                     (double)aggregate);
    UmiAiTrainingFederatedJobDestroy(job);
    return okay ? 0 : 1;
}
int main(int argc,char **argv)
{
    if (argc == 4 && strcmp(argv[1],"emit") == 0) {
        char *end = NULL;
        unsigned long worker = strtoul(argv[2],&end,10);
        if (end == argv[2] || *end != '\0' || worker < 1UL || worker > 3UL)
            return 2;
        return Emit((unsigned)worker,argv[3]);
    }
    if (argc == 4 && strcmp(argv[1],"worker") == 0)
        return Worker(argv[2],argv[3]);
    if (argc == 5 && strcmp(argv[1],"finish") == 0) {
        const char *paths[3] = {argv[2],argv[3],argv[4]};
        return Finish(paths);
    }
    fprintf(stderr,"Use: emit <worker 1..3> <assignment-file>\n"
                   "     worker <assignment-file> <result-file>\n"
                   "     finish <result-1> <result-2> <result-3>\n");
    return 2;
}
