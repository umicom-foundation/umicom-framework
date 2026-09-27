# Focused risk review — Batch 31

## Reproduced baseline defects

1. The legacy compiler diagnostic parser split at the first colon. On the baseline, `C:/Umicom Notes/main.c:8:5: error: ...` returned parse error and the partial file `C`. The corrected adapter uses the existing canonical parser and retains the full path.
2. The baseline accepted a source line above UINT32_MAX and returned line zero on the tested host. The corrected legacy adapter refuses a canonical position it cannot represent. It does not crop it into a different location.
3. The baseline silently retained 511 bytes of a 600-byte message and returned OK. The corrected adapter returns capacity exceeded and clears its output.
4. The canonical grammar did not recognise CMake Warning (author). It now also accepts the explicit author and deprecation headers. Existing Error/Warning/dev cases remain.
5. The legacy diagnostic collection's item lookup trusted its public count. A corrupted count above the fixed array allowed an outside-item pointer. The corrected lookup refuses invalid counts; insertion additionally checks strings, severity, counters and revision overflow.

The original implementations remain in explained #if 0 blocks. The reproduction binaries compare actual original source with the delivered source; no Windows process was executed.

## New boundary controls

A snapshot copies one coherent history generation under the existing mutex. It owns the results and does not keep a worker, Studio runtime, history index or live producer pointer. Corrupt counts/strings are rejected before publication. All outputs use bounded allocation; injected allocation failures return errors without a partial report. Immutable reviews support concurrent readers when the caller preserves their lifetime and does not mutate input filters.

The command is display-only. Logs are imported as unrecorded evidence, not as successful process results. Report rendering escapes controls and invalid/bidi text. The GTK view uses text, not markup, and never opens a diagnostic path automatically. Filters are literal, case-sensitive and bounded.

## Residual risks and limits

- Full Framework/Studio compilation, GTK execution and Windows integration are not validated here. Six real GTK lifecycle tests are supplied, not counted as passes.
- The old build result does not record source revision, original working directory or output truncation. Those facts remain unknown. History already evicted before capture is also unknown.
- Capture is a bounded synchronous copy (GUI eight records; API sixteen). Large histories/reports and interactive filtering need responsiveness checks on the target. No timing promise is made.
- A successful recorded process does not prove application correctness; diagnostic parsing can be incomplete. Dropped entries and unrecognised lines remain important. Existing canonical parser formats are reused, not claimed exhaustive.
- Fixed C ABI callers must provide valid memory and lengths. A mutex does not protect against destroying history during capture or destroying a review while another thread uses it. Existing thread owners must join workers before destroying their handles and user data.
- Broader legacy collection/query APIs, editor callbacks, file saves, language servers and debugger transports were not audited or refactored in this slice. The code does not claim repository-wide memory-leak elimination.
- Logs and formatted commands may contain personal paths, source text or credentials. Reports remain local; sharing is a separate reviewed action.
- The CLI uses the platform C runtime for opening an explicit path. Windows argument/path encoding, redirected output and clean-machine startup need platform testing.
- Physical-media writes remain disabled. No media, VM, financial, authentication, database or OS boot qualification is advanced by this read-only IDE update.

## Validation interpretation

GCC Release and Clang ASan/UBSan runs exercise actual canonical history, build parser, diagnostics, compiler and threading sources. Concurrency tests join their actual worker. Three allocator-wrapper cases are Linux-specific. The deliberately faulty Notes project is compiled and executed through CTest in a disposable directory; it is restored and rerun. The separate real-compiler test proves parsing an actual compiler failure, not executing Studio. These are focused dependencies, not a complete platform build.
