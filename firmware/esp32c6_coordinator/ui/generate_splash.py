"""Convert an 8-bit RGB/RGBA PNG to a small RGB565 boot asset, without packages.

The manifest names the replaceable source image. PNG filters and alpha are
decoded here; firmware only reads a const array, with no runtime image decoder.
"""
import json
from pathlib import Path
import struct
import sys
import zlib


def read_png(path):
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("Splash must be a PNG")
    pos, compressed, header = 8, bytearray(), None
    while pos < len(data):
        size = struct.unpack_from(">I", data, pos)[0]
        kind = data[pos+4:pos+8]
        chunk = data[pos+8:pos+8+size]
        crc = struct.unpack_from(">I", data, pos+8+size)[0]
        if zlib.crc32(kind+chunk) & 0xFFFFFFFF != crc:
            raise ValueError("PNG CRC mismatch")
        if kind == b"IHDR":
            header = struct.unpack(">IIBBBBB", chunk)
        if kind == b"IDAT":
            compressed.extend(chunk)
        pos += size+12
        if kind == b"IEND":
            break
    if header is None:
        raise ValueError("PNG header missing")
    w, h, depth, color, compression, filtering, interlace = header
    if depth != 8 or color not in (2, 6) or compression or filtering or interlace:
        raise ValueError("Use a non-interlaced 8-bit RGB/RGBA PNG")
    if not 1 <= w <= 4096 or not 1 <= h <= 4096:
        raise ValueError("Splash image dimensions must be 1..4096")
    channels = 4 if color == 6 else 3
    stride = w*channels
    raw = zlib.decompress(compressed)
    if len(raw) != (stride+1)*h:
        raise ValueError("PNG decompressed size mismatch")
    previous = bytearray(stride)
    pixels = []
    for y in range(h):
        offset = y*(stride+1)
        mode = raw[offset]
        row = bytearray(raw[offset+1:offset+1+stride])
        if mode > 4:
            raise ValueError("Invalid PNG filter")
        for x in range(stride):
            a = row[x-channels] if x >= channels else 0
            b = previous[x]
            c = previous[x-channels] if x >= channels else 0
            if mode == 1:
                predictor = a
            elif mode == 2:
                predictor = b
            elif mode == 3:
                predictor = (a+b)//2
            elif mode == 4:
                p = a+b-c
                distances = (abs(p-a), abs(p-b), abs(p-c))
                predictor = (a, b, c)[distances.index(min(distances))]
            else:
                predictor = 0
            row[x] = (row[x]+predictor) & 255
        pixels.append(row)
        previous = row
    return w, h, channels, pixels


def generate(manifest, output):
    config = json.loads(manifest.read_text())
    source = (manifest.parent / config["image"]).resolve()
    if source.parent != manifest.parent.resolve():
        raise ValueError("Splash image must live beside splash.json")
    size = config["size"]
    if not isinstance(size, int) or not 16 <= size <= 128:
        raise ValueError("Splash size must be 16..128")
    w, h, channels, pixels = read_png(source)
    # Aspect fit, then area-average premultiplied color onto black.
    dw, dh = (size, max(1, h*size//w)) if w >= h else (max(1, w*size//h), size)
    values = []
    for y in range(dh):
        ya, yb = y*h//dh, max(y*h//dh+1, (y+1)*h//dh)
        for x in range(dw):
            xa, xb = x*w//dw, max(x*w//dw+1, (x+1)*w//dw)
            sums = [0, 0, 0]
            for yy in range(ya, yb):
                for xx in range(xa, xb):
                    row, offset = pixels[yy], xx*channels
                    alpha = row[offset+3] if channels == 4 else 255
                    for c in range(3):
                        sums[c] += row[offset+c]*alpha
            r, g, b = (s//((xb-xa)*(yb-ya)*255) for s in sums)
            values.append((r>>3)<<11 | (g>>2)<<5 | (b>>3))
    output.parent.mkdir(parents=True, exist_ok=True)
    lines = ["/* Generated from ui/assets/splash.json; do not edit. */", "#pragma once",
             f"#define UI_SPLASH_WIDTH {dw}", f"#define UI_SPLASH_HEIGHT {dh}",
             "static const uint16_t ui_splash_pixels[] = {"]
    for start in range(0, len(values), 16):
        lines.append(",".join(f"0x{v:04X}" for v in values[start:start+16])+",")
    output.write_text("\n".join(lines)+"\n};\n")


if __name__ == "__main__":
    generate(Path(sys.argv[1]), Path(sys.argv[2]))
