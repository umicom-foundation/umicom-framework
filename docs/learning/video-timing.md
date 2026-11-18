![Umicom](../../resources/brand/umicom-logo-on-dark.svg)

# Select decoded frames by time

Use UmiAiTrainingVideoSelectAtOrBefore from umicom/ai/training/video_timing.h through Umicom::ai when an explicit hold-previous policy is appropriate. Each result is the latest source index whose timestamp is no later than the corresponding query. This is a preparation policy, not a decoder or a learned video generator.

1. Obtain presentation timestamps in display order from your decoder. Decode order and presentation order may differ.
2. Convert them to a documented common unsigned integer unit and origin using checked arithmetic before this call. The service does not convert a rational container time base or negative timestamps.
3. Require strictly increasing source timestamps. Review duplicates or discontinuities explicitly; do not sort times separately from their pixel frames.
4. Supply nondecreasing requested times inside the closed source interval, up to 64 queries over at most 4096 source times. Equal queries are allowed.
5. Check status before using output. Every refusal preserves the entire index buffer. Success writes exactly queryCount size_t elements and leaves its tail unchanged.
6. Use indices to construct borrowed UmiAiTrainingVideoFrame descriptors, then call the existing VideoPackRgb8 service with its own dimension, count and capacity limits.

For source times 0, 40000, 100000, 160000 and queries 0, 50000, 100000, 150000, the output is 0, 1, 2, 2. Units in this example are microseconds. Queries are fifty milliseconds apart, but the source times are not evenly spaced. Equal index steps are therefore a different policy. Multiple queries can select one frame without copying its pixels; tensor packing subsequently writes each selected position.

No heap allocation occurs. Complete source and query metadata is validated before the first output write. A monotonic scan uses comparisons only, avoiding timestamp subtraction, midpoint overflow and floating-point precision loss near UINT64_MAX. Readable inputs and writable outputs must be backed by valid disjoint spans and remain stable for the synchronous call. Validation of metadata cannot make an invalid pointer safe.

Before-first and after-last queries are refused. The service does not infer the final frame duration or extend an endpoint. It also does not interpolate pixels, apply motion compensation, align audio, choose a target frame rate, remove duplicate images or synchronize captions. A held frame can bias training or misrepresent motion; record the policy and compare it with the actual model contract.

The Bits2Banking video-tensor lab contains a complete timing.c consumer. Its source-focused checks do not qualify installed SDK resolution, real codec integration or Media GUI behavior.
