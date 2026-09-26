#!/usr/bin/env python3
"""Compare actual Framework output against independent integer and file checks.
Umicom Foundation / Sammy Hegab. MIT. Standard library only; no network access.
"""
from __future__ import annotations
import base64
from html.parser import HTMLParser
from pathlib import Path
import random
import subprocess
import sys
import tempfile

class Report(HTMLParser):
    def __init__(self) -> None:
        super().__init__(convert_charrefs=True)
        self.tags: list[str] = []
        self.images: list[str] = []
        self.text: list[str] = []
    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        self.tags.append(tag)
        values = dict(attrs)
        if tag == "img":
            self.images.append(values.get("src") or "")
    def handle_data(self, data: str) -> None:
        self.text.append(data)

def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)

def run(argv: list[str], expected: int = 0, **kwargs: object) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(argv, text=True, capture_output=True, timeout=25, **kwargs)
    require(result.returncode == expected, f"Command failed: {argv}\n{result.stdout}\n{result.stderr}")
    return result

def main() -> None:
    require(len(sys.argv) == 3, "Expected fixture and canonical project directory")
    fixture = str(Path(sys.argv[1]).resolve())
    sources = Path(sys.argv[2]).resolve()
    rng = random.Random(180018)
    pairs = [(0, 0), (1, 0), (0, 1), (1, 1), ((1 << 64)-2, (1 << 64)-1)]
    pairs += [(rng.getrandbits(64), rng.getrandbits(64)) for _ in range(4096)]
    result = run([fixture, "--percent"], input="".join(f"{v} {m}\n" for v,m in pairs))
    values = [int(line) for line in result.stdout.splitlines()]
    expected = [0 if m == 0 else min(100, v*100//m) for v,m in pairs]
    require(values == expected, "Percentage output differs from arbitrary-precision Python arithmetic")
    compared = 0
    with tempfile.TemporaryDirectory(prefix="umicom-education-") as temporary:
        root = Path(temporary)
        for project in ("notes", "assembly", "framework"):
            destination = root / project
            run([fixture, "--export", project, str(destination)])
            actual = sorted(p.name for p in destination.iterdir())
            canonical = sorted(p.name for p in (sources/project).iterdir() if p.is_file())
            require(actual == canonical, f"Unexpected exported file list: {project}")
            for name in canonical:
                require((destination/name).read_bytes() == (sources/project/name).read_bytes(), f"Altered project file: {project}/{name}")
                compared += 1
            before = {p.name:p.read_bytes() for p in destination.iterdir()}
            refused = run([fixture, "--export", project, str(destination)], expected=1)
            require(refused.stdout.strip() == "5 0", "Existing directory was not refused before writes")
            require(before == {p.name:p.read_bytes() for p in destination.iterdir()}, "Existing files changed")
        empty = root / "existing-empty"
        empty.mkdir()
        run([fixture,"--export","notes",str(empty)], expected=1)
        require(not list(empty.iterdir()), "Existing empty directory was changed")
        unknown = root / "unknown"
        run([fixture,"--export","missing",str(unknown)], expected=1)
        require(not unknown.exists(), "Unknown project created a destination")
        report = root / "learning.html"
        run([fixture,"--report",str(report)])
        html = report.read_text(encoding="utf-8")
        parsed = Report()
        parsed.feed(html)
        require("script" not in parsed.tags, "Unescaped note created an executable tag")
        require("A & <B>" in "".join(parsed.text), "Display name lost its literal characters")
        require('<script>alert(1)</script> & a "quote"' in "".join(parsed.text), "Note text changed")
        require(len(parsed.images) >= 1, "Umicom branding missing")
        for uri in parsed.images:
            require(uri.startswith("data:image/png;base64,"), "Unexpected external image request")
            require(base64.b64decode(uri.split(",",1)[1],validate=True).startswith(b"\x89PNG\r\n\x1a\n"), "Invalid embedded brand image")
        require("does not claim" in html and "compiled or tested" in html, "Missing self-study scope")
    print(f"{len(pairs)} exact percentage comparisons; {compared} byte-identical project files; exclusive export and escaped report checks passed.")

if __name__ == "__main__":
    main()
