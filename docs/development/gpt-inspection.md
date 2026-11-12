# Inspect GPT structures through Framework

![Umicom](../../resources/brand/umicom-logo-on-dark.svg)

Umicom Framework — Sammy Hegab, Umicom Foundation — MIT

The installed component **gpt_image** provides the C23 function **UmiGptImageInspect**. Boot Media uses it for GUID partition-table inspection. A client supplies an exact-read callback over stable bytes; the service opens no file, acquires no device and starts no process.

~~~cmake
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS gpt_image)
target_link_libraries(my_image_reader PRIVATE Umicom::gpt_image)
~~~

## 1. Supply an exact reader

~~~c
#include "umicom/image_inspection/gpt.h"
#include <string.h>

typedef struct ImageBytes {
    const unsigned char *data;
    size_t length;
} ImageBytes;

static UmiStatus ReadImage(void *context, uint64_t offset,
    void *destination, size_t count)
{
    const ImageBytes *image = context;
    if (offset > image->length || count > image->length - offset)
        return UMI_STATUS_IO_ERROR;
    memcpy(destination, image->data + (size_t)offset, count);
    return UMI_STATUS_OK;
}
~~~

The caller must provide a valid ImageBytes object whose length describes its allocation. Keep that object and its data alive until inspection returns. Do not resize or change the bytes from the callback. A real file adapter must return an error for short reads rather than silently filling the remainder with zeros.

## 2. Pass observed geometry

~~~c
UmiGptImageInfo observation = {0};
UmiStatus status = UmiGptImageInspect(ReadImage, &image,
    image.length, 512U, &observation);
if (status == UMI_STATUS_OK) {
    /* Use observation.partitions and the inclusive usable-LBA interval. */
}
~~~

Here image is the caller's initialized ImageBytes. Use 4096U only when the source actually uses 4096-byte logical blocks. A physical-sector size or filesystem allocation size is not a substitute for logical-block geometry. The service supports both block sizes; Boot Media's existing transfer policy still uses its 512-byte image profile.

The format rules used for entry sizes, reservations and reserved fields come from [UEFI 2.11, chapter 5](https://uefi.org/specs/UEFI/2.11/05_GUID_Partition_Table_Format.html). Entry widths use powers of two beginning at 128 bytes. Each side reserves at least 16 KiB for its entry array. Header and active-entry reserved fields are checked. A zero type GUID denotes an unused slot; legacy identity and range bytes in that slot are ignored, while reserved extension bytes remain checked.

## 3. Interpret status before output

| Status | Meaning in this service |
| --- | --- |
| OK | Both header copies and arrays passed this bounded structural profile. The output was replaced. |
| INVALID_ARGUMENT | The callback or output was absent, or length/block geometry was invalid. |
| PARSE_ERROR | Recognised structures were inconsistent, corrupt or outside their allowed geometry. |
| UNAVAILABLE | A revision or otherwise valid profile size exceeded this implementation. |
| OUT_OF_MEMORY | One of the bounded entry-array allocations failed. |
| Callback error | An exact read could not be completed; its status is propagated. |

Every failure leaves output storage byte-for-byte unchanged. Initialize it if this is the first call and always branch on status. An old observation is not evidence about a later rejected input.

The profile accepts 1–256 slots, 128/256/512-byte entries and revision 1 headers. It allocates at most two 128 KiB arrays. Successful calls perform four exact reads: primary header, backup header, primary array and backup array. An empty table is a valid observation, but Boot Media separately refuses to admit an empty GPT as a useful disk image.

## 4. Keep application policy separate

This inspector does not validate the protective MBR, partition names, filesystems, operating-system loaders, signatures or actual firmware boot. It checks the two copies together and refuses inconsistency; it does not recover, rewrite or relocate either copy. The caller remains responsible for source identity, exclusive access and any decision to offer a subsequent operation.

Use the [existing media workflow](boot-media.md) for regular-file inspection, explicit review and copying. Do not add device-opening code to an application solely to call this parser. The Framework media service already owns that authority.

## 5. Qualify the changed contract

From a configured Framework build directory, build **umicom-gpt-image-test**, **umicom-gpt-allocation-test** and **umicom-boot-media-test**, then run:

~~~powershell
ctest -N -R "^framework[.](image_inspection[.]gpt|boot_media[.]gpt)[.]"
ctest --output-on-failure --no-tests=error -R "^framework[.](image_inspection[.]gpt|boot_media[.]gpt)[.]"
~~~

The first command lists registered cases; it does not execute them. The second runs synthetic memory and regular-file fixtures. Allocation cases compile the actual parser with a narrow test-only allocator substitution and check release of partial ownership. Keep the ordinary Framework metadata, SDK installation and installed-consumer checks in qualification too. Authored coverage and source review do not establish a compiler or runtime pass.

When a real image fails, keep its original bytes, record its digest and logical-block size, and inspect the returned status. Do not modify a header merely to make its checksum pass. A corrected producer should regenerate the complete structure, after which structural, byte-copy and real-boot observations must be recorded separately.
