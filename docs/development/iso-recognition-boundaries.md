![Umicom](../../resources/brand/umicom-logo.svg)

# Recognise a truncated optical image without overreading

The image inspector distinguishes three results: unrelated input, a recognised but malformed structure, and an accepted structural observation. A file extension does not select one of these results. The parser reads bounded bytes supplied by its caller.

This distinction matters to clients that try more than one image format. If an ISO identifier is present but its descriptor is incomplete, returning “not ISO” could let the caller continue under another format's policy. Framework returns `PARSE_ERROR` for that recognised truncation.

## Locate the boundary

The supported ISO profile begins its volume descriptors at byte `16 * 2048`. The first byte identifies a descriptor type; the following five bytes contain `CD001`. Six available bytes at that offset therefore suffice for format recognition, even though a complete descriptor needs 2048 bytes.

| Available source bytes | Inspector behavior |
| --- | --- |
| Fewer than `16 * 2048 + 6` | No complete identifier is available; return zero format flags without a read |
| A complete identifier but an incomplete first descriptor | Read only the six-byte prefix; refuse `CD001` as truncated |
| A different complete prefix | Return zero format flags |
| A complete first descriptor | Use the existing descriptor parser and its bounded sequence checks |

The version byte is not needed to establish that a truncated structure is attempting to describe this format. A recognised structure still has to satisfy the full parser's version, descriptor and catalogue rules before success.

## Supply an exact-read callback

`UmiIsoImageInspect` receives a callback instead of opening a path itself. That keeps parsing separate from file permissions, device access and source ownership. The callback must copy every requested byte or return an error.

A memory-backed callback should check the remaining extent before copying:

~~~c
if (offset > image->bytes ||
    (uint64_t)length > image->bytes - offset) {
    return UMI_STATUS_IO_ERROR;
}
memcpy(destination, image->data + (size_t)offset, length);
return UMI_STATUS_OK;
~~~

This example assumes the backing allocation and its size are representable by `size_t`. A file callback instead uses checked offsets and exact reads from its pinned handle. Neither callback may return success for a short read. The inspector cannot reconstruct bytes the callback failed to provide.

The prefix probe preserves an error returned by the callback. It does not turn an unreadable prefix into a negative format result. Source bytes and callback context must remain stable throughout this synchronous operation.

## Publish one complete observation

The public inspector stages its flags in a local candidate. It replaces the caller's result only on success. A recognised truncation or read error therefore leaves an earlier result untouched.

Callers must inspect the returned status before interpreting retained flags:

~~~c
UmiIsoImageInfo candidate = {0};
UmiStatus status = UmiIsoImageInspect(read_at, context, source_bytes, &candidate);
if (status != UMI_STATUS_OK) {
    /* Explain this refusal; do not present a previous observation as current. */
} else if (!candidate.iso9660) {
    /* No ISO format was recognised within this supported profile. */
} else {
    /* Structural observation only: no boot code has executed. */
}
~~~

A successful structural observation does not validate every filesystem record, trust a publisher or establish firmware compatibility. The boot-media service separately owns review, destination policy, transfer and read-back evidence.

## Owner qualification

After integrating and configuring Framework, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-iso-image-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]image_inspection[.]iso[.]" --output-on-failure
~~~

The expected suite now contains 53 cases, including 11 prefix boundaries and callback-error checks. The fake reader independently refuses reads outside the advertised extent. Refusal cases compare the unchanged output, and the new cases check that at most one prefix/first-sector read was requested.

These cases are authored and statically reviewed. Compiler, runtime, produced-image compatibility and firmware boot checks remain unexecuted until owner qualification. The tests use memory fixtures and do not acquire a disk.
