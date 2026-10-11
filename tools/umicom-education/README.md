# Umicom Education — C23 learning-site and manuscript tools

`umicom-education` is a Framework-owned C23 command used with the existing
Bits2Banking repository. Native operations prepare approved teaching material,
inspect volume manifests, compile reviewable Markdown manuscripts and serve
an already-built website over IPv4 loopback. The existing `umicom` CLI remains
unchanged; integration into it is a separate roadmap milestone.

## Configure, build and test on Windows

Open PowerShell:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

cmake -S ".\framework\tools\umicom-education" `
      -B ".\build\umicom-education" `
      -G Ninja `
      -DCMAKE_C_COMPILER="C:\msys64\ucrt64\bin\gcc.exe" `
      -DBUILD_TESTING=ON

cmake --build ".\build\umicom-education" --parallel 2
ctest --test-dir ".\build\umicom-education" --output-on-failure
```

An incremental build does not require deleting the existing build directory.
CTest includes tests for bounded HTTP paths, publication, native book assembly,
renderer error handling and CLI help. The renderer tests use a native test double
rather than installing MkDocs or contacting the Internet.
```bash
cd "$HOME/umicom/Umicom-Applications"
cmake -S framework/tools/umicom-education -B build/umicom-education -G Ninja -DBUILD_TESTING=ON
cmake --build build/umicom-education --parallel 2
ctest --test-dir build/umicom-education --output-on-failure
```

## Solve `No module named mkdocs`

The original `build` subcommand selected the first `python` executable on
`PATH`. Under MSYS2 UCRT64 this may be `C:\msys64\ucrt64\bin\python.exe`,
which commonly lacks MkDocs. This is a **website-renderer** failure, not a C
compiler or web-server startup failure.

The updated code selects an interpreter that can import both MkDocs and Material,
preferentially the project's `.venv` Python. It does not install dependencies.

In a separate PowerShell terminal:

```powershell
Set-Location "C:\umicom\Umicom-Bits2Banking"

py -3 -m venv .venv
& ".\.venv\Scripts\python.exe" -m pip install -r ".\requirements-docs.txt"
& ".\.venv\Scripts\python.exe" -m mkdocs --version
```

If the Python launcher is not installed, substitute a known Windows Python
interpreter path for `py -3`. Do not install packages into the MSYS2 toolchain
interpreter unless that is your intended documentation environment.
<!-- The shell example now closes beside its commands. This former closing
fence is retained for source review so it cannot hide the following guide.
```
-->

## Commands

| Command | Behaviour | External runtime |
| --- | --- | --- |
| `doctor --repo PATH` | Check manuscript, branding and MkDocs availability. | Uses the selected interpreter to probe the renderer. |
| `prepare --repo PATH [--brand PATH] [--refresh]` | Prepare copies of the original ten Islam chapters, illustrations and Framework branding; preserve differing published files by default. | None. |
| `verify --repo PATH` | Read-only check of required original and published site inputs. | None. |
| `book list --repo PATH` | Read `dropzone/documents/*/volume.yml` and show volume titles and chapter counts. | None. |
| `book assemble --repo PATH --volume SLUG [--refresh]` | Assemble ordered Markdown into `output/native-books/SLUG.md`, keeping originals untouched. | None. |
| `build --repo PATH [--python PATH]` | Find an environment containing MkDocs and Material, run the existing strict MkDocs build and verify `site/index.html`. | Python + MkDocs + Material. |
| `serve --repo PATH --port N [--once]` | Serve built site through read-only streamed HTTP GET/HEAD on `127.0.0.1`. | None. |

The temporary book output and existing book output are compared. A differing
output is not replaced without `--refresh`. The book assembler parses the
manifest's simple title/chapter-list subset; it is not a general YAML engine.
Compiled Markdown is a review artefact, not a replacement for existing BookForge,
Pandoc, DOCX, PDF, EPUB or site publication tooling.

## Build and preview the website

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$Education = ".\build\umicom-education\bin\umicom-education.exe"
$Books = "C:\umicom\Umicom-Bits2Banking"

& $Education doctor --repo $Books
& $Education prepare --repo $Books
& $Education verify --repo $Books
& $Education build --repo $Books
if ($LASTEXITCODE -ne 0) { throw "The website was not built." }

& $Education serve --repo $Books --port 8765
```

The `--repo` argument identifies the existing Bits2Banking checkout. The
publication owner writes to its approved `content` subdirectories and never
changes the original book sources.

The `--python` option can specify an interpreter even if no `.venv` exists:

```powershell
& $Education build --repo $Books `
    --python "C:\umicom\Umicom-Bits2Banking\.venv\Scripts\python.exe"
```

Open `http://127.0.0.1:8765/`; stop the server with Ctrl+C. If `site/index.html`
is missing, run `build` and fix its first reported error rather than starting the
server. The preview is neither a public web host nor a TLS service.

## Assemble the original book as Markdown

```powershell
& $Education book list --repo $Books
& $Education book assemble --repo $Books --volume islam_blueprint_for_life
```

This creates `output/native-books/islam_blueprint_for_life.md`, leaving the source
chapters and previously generated PDF/DOCX books intact. Chapter navigation links
may need rewriting for a single-file edition: the output is a source manuscript,
not the final formatted release.

## Linux

```bash
cd "$HOME/umicom/Umicom-Applications"
cmake -S framework/tools/umicom-education -B build/umicom-education -G Ninja -DBUILD_TESTING=ON
cmake --build build/umicom-education --parallel 2
ctest --test-dir build/umicom-education --output-on-failure

cd "$HOME/umicom/Umicom-Bits2Banking"
python3 -m venv .venv
./.venv/bin/python -m pip install -r requirements-docs.txt
```

## Architectural boundaries

The Framework service interfaces are declared in
`include/umicom/education/site.h`. The new `src/education/book.c` implements
volume discovery and safe manuscript assembly; `site_renderer.c` selects and
runs the current MkDocs build; `site_content.c` owns asset preparation; and
`site_preview.c` owns the read-only streaming loopback listener.

The existing Framework `Umicom::web` remains authoritative for general HTTP
services. This separate preview is a bounded educational experiment until its
streaming behaviour can be merged safely into the shared web service.
The Windows preview still uses narrow Win32 path APIs and assumes a trusted
local generated tree. It must not be exposed to a public network or used to
serve sensitive files. Unicode path support, a single publication owner and
full native publishing are future changes.

Original documents, scripts, working CMake projects, source comments and
application functionality have not been removed or disabled by this batch.

## Site publication manifest (after MkDocs and BookForge)

The publication integrity service is Framework-owned. It reuses
`src/base/sha256.c` rather than introducing a second SHA-256 implementation.
It works on the combined `site/` directory, including the historical book
archive. It does not run GitHub Actions or publish files to the Internet.

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$Education = ".\build\umicom-education\bin\umicom-education.exe"
$Books = "C:\umicom\Umicom-Bits2Banking"
& $Education publish manifest --repo $Books
& $Education publish verify --repo $Books
```

The digest list is generated at `output/native-publication/site.sha256`. A
changed file refuses verification. A changed manifest cannot be overwritten
without the explicit `publish manifest --refresh` option. Neither operation
changes manuscripts, lessons, video scripts or source files.

`framework.education.publication_manifest` tests a complete manifest, altered
bytes, added files, refused manifest replacement and linked-file rejection
(on platforms that support symbolic links).

The consolidated GitHub Pages publisher is maintained by Bits2Banking under
`.github/workflows/learning-pages.yml`. It builds the former BookForge output
and the MkDocs site independently, then combines them. The older book artifact
jobs remain available; they no longer independently deploy to GitHub Pages.

## Renderer regression: executable and directory paths containing spaces

`framework.education.renderer` tests a native mock Python process. Earlier
Windows UCRT64 runs failed when the test executable lived in a directory with
spaces, even though the unrelated test cases passed. The standalone education
launcher now uses an explicit Windows Unicode executable path and safely quoted
arguments via `CreateProcessW` rather than the previous `_spawnvp` path.

The executable name and the MkDocs `--config-file` and `--site-dir` values each
receive a regression case with spaces. The fake renderer verifies the exact argv
fields and can report success without writing `index.html`; the education
builder must reject that incomplete result. These tests do not install or
execute the real MkDocs package.

To run the focused test from Windows PowerShell:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake --build ".\build\umicom-education" --parallel 2
ctest --test-dir ".\build\umicom-education" `
    -R "^framework\.education\.renderer$" -V
```

For real website acceptance, use the repository-local Python interpreter with
`umicom-education build --repo PATH --python PATH`. The existing Python and
MkDocs installation is not modified by the test. The platform-wide process
API remains `umi_process_execute`; application modules must not copy the
standalone education launch adapter.

## Update 06: build an HTML site before previewing it

A successful `doctor` check means the MkDocs renderer is available. It does not
create `site/index.html`. The `serve` command continues to reject missing
pages rather than incorrectly treating unbuilt Markdown as a website.

To build once and then start the read-only native preview in one explicit step:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$Education = ".\build\umicom-education\bin\umicom-education.exe"
$Books = "C:\umicom\Umicom-Bits2Banking"
$Python = "$Books\.venv\Scripts\python.exe"
& $Education serve --repo $Books --build --python $Python --port 8765
```

The same action can be performed as two commands: `build` followed by `serve`.
`--build` is permitted only for `serve`, and it is incompatible with
`--directory`, which selects an existing export. A failed MkDocs invocation
prevents server startup. Existing publishing and manuscript files remain
untouched; this only generates the normal MkDocs site output.

CTest now includes `framework.education.serve.preflight` to verify that a
missing homepage gives an actionable diagnostic, failed renderer selection
cannot start the socket, and unrelated commands reject `--build`.

## Audit local lesson navigation without Python

After building this tool, run `umicom-education audit --repo C:\umicom\Umicom-Bits2Banking`.

The Framework-owned C23 preflight checks the local Markdown targets in the
`nav:` section of `mkdocs.yml`. It detects missing destinations, duplicate
references and unsafe relative paths without running a Python interpreter or
modifying the repository. It deliberately does not parse full YAML syntax or
render the website. Run the existing MkDocs strict build for full validation.
