#!/usr/bin/env python3
"""Parse actual C-generated exports independently using the Python standard library.

Umicom Framework — Sammy Hegab, Umicom Foundation — MIT.
This is format/content verification, not a GTK, neural-model or media-device test.
"""
from __future__ import annotations
import argparse
import math
import struct
import subprocess
import tempfile
import wave
import xml.etree.ElementTree as ET
from html.parser import HTMLParser
from pathlib import Path

class Page(HTMLParser):
    def __init__(self) -> None:
        super().__init__(); self.figures = 0; self.images: list[str] = []; self.scripts = 0; self.csp = ""
    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        values = dict(attrs)
        if tag == "figure": self.figures += 1
        if tag == "script": self.scripts += 1
        if tag == "img": self.images.append(values.get("src") or "")
        if tag == "meta" and values.get("http-equiv") == "Content-Security-Policy": self.csp = values.get("content") or ""

def require(condition: bool, message: str) -> None:
    if not condition: raise AssertionError(message)

def check(probe: Path, directory: Path) -> int:
    ns = {"s": "http://www.w3.org/2000/svg"}; checks = 0
    for profile in ("kitchen", "media", "music", "web-studio", "mobile-studio"):
        out = directory / profile; out.mkdir()
        subprocess.run([str(probe), profile, str(out)], check=True, timeout=30)
        document = ET.parse(out / "drawing.svg").getroot()
        require(document.tag == "{" + ns["s"] + "}svg", "SVG namespace")
        require(document.findtext("s:title", namespaces=ns) == "Workshop & café <visual>", "UTF-8/XML title round trip")
        checks += 2
        if profile == "kitchen":
            require(document.attrib["width"] == "3000mm" and document.attrib["height"] == "2400mm", "physical dimensions")
            groups = {g.attrib["data-element"]: g for g in document.findall("s:g", ns)}
            require(groups["cabinet"].find("s:rect", ns).attrib["width"] == "600", "cabinet dimension")
            checks += 2
        if profile == "media":
            group = next(g for g in document.findall("s:g", ns) if g.attrib["data-element"] == "card")
            rect = group.find("s:rect", ns)
            require(float(rect.attrib["x"]) == 290 and float(rect.attrib["y"]) == 230, "independent keyframe midpoint")
            checks += 1
        for name, expected in (("prototype.html", 1), ("storyboard.html", 3)):
            text = (out / name).read_text(encoding="utf-8"); page = Page(); page.feed(text)
            require(page.figures == expected and page.scripts == 0, "inert frame count")
            require("default-src 'none'" in page.csp, "restrictive CSP")
            require(len(page.images) == 1 and page.images[0].startswith("data:image/png;base64,"), "embedded original logo")
            require("&lt;visual&gt;" in text, "HTML escaping")
            checks += 4
    with wave.open(str(directory / "music" / "notes.wav"), "rb") as audio:
        require(audio.getframerate() == 48000 and audio.getnchannels() == 1 and audio.getsampwidth() == 2, "PCM contract")
        require(audio.getnframes() == 8 * 48000, "duration")
        raw = audio.readframes(audio.getnframes())
    samples = struct.unpack("<" + "h" * (len(raw) // 2), raw)
    require(all(v == 0 for v in samples[:24000]), "silence before tick 480")
    require(all(v == 0 for v in samples[72000:]), "silence after tick 1440")
    require(max(abs(v) for v in samples) <= 26215, "headroom")
    section = samples[30000:54000]
    def energy(frequency: float) -> float:
        real = sum(v * math.cos(2 * math.pi * frequency * i / 48000) for i, v in enumerate(section))
        imag = sum(v * math.sin(2 * math.pi * frequency * i / 48000) for i, v in enumerate(section))
        return real * real + imag * imag
    require(energy(440) > 100 * max(energy(415.3047), energy(466.1638)), "A4 pitch independent Fourier check")
    checks += 6
    return checks

def main() -> None:
    parser = argparse.ArgumentParser(); parser.add_argument("probe", type=Path); parser.add_argument("--keep", type=Path)
    args = parser.parse_args(); probe = args.probe.resolve()
    if args.keep:
        args.keep.mkdir(parents=True, exist_ok=False); count = check(probe, args.keep.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix="umicom-creative-export-") as temp: count = check(probe, Path(temp))
    print(f"PASS: {count} independent format/content/audio checks across five profiles.")
if __name__ == "__main__": main()
