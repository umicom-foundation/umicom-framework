/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/audio_arrangement/fixture.h
 * PURPOSE: Build short independent PCM fixtures and inspect rendered signed samples.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AUDIO_ARRANGEMENT_TEST_FIXTURE_H
#define UMICOM_AUDIO_ARRANGEMENT_TEST_FIXTURE_H
#include "../creative_library/check.h"
#include "umicom/creative_workspace/audio_arrangement.h"
static inline void Put16(unsigned char *out, uint16_t value)
{
    out[0] = (unsigned char)(value & 255U);
    out[1] = (unsigned char)(value >> 8U);
}
static inline void Put32(unsigned char *out, uint32_t value)
{
    for (size_t i = 0U; i < 4U; ++i)
        out[i] = (unsigned char)((value >> (8U * i)) & 255U);
}
static inline UmiCreativeAsset *Wave(uint32_t rate, unsigned channels, int16_t left, int16_t right)
{
    size_t size = 44U + 128U * channels * 2U;
    unsigned char bytes[44U + 128U * 4U] = {0};
    memcpy(bytes, "RIFF", 4U);
    Put32(bytes + 4U, (uint32_t)(size - 8U));
    memcpy(bytes + 8U, "WAVEfmt ", 8U);
    Put32(bytes + 16U, 16U);
    Put16(bytes + 20U, 1U);
    Put16(bytes + 22U, (uint16_t)channels);
    Put32(bytes + 24U, rate);
    Put32(bytes + 28U, rate * channels * 2U);
    Put16(bytes + 32U, (uint16_t)(channels * 2U));
    Put16(bytes + 34U, 16U);
    memcpy(bytes + 36U, "data", 4U);
    Put32(bytes + 40U, (uint32_t)(size - 44U));
    for (size_t i = 0U; i < 128U; ++i)
        for (unsigned channel = 0U; channel < channels; ++channel)
            Put16(bytes + 44U + (i * channels + channel) * 2U, (uint16_t)(channel == 0U ? left : right));
    UmiCreativeAsset *asset = NULL;
    CHECK(UmiCreativeAssetCapture("Fixture wave", UMI_CREATIVE_ASSET_AUDIO, bytes, size, NULL, &asset) ==
          UMI_STATUS_OK);
    return asset;
}
static inline UmiCreativeAssetLibrary *Sources(uint32_t rate, unsigned channels, int16_t left, int16_t right)
{
    UmiCreativeAssetLibrary *library = NULL;
    CHECK(UmiCreativeAssetLibraryCreate("Audio sources", &library) == UMI_STATUS_OK);
    UmiCreativeAsset *asset = Wave(rate, channels, left, right);
    CHECK(UmiCreativeAssetLibraryInsert(library, "voice", &asset) == UMI_STATUS_OK);
    return library;
}
static inline UmiCreativeAudioArrangement Plan(void)
{
    UmiCreativeAudioArrangement plan;
    CHECK(UmiCreativeAudioArrangementInit("Mix", 8000U, &plan) == UMI_STATUS_OK);
    UmiCreativeAudioPlacement clip = {"first", "voice", 0U, 0U, 16U, 1000U, 0U, 0U};
    CHECK(UmiCreativeAudioArrangementPut(&plan, &clip, false) == UMI_STATUS_OK);
    return plan;
}
static inline int16_t Sample(const UmiCreativeAsset *asset, size_t frame, unsigned channel)
{
    const void *data = NULL;
    size_t size = 0U;
    CHECK(UmiCreativeAssetBytes(asset, &data, &size) == UMI_STATUS_OK &&
          44U + frame * 4U + channel * 2U + 2U <= size);
    const unsigned char *bytes = (const unsigned char *)data + 44U + frame * 4U + channel * 2U;
    uint16_t bits = (uint16_t)((uint16_t)bytes[0] | (uint16_t)((uint16_t)bytes[1] << 8U));
    return (int16_t)(bits <= INT16_MAX ? (int32_t)bits : (int32_t)bits - 65536);
}
#endif
