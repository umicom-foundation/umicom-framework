/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/inventory_identity.c
 * Purpose: Share canonical configured-inventory identity across planning and observation.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/distribution/runtime/inventory.h"
#include "umicom/base/sha256.h"
#include <string.h>

/* Length prefixes make identities unambiguous even when fields contain tabs,
 * newlines or strings that resemble record separators. Encoding is host-neutral. */
static UmiStatus Number(UmiSha256 *hash, uint64_t value)
{
    unsigned char bytes[8];
    for (size_t i = 0U; i < sizeof(bytes); ++i) {
        bytes[sizeof(bytes) - i - 1U] = (unsigned char)(value & 255U);
        value >>= 8U;
    }
    return UmiSha256Update(hash, bytes, sizeof(bytes));
}
static UmiStatus Field(UmiSha256 *hash, const char *value)
{
    size_t n = strlen(value);
    UmiStatus status = Number(hash, (uint64_t)n);
    return status == UMI_STATUS_OK ? UmiSha256Update(hash, value, n) : status;
}
static UmiStatus Finish(UmiSha256 *hash, char output[65])
{
    unsigned char digest[32];
    UmiStatus status = UmiSha256Final(hash, digest);
    if (status == UMI_STATUS_OK) UmiSha256Hex(digest, output);
    return status;
}
/* Keep the original source-inventory domain and field encoding stable so old
 * source observations retain their meaning when other services share the identity. */
UmiStatus UmiReleaseInventoryFingerprint(const UmiReleaseInventory *inventory, char output[65])
{
    if (inventory == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSha256 hash;
    UmiSha256Init(&hash);
    const char *context[] = {"umicom-source-inventory-1",
        UmiReleaseInventoryProducer(inventory), UmiReleaseInventoryGeneration(inventory),
        UmiReleaseInventorySourceRoot(inventory), UmiReleaseInventoryBuildRoot(inventory),
        UmiReleaseInventoryConfiguration(inventory)};
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(context) / sizeof(context[0]); ++i)
        status = Field(&hash, context[i]);
    size_t count = UmiReleaseInventoryCount(inventory);
    if (status == UMI_STATUS_OK) status = Number(&hash, (uint64_t)count);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        const UmiReleaseInventoryRecord *record = UmiReleaseInventoryAt(inventory, i);
        status = Number(&hash, (uint64_t)record->kind);
        const char *fields[] = {record->identity, record->owner, record->state, record->detail};
        for (size_t j = 0U; status == UMI_STATUS_OK && j < sizeof(fields) / sizeof(fields[0]); ++j)
            status = Field(&hash, fields[j]);
    }
    return status == UMI_STATUS_OK ? Finish(&hash, output) : status;
}
