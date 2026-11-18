![Umicom](../resources/brand/umicom-logo.svg)

# Own a response buffer until delivery finishes

**UmiWebResponseBodyCreate** keeps its copying behavior. Use it when the input is borrowed or the caller must retain a mutable copy. The additional **UmiWebResponseBodyTake** transfers an existing buffer without copying its payload.

1. Initialize the output body pointer to null.
2. Supply the input pointer's address, length, matching release callback and callback context.
3. On success the input pointer becomes null and the response owns the original buffer.
4. On refusal the input pointer stays unchanged, the output is null and the original owner must release its buffer.
5. Destroy the body exactly once after delivery, cancellation or failure. Destruction invokes the supplied release callback once, including for a successfully transferred empty buffer.

Inputs and outputs must be disjoint. Keep callback context alive until destruction. Do not mutate transferred bytes through an old alias. A zero-length null buffer is supported when paired with a release callback that handles null.

The static-file handler reads through the existing rooted-file service and transfers that service's allocation. Its callback uses **UmiRootedFileFree**, retaining the correct allocator boundary. Existing path admission, size limits, media type selection, header construction and connection cleanup remain in force.

## What this changes

The static-file path no longer needs a second payload-sized response allocation after reading a file. Metadata is still allocated. The entire bounded file is still read into memory: this is not streaming file I/O, an operating-system zero-copy send or a process-wide leak fix.

## Check ownership and delivery

~~~powershell
cmake --build build/windows-ucrt64-debug --target umicom-web-response-body-test umicom-web-response-body-take-test --parallel 2
ctest --test-dir build/windows-ucrt64-debug -R "^framework.web.response_body." --output-on-failure --no-tests=error
~~~

Run these from a configured Framework checkout, separately, stopping on errors. The transfer regression checks pointer identity, small metadata allocation, failure ownership and exact release count. Existing tests verify canonical SVG/ICO payloads, empty content, HEAD, bounded files, partial writes, cancellation and failed delivery. They do not open a network listener.

For process-memory comparisons, record input size, concurrency, binary identity, platform, allocator and metric. Allocation evidence for this one function must not be presented as a measured whole-application peak.
