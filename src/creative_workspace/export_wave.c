/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/export_wave.c
 * PURPOSE: Synthesize a bounded note sequence as a real mono PCM-WAVE file.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
static void Little16(unsigned char * p, uint16_t value){
    p[0] = (unsigned char)(value & 255U);
    p[1] = (unsigned char)(value >> 8U);
}

static void Little32(unsigned char * p, uint32_t value){
    for (unsigned i = 0U;i < 4U; ++ i)p[i] = (unsigned char)((value >> (8U * i)) & 255U);
}

static uint64_t SampleAtTick(uint64_t tick, uint32_t tempo)
{
    return tick * 60U * UMI_CREATIVE_SAMPLE_RATE / ((uint64_t)tempo * UMI_CREATIVE_TICKS_PER_BEAT);
}

UmiStatus UmiCreativeExportWave(const UmiCreativeProject * p, UmiCreativeExport * out)
{
    if (p -> noteCount == 0U)return UMI_STATUS_INVALID_STATE;
    uint64_t frameCount = (uint64_t)p -> settings.durationMs * UMI_CREATIVE_SAMPLE_RATE / 1000U;
    size_t dataSize = (size_t)frameCount * 2U, total = dataSize + 44U;
    if (total > UMI_CREATIVE_EXPORT_LIMIT)return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char * bytes = calloc(total, 1U);
    double * mix = calloc((size_t)frameCount, sizeof( * mix));
    if (bytes == NULL || mix == NULL){
        free(bytes);
        free(mix);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t voices = 1U;
    for (size_t i = 0U;i < p -> noteCount; ++ i){
        size_t count = 0U;
        uint64_t tick = p -> notes[i].startTick;
        for (size_t j = 0U;j < p -> noteCount; ++ j)if (tick >= p -> notes[j].startTick && tick < (uint64_t)p -> notes[j].startTick + p -> notes[j].durationTicks) ++ count;
        if (count > voices)voices = count;
    }
    const double tau = 6.283185307179586476925286766559;
    for (size_t i = 0U;i < p -> noteCount; ++ i){
        const UmiCreativeNote * note = & p -> notes[i];
        uint64_t start = SampleAtTick(note -> startTick, p -> settings.tempoBpm);
        uint64_t end = SampleAtTick((uint64_t)note -> startTick + note -> durationTicks, p -> settings.tempoBpm);
        if (end > frameCount){
            free(bytes);
            free(mix);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        uint64_t length = end - start, fade = UMI_CREATIVE_SAMPLE_RATE / 200U;
        if (fade > length / 2U)fade = length / 2U;
        double frequency = 440.0 * exp2(((double)note -> pitch-69.0) / 12.0);
        double gain = 0.8 * (double)note -> velocity / (127.0 * (double)voices);
        for (uint64_t j = 0U;j < length; ++ j){
            double envelope = 1.0;
            if (fade > 0U){
                if (j < fade)envelope = (double)j / (double)fade;
                if (length-1U - j < fade)envelope = (double)(length-1U - j) / (double)fade;
            }
            mix[start + j] += gain * envelope * sin(tau * frequency * (double)j / (double)UMI_CREATIVE_SAMPLE_RATE);
        }
    }
    memcpy(bytes, "RIFF", 4U);
    Little32(bytes + 4U, (uint32_t)(total-8U));
    memcpy(bytes + 8U, "WAVEfmt ", 8U);
    Little32(bytes + 16U, 16U);
    Little16(bytes + 20U, 1U);
    Little16(bytes + 22U, 1U);
    Little32(bytes + 24U, UMI_CREATIVE_SAMPLE_RATE);
    Little32(bytes + 28U, UMI_CREATIVE_SAMPLE_RATE * 2U);
    Little16(bytes + 32U, 2U);
    Little16(bytes + 34U, 16U);
    memcpy(bytes + 36U, "data", 4U);
    Little32(bytes + 40U, (uint32_t)dataSize);
    for (size_t i = 0U;i < (size_t)frameCount; ++ i){
        double value = mix[i] * 32767.0;
        if (!isfinite(value) || value > 32767.0 || value < -32768.0){
            free(bytes);
            free(mix);
            return UMI_STATUS_INTERNAL_ERROR;
        }
        int32_t rounded = (int32_t)(value >= 0.0 ? floor(value + 0.5) : ceil(value-0.5));
        Little16(bytes + 44U + i * 2U, (uint16_t)(int16_t)rounded);
    }
    free(mix);
    out -> bytes = bytes;
    out -> size = total;
    return UMI_STATUS_OK;
}

