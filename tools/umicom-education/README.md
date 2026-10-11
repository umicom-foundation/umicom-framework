# Umicom Education — native C23 learning-site tools

`umicom-education` is a reusable Framework-owned C23 command for checking,
preparing, rendering and previewing the Bits2Banking educational website. It is
built separately while the new native platform contracts are qualified. The
existing `umicom` CLI and its dispatcher are not changed in this delivery.

## Building on Windows (MSYS2 UCRT64)

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\tools\umicom-education" -B ".\build\umicom-education" -G Ninja -DCMAKE_C_COMPILER="C:\msys64\ucrt64\bin\gcc.exe" -DBUILD_TESTING=ON
cmake --build ".\build\umicom-education" --parallel 2
ctest --test-dir ".\build\umicom-education" --output-on-failure
```

## Building on Linux

```bash
cd "$HOME/umicom/Umicom-Applications"
cmake -S framework/tools/umicom-education -B build/umicom-education -G Ninja -DBUILD_TESTING=ON
cmake --build build/umicom-education --parallel 2
ctest --test-dir build/umicom-education --output-on-failure
```

## The commands

| Command | Behaviour | Dependencies |
| --- | --- | --- |
| `doctor --repo PATH` | Check original manuscript source, MkDocs config and official brand source. | Native C standard library and host OS. |
| `prepare --repo PATH [--brand PATH] [--refresh]` | Publish existing ten Islam chapters, thirteen illustrations, seven official brand files. Different published files remain untouched without explicit `--refresh`. | Native C standard library and host OS. |
| `verify --repo PATH` | Verify that original and copied files and branding exist. | Native C standard library and host OS. |
| `build --repo PATH` | Invoke the existing MkDocs builder as a child and relay its status. | **Python + MkDocs still required.** |
| `serve --repo PATH --port N` | Preview the generated site on `127.0.0.1`, with streamed GET/HEAD responses. Stop with Ctrl+C. | Native C standard library, WinSock on Windows or POSIX sockets on Linux. |

From `C:\umicom\Umicom-Applications`, after successful building:

```powershell
$Education = ".\build\umicom-education\bin\umicom-education.exe"
$Books = "C:\umicom\Umicom-Bits2Banking"
& $Education doctor --repo $Books
& $Education prepare --repo $Books
& $Education verify --repo $Books
& $Education build --repo $Books
& $Education serve --repo $Books --port 8765
```

The `--repo` argument identifies the existing Bits2Banking checkout. The
publication owner writes to its approved `content` subdirectories and never
modifies original `dropzone` manuscripts. Changes to an already-published copy
must be compared before choosing `--refresh`.

## Architecture and boundaries

`include/umicom/education/site.h` declares the public C interface.
`src/education/site_content.c` owns safe publication of existing material.
`src/education/site_renderer.c` launches the current MkDocs renderer without a
shell command string. `src/education/site_preview.c` implements the local-only
HTTP preview and streams site assets in bounded chunks.

The Framework's existing `Umicom::web` and `UmiWebStaticFiles` provide
additional HTTP/WebSocket/service contracts. Their current static-file helper
intentionally bounds each complete response to under 16 KiB. This education
preview has a separate streaming implementation because MkDocs' generated
JavaScript, diagrams and books can exceed that limit. The long-term
integration belongs in the canonical Framework web service, not in Bits2Banking
or an application-local copy. Do not publish this development-only listener on
an external interface.

### Security and portability

The preview binds to IPv4 loopback, accepts read-only GET/HEAD and rejects
unrecognised Host/Origin values, dot-prefixed paths, encoded separators,
filesystem traversal and directory listings. It follows POSIX directory handles
with `openat`/`O_NOFOLLOW` on Linux. The initial Windows implementation uses
narrow Win32 paths and attribute checks. It is intended for a trusted local
build directory, **not** an untrusted concurrently writable tree or an
Internet-facing service. Unicode Windows paths, detailed request logging,
accessibility preview diagnostics, cancellation and final integration with the
canonical Web Server remain separate milestones.

The current `verify` is a presence check, not full Markdown link validation.
The original Python and PowerShell tools are retained until feature parity is
measured. No current command generates PDF or DOCX natively, installs
third-party dependencies, or deploys to GitHub Pages.
