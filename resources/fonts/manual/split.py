#!/usr/bin/env python3
"""
Bitmap font split/merge tool (updated)
- split: saves individual char PNGs + font_meta.json into <out_sep_folder>
- merge: reads actual PNG sizes from <sep_folder>, places glyphs into atlas,
         writes .png and .fnt with xoffset=0 yoffset=0 xadvance=0
- lower: (kept minimal) - can be added/extended as you like

Usage:
  python split.py split template.fnt template.png separate
  python split.py merge separate out.fnt out.png [--padding N] [--maxwidth N]
"""
import os, json, argparse
from PIL import Image

def parse_fnt(fnt_path):
    chars = []
    common = {}
    info = {}
    pages = []
    with open(fnt_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if line.startswith("info "):
                parts = line[len("info "):].split()
                for p in parts:
                    if '=' in p:
                        k, v = p.split('=', 1)
                        info[k] = v.strip('"')
            elif line.startswith("common "):
                parts = line[len("common "):].split()
                for p in parts:
                    if '=' in p:
                        k, v = p.split('=', 1)
                        common[k] = v.strip('"')
            elif line.startswith("page "):
                parts = line[len("page "):].split()
                page_entry = {}
                for p in parts:
                    if '=' in p:
                        k, v = p.split('=', 1)
                        page_entry[k] = v.strip('"')
                pages.append(page_entry)
            elif line.startswith("char "):
                parts = line[len("char "):].split()
                ch = {}
                for p in parts:
                    if '=' in p:
                        k, v = p.split('=', 1)
                        ch[k] = v.strip('"')
                for fld in ['id','x','y','width','height','xoffset','yoffset','xadvance','page','chnl']:
                    if fld in ch:
                        try:
                            ch[fld] = int(ch[fld])
                        except:
                            ch[fld] = 0
                chars.append(ch)
            # we intentionally ignore kerning for this workflow
    return {'info': info, 'common': common, 'pages': pages, 'chars': chars}

def split_font(fnt_path, png_path, out_sep_folder):
    meta = parse_fnt(fnt_path)
    img = Image.open(png_path).convert("RGBA")
    os.makedirs(out_sep_folder, exist_ok=True)

    glyph_meta = []
    for ch in meta['chars']:
        cid = ch['id']
        x, y, w, h = ch.get('x',0), ch.get('y',0), ch.get('width',0), ch.get('height',0)

        if w <= 0 or h <= 0:
            # e.g. space glyph
            print(f"Skipping char id={cid} (zero size).")
            glyph_meta.append({'id': cid, 'width': 0, 'height': 0, 'file': None})
            continue

        # clamp to image bounds to avoid "tile cannot extend outside image" errors
        max_w, max_h = img.width, img.height
        if x < 0: x = 0
        if y < 0: y = 0
        if x + w > max_w: w = max_w - x
        if y + h > max_h: h = max_h - y
        if w <= 0 or h <= 0:
            print(f"After clamping, skipping char id={cid}")
            glyph_meta.append({'id': cid, 'width': 0, 'height': 0, 'file': None})
            continue

        glyph = img.crop((x, y, x + w, y + h))
        fname = f"char_{cid}.png"
        outpath = os.path.join(out_sep_folder, fname)
        glyph.save(outpath)
        # store meta (initial sizes from original image); merge will re-read actual files
        glyph_meta.append({'id': cid, 'width': w, 'height': h, 'file': fname})

    meta_out = {
        'info': meta.get('info', {}),
        'common': meta.get('common', {}),
        'chars': meta.get('chars', []),
        'glyphs': glyph_meta
    }
    with open(os.path.join(out_sep_folder, "font_meta.json"), 'w', encoding='utf-8') as f:
        json.dump(meta_out, f, indent=2)
    print(f"Split complete → folder: {out_sep_folder}")

def merge_font(sep_folder, out_fnt, out_png, padding=0, max_texture_width=2048):
    meta_path = os.path.join(sep_folder, "font_meta.json")
    if not os.path.exists(meta_path):
        raise RuntimeError("font_meta.json not found in sep_folder: " + sep_folder)

    with open(meta_path, 'r', encoding='utf-8') as f:
        meta = json.load(f)

    chars = meta.get('chars', [])
    glyphs_meta = {g['id']: g for g in meta.get('glyphs', [])}
    info = meta.get('info', {})
    common = meta.get('common', {})

    # Build real glyph map from actual PNGs on disk (this ensures sizes always match file)
    glyph_map = {}
    for ch in chars:
        cid = ch['id']
        # file name from meta if present, else fallback to char_{id}.png
        ginfo = glyphs_meta.get(cid, {})
        fname = ginfo.get('file') or f"char_{cid}.png"
        path = os.path.join(sep_folder, fname)
        if os.path.exists(path):
            im = Image.open(path).convert("RGBA")
            w, h = im.width, im.height
            glyph_map[cid] = {'id': cid, 'file': fname, 'width': w, 'height': h}
        else:
            # no image for this glyph (zero-sized or removed)
            glyph_map[cid] = {'id': cid, 'file': None, 'width': 0, 'height': 0}

    # Layout: place glyphs row-by-row using actual widths/heights
    x_cur = 0
    y_cur = 0
    row_h = 0
    positions = {}
    for ch in chars:
        cid = ch['id']
        w = glyph_map[cid]['width']
        h = glyph_map[cid]['height']
        if w <= 0 or h <= 0:
            positions[cid] = (0, 0)
            continue
        # add padding to width for layout calculations
        step = w + padding
        if x_cur + step > max_texture_width and x_cur > 0:
            # wrap
            x_cur = 0
            y_cur += row_h
            row_h = 0
        positions[cid] = (x_cur, y_cur)
        x_cur += step
        if h > row_h: row_h = h

    final_w = max_texture_width if (x_cur > max_texture_width) else max(1, x_cur)
    final_h = max(1, y_cur + row_h)

    # Create atlas image
    atlas = Image.new('RGBA', (final_w, final_h), (0,0,0,0))

    for ch in chars:
        cid = ch['id']
        g = glyph_map[cid]
        fname = g.get('file')
        w = g.get('width', 0)
        h = g.get('height', 0)
        if not fname or w == 0 or h == 0:
            continue
        path = os.path.join(sep_folder, fname)
        if not os.path.exists(path):
            print(f"Warning: missing glyph file {path} (char id {cid})")
            continue
        im = Image.open(path).convert("RGBA")
        x, y = positions[cid]
        atlas.paste(im, (x, y))

    atlas.save(out_png)
    print(f"Saved merged PNG: {out_png}")

    # Write .fnt with offsets and xadvance forced to 0
    with open(out_fnt, 'w', encoding='utf-8') as f:
        # info
        if info:
            info_parts = [f'{k}="{v}"' for k, v in info.items()]
        else:
            info_parts = ['font="Generated"', 'size=0']
        f.write("info " + " ".join(info_parts) + "\n")

        # common: preserve fields except override scaleW/scaleH
        common_parts = []
        for k, v in common.items():
            if k in ('scaleW', 'scaleH'):
                continue
            common_parts.append(f"{k}={v}")
        common_parts.append(f"scaleW={final_w}")
        common_parts.append(f"scaleH={final_h}")
        if 'pages' not in common:
            common_parts.append("pages=1")
        f.write("common " + " ".join(common_parts) + "\n")

        f.write(f'page id=0 file="{os.path.basename(out_png)}"\n')
        f.write(f"chars count={len(chars)}\n")

        for ch in chars:
            cid = ch['id']
            x, y = positions.get(cid, (0,0))
            g = glyph_map[cid]
            w = g.get('width', 0)
            h = g.get('height', 0)
            # force offsets and advance to 0 per your request
            xoffset = 0
            yoffset = 0
            xadvance = 0
            page = ch.get('page', 0)
            chnl = ch.get('chnl', 0)
            f.write(f"char id={cid} x={x} y={y} width={w} height={h} ")
            f.write(f"xoffset={xoffset} yoffset={yoffset} xadvance={xadvance} page={page} chnl={chnl}\n")

    print(f"Saved merged FNT: {out_fnt}")

def main():
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest='cmd')

    p_split = sub.add_parser('split')
    p_split.add_argument('fnt')
    p_split.add_argument('png')
    p_split.add_argument('out_sep_folder')

    p_merge = sub.add_parser('merge')
    p_merge.add_argument('sep_folder')
    p_merge.add_argument('out_fnt')
    p_merge.add_argument('out_png')
    p_merge.add_argument('--padding', type=int, default=0, help='pixels between glyphs in atlas')
    p_merge.add_argument('--maxwidth', type=int, default=2048, help='max texture width for atlas')

    args = parser.parse_args()

    if args.cmd == 'split':
        split_font(args.fnt, args.png, args.out_sep_folder)
    elif args.cmd == 'merge':
        merge_font(args.sep_folder, args.out_fnt, args.out_png, padding=args.padding, max_texture_width=args.maxwidth)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
