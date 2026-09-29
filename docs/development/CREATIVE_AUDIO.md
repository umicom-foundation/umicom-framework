# Native creative audio contract

Owner: `Umicom::creative_workspace`. Public C23 API: `umicom/creative_workspace/audio.h`; optional GTK page: `umicom/ui/gtk4/creative_audio.h`.

`Decode` validates the complete RIFF structure before allocating a clip and copying its PCM bytes. A fresh NULL output pointer is required; failures do not replace it. The clip is immutable. `GetInfo` and `Sample` copy results. `Inspect` validates a nonempty half-open frame range and copies an independent overview; it performs no allocation. The host owns synchronisation of destruction.

`Render` takes an explicit range and attenuation/fade configuration. It creates new minimal RIFF/WAVE bytes at the same rate and channel count. Output must be zero-initialised. Full-range gain 1000 with zero fades preserves every sample bit but discards ancillary chunks. Gain, fade-in and fade-out are sequential integer operations, each truncating toward zero. Fades can overlap; a one-frame fade is defined to zero its endpoint. No resampling, dither, amplification, compression or conversion to another PCM width is performed.

`LoadFile` is synchronous and uses a regular-file handle, size bound and final-component link/device checks. On POSIX it opens nonblocking before fstat to reject FIFOs without waiting, and compares file metadata before/after reading. This is best-effort change detection, not a hostile-writer snapshot guarantee. Windows permits other readers, not writer sharing, and rejects final reparse points. Parent paths, network mounts and mapped drives are outside that boundary.

`UmiCreativeExportWriteNew` remains the single creative exporter. Its former semantics are unchanged: exclusive new-file creation, no overwrite, possible retained partial file after I/O failure, no power-loss-atomic publication promise. The audio reader and writer share a private path-policy wrapper rather than fork Windows name rules.

The GTK page owns clip/preview independently of the saved project. New buttons use root-bound signal closures. Drawing receives an owned overview copy with a destroy notifier; it does not borrow panel state. Editing parameters drops the preview and disables export. Loading errors preserve the old clip. Data Server is not accessed by this page. GLib allocation follows the toolkit's fatal-allocation policy; native model allocations return checked status values.

Build integration appends `UmicomCreativeAudio.cmake` to the existing creative CMake module. New tests use include with explicit list-file-relative source paths, not late add_subdirectory. No runtime definitions or application sources are replaced.

Limit profile: 4 MiB input/export, 256 input chunks, 512 overview bins, one/two channels, 8000..192000 Hz, PCM tag 1, 16-bit little-endian samples. Maximum duration therefore depends on channels and rate. Empty clips/ranges are refused.

This is not a persistent asset store or a real-time playback engine. A future asset model should persist approved source identity/transform descriptors through Data Server rather than embed opaque mutable pointers in a project.
