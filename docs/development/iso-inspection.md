# ISO inspection through a read-only port

![Umicom](../../resources/brand/umicom-logo-on-dark.svg)

Umicom Framework — Sammy Hegab, Umicom Foundation — MIT

The `Umicom::iso_image` component supplies `UmiIsoImageInspect` in `umicom/image_inspection/iso.h`. Setup and Boot Media use it for optical-format observations. Applications can link the same component through the installed SDK:

~~~cmake
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS iso_image)
target_link_libraries(my_image_reader PRIVATE Umicom::iso_image)
~~~

## Connect a reader

1. Keep the source bytes and callback context alive for the entire synchronous call.
2. Provide an exact-read function. It must fill the requested range or return a nonzero UmiStatus. Never report success after a partial read.
3. Pass the stable source length. Changes to that source during inspection are outside the callback contract.
4. Inspect the return status before using the output. The result is replaced only on success; errors preserve the previous value.
5. Release your source after the call returns. Framework retains no context or buffer and creates no worker.

~~~c
typedef struct MemorySource {
    const unsigned char *bytes;
    size_t length;
} MemorySource;

static UmiStatus ReadAt(void *context, uint64_t offset,
    void *destination, size_t length)
{
    MemorySource *source = context;
    if (offset > (uint64_t)source->length ||
        (uint64_t)length > (uint64_t)source->length - offset)
        return UMI_STATUS_IO_ERROR;
    memcpy(destination, source->bytes + (size_t)offset, length);
    return UMI_STATUS_OK;
}
~~~

Include the public ISO header and string.h for this adapter. Its caller must supply a valid MemorySource; a real allocation length bounds the conversion to size_t. The [complete installed consumer](https://github.com/umicom-foundation/Bits2Banking/tree/main/labs/native/framework-iso-inspection) includes construction, use and cleanup.

## Interpret the result

| Result | Meaning |
| --- | --- |
| OK, zero flags | Input was not identified as an ISO by this profile. |
| OK, iso9660 only | Bounded volume descriptors were accepted; no supported boot catalogue was present. |
| OK, elTorito | At least one supported BIOS or EFI no-emulation entry was accepted. |
| PARSE_ERROR | A recognised record was malformed or exceeded a required range. |
| UNAVAILABLE | A boot profile or catalogue extension was outside this implementation. |
| Callback error | The supplied reader could not complete a requested read. |

The parser observes at most 64 descriptors and one 2048-byte catalogue. It checks the duplicate byte-order fields for volume size, required block size, descriptor termination, catalogue validation checksum, entry ranges and section counts. It refuses hidden nonzero trailer bytes after an empty catalogue header.

EFI platform 0xef has a special rule for load counts zero and one: they extend to the end of the source. Other counts use 512-byte units. The entry start uses 2048-byte logical blocks and must always be inside the source. These observations do not validate every filesystem record, the EFI partition contents, signatures, firmware compatibility or a real boot.

## Reuse filesystem authority

Do not copy native path or handle checks into an application merely to call this parser. Boot Media already supplies an owned-handle adapter and a larger media workflow. Setup supplies checked regular-file reads after an explicit producer operation. Its reopened reads compare file sizes; that comparison does not exclude a concurrent same-size replacement. Keep producer output private while it is inspected.

For a malformed result, retain the source and record the status, Framework revision and input digest before changing it. For an unavailable result, identify the unsupported format rather than treating it as corruption. Qualify a new profile with independent fixture bytes and actual producer artifacts before broadening acceptance. Read [native boot-media contracts](boot-media.md) for transfer and device limits.
