/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/native_launcher/sha256.c
 * Purpose: Portable streaming SHA-256 without a process or interpreter.
 * Unsigned 32-bit wraparound is intentional in the compression function.
 * Delivery hashes detect changed bytes; they do not identify a trusted signer.
 *---------------------------------------------------------------------------*/
#include "umicom/native_launcher/sha256.h"
#include <string.h>
static const uint32_t ROUND_CONSTANTS[64] = {
    0x428a2f98U,0x71374491U,0xb5c0fbcfU,0xe9b5dba5U,0x3956c25bU,0x59f111f1U,0x923f82a4U,0xab1c5ed5U,
    0xd807aa98U,0x12835b01U,0x243185beU,0x550c7dc3U,0x72be5d74U,0x80deb1feU,0x9bdc06a7U,0xc19bf174U,
    0xe49b69c1U,0xefbe4786U,0x0fc19dc6U,0x240ca1ccU,0x2de92c6fU,0x4a7484aaU,0x5cb0a9dcU,0x76f988daU,
    0x983e5152U,0xa831c66dU,0xb00327c8U,0xbf597fc7U,0xc6e00bf3U,0xd5a79147U,0x06ca6351U,0x14292967U,
    0x27b70a85U,0x2e1b2138U,0x4d2c6dfcU,0x53380d13U,0x650a7354U,0x766a0abbU,0x81c2c92eU,0x92722c85U,
    0xa2bfe8a1U,0xa81a664bU,0xc24b8b70U,0xc76c51a3U,0xd192e819U,0xd6990624U,0xf40e3585U,0x106aa070U,
    0x19a4c116U,0x1e376c08U,0x2748774cU,0x34b0bcb5U,0x391c0cb3U,0x4ed8aa4aU,0x5b9cca4fU,0x682e6ff3U,
    0x748f82eeU,0x78a5636fU,0x84c87814U,0x8cc70208U,0x90befffaU,0xa4506cebU,0xbef9a3f7U,0xc67178f2U
};
static uint32_t Rotate(uint32_t value, unsigned bits)
{
    return (value >> bits) | (value << (32U - bits));
}
static void Compress(UmiNativeSha256 *context, const unsigned char block[64])
{
    uint32_t schedule[64];
    for (size_t i = 0U; i < 16U; ++i) {
        size_t p = i * 4U;
        schedule[i] = ((uint32_t)block[p] << 24U) | ((uint32_t)block[p+1U] << 16U)
            | ((uint32_t)block[p+2U] << 8U) | (uint32_t)block[p+3U];
    }
    for (size_t i = 16U; i < 64U; ++i) {
        uint32_t a = schedule[i-15U], b = schedule[i-2U];
        schedule[i] = schedule[i-16U] + (Rotate(a,7U) ^ Rotate(a,18U) ^ (a >> 3U))
            + schedule[i-7U] + (Rotate(b,17U) ^ Rotate(b,19U) ^ (b >> 10U));
    }
    uint32_t a=context->words[0], b=context->words[1], c=context->words[2], d=context->words[3];
    uint32_t e=context->words[4], f=context->words[5], g=context->words[6], h=context->words[7];
    for (size_t i = 0U; i < 64U; ++i) {
        uint32_t first = h + (Rotate(e,6U) ^ Rotate(e,11U) ^ Rotate(e,25U))
            + ((e & f) ^ ((~e) & g)) + ROUND_CONSTANTS[i] + schedule[i];
        uint32_t second = (Rotate(a,2U) ^ Rotate(a,13U) ^ Rotate(a,22U))
            + ((a & b) ^ (a & c) ^ (b & c));
        h=g; g=f; f=e; e=d+first; d=c; c=b; b=a; a=first+second;
    }
    context->words[0]+=a; context->words[1]+=b; context->words[2]+=c; context->words[3]+=d;
    context->words[4]+=e; context->words[5]+=f; context->words[6]+=g; context->words[7]+=h;
}
void UmiNativeSha256Init(UmiNativeSha256 *context)
{
    if (context == NULL) return;
    *context = (UmiNativeSha256){.words={0x6a09e667U,0xbb67ae85U,0x3c6ef372U,0xa54ff53aU,
        0x510e527fU,0x9b05688cU,0x1f83d9abU,0x5be0cd19U}};
}
UmiStatus UmiNativeSha256Update(UmiNativeSha256 *context, const void *data, size_t length)
{
    if (context == NULL || (data == NULL && length != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (context->finalised || context->used >= 64U) return UMI_STATUS_INVALID_STATE;
    if (context->totalBytes > UINT64_MAX / 8U || (uint64_t)length > UINT64_MAX / 8U - context->totalBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *input = data;
    context->totalBytes += (uint64_t)length;
    while (length != 0U) {
        size_t count = 64U - context->used;
        if (count > length) count = length;
        memcpy(context->block + context->used, input, count);
        context->used += count; input += count; length -= count;
        if (context->used == 64U) { Compress(context, context->block); context->used = 0U; }
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiNativeSha256Final(UmiNativeSha256 *context, unsigned char outDigest[32])
{
    if (context == NULL || outDigest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (context->finalised || context->used >= 64U || context->totalBytes > UINT64_MAX / 8U)
        return UMI_STATUS_INVALID_STATE;
    uint64_t bits = context->totalBytes * 8U;
    context->block[context->used++] = 0x80U;
    if (context->used > 56U) {
        memset(context->block + context->used, 0, 64U - context->used);
        Compress(context, context->block); context->used = 0U;
    }
    memset(context->block + context->used, 0, 56U - context->used);
    for (size_t i = 0U; i < 8U; ++i) context->block[63U-i] = (unsigned char)(bits >> (i*8U));
    Compress(context, context->block);
    for (size_t i = 0U; i < 8U; ++i)
        for (size_t j = 0U; j < 4U; ++j) outDigest[i*4U+j] = (unsigned char)(context->words[i] >> (24U-j*8U));
    context->used = 0U; context->finalised = 1;
    return UMI_STATUS_OK;
}
void UmiNativeSha256Hex(const unsigned char digest[32], char outHex[65])
{
    static const char HEX[] = "0123456789abcdef";
    if (digest == NULL || outHex == NULL) return;
    for (size_t i = 0U; i < 32U; ++i) { outHex[2U*i]=HEX[digest[i]>>4U]; outHex[2U*i+1U]=HEX[digest[i]&15U]; }
    outHex[64] = '\0';
}
UmiStatus UmiNativeSha256Buffer(const void *data, size_t length, char outHex[65])
{
    UmiNativeSha256 context;
    unsigned char digest[32];
    if (outHex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    outHex[0] = '\0'; UmiNativeSha256Init(&context);
    UmiStatus status = UmiNativeSha256Update(&context, data, length);
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Final(&context, digest);
    if (status == UMI_STATUS_OK) UmiNativeSha256Hex(digest, outHex);
    return status;
}
