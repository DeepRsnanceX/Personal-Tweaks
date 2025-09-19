#!/usr/bin/env python3
"""
Bitmap font split/merge/port tool
  - split: save individual glyph PNGs + meta
  - merge: combine glyph PNGs into atlas + .fnt, with offsets, xadvance = 0
  - port: for all .fnt/.png pairs in a folder, create lower-quality version (half-size)
Usage:
  python split.py split template.fnt template.png separate
  python split.py merge separate out.fnt out.png [--padding N] [--maxwidth N]
  python split.py port portme_folder out_folder [--suffix SUF]
"""
import os, json, argparse
from PIL import Image
import shutil

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
            # ignore kerning
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
            # zero-sized glyph
            glyph_meta.append({'id': cid, 'width': 0, 'height': 0, 'file': None})
            continue

        max_w, max_h = img.width, img.height
        if x < 0: x = 0
        if y < 0: y = 0
        if x + w > max_w: w = max_w - x
        if y + h > max_h: h = max_h - y
        if w <= 0 or h <= 0:
            glyph_meta.append({'id': cid, 'width': 0, 'height': 0, 'file': None})
            continue

        glyph = img.crop((x, y, x + w, y + h))
        fname = f"char_{cid}.png"
        outpath = os.path.join(out_sep_folder, fname)
        glyph.save(outpath)
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
        raise RuntimeError(f"font_meta.json not found in sep_folder: {sep_folder}")

    with open(meta_path, 'r', encoding='utf-8') as f:
        meta = json.load(f)

    chars = meta.get('chars', [])
    glyphs_meta = {g['id']: g for g in meta.get('glyphs', [])}
    info = meta.get('info', {})
    common = meta.get('common', {})

    glyph_map = {}
    for ch in chars:
        cid = ch['id']
        ginfo = glyphs_meta.get(cid, {})
        fname = ginfo.get('file') or f"char_{cid}.png"
        path = os.path.join(sep_folder, fname)
        if os.path.exists(path):
            im = Image.open(path).convert("RGBA")
            w, h = im.width, im.height
            glyph_map[cid] = {'file': fname, 'width': w, 'height': h}
        else:
            glyph_map[cid] = {'file': None, 'width': 0, 'height': 0}

    x_cur = 0
    y_cur = 0
    row_h = 0
    positions = {}
    for ch in chars:
        cid = ch['id']
        g = glyph_map[cid]
        w, h = g['width'], g['height']
        if w <= 0 or h <= 0:
            positions[cid] = (0, 0)
            continue
        step = w + padding
        if x_cur + step > max_texture_width and x_cur > 0:
            x_cur = 0
            y_cur += row_h
            row_h = 0
        positions[cid] = (x_cur, y_cur)
        x_cur += step
        if h > row_h: row_h = h

    final_w = max(1, x_cur) if x_cur <= max_texture_width else max_texture_width
    final_h = max(1, y_cur + row_h)

    atlas = Image.new('RGBA', (final_w, final_h), (0,0,0,0))
    for ch in chars:
        cid = ch['id']
        g = glyph_map[cid]
        fname = g.get('file')
        w, h = g.get('width', 0), g.get('height', 0)
        if not fname or w == 0 or h == 0:
            continue
        path = os.path.join(sep_folder, fname)
        if not os.path.exists(path):
            print(f"Warning: missing glyph file {path} for char id {cid}")
            continue
        im = Image.open(path).convert("RGBA")
        x, y = positions[cid]
        atlas.paste(im, (x, y))

    atlas.save(out_png)
    print(f"Saved merged PNG: {out_png}")

    with open(out_fnt, 'w', encoding='utf-8') as f:
        # info
        if info:
            info_parts = [f'{k}="{v}"' for k, v in info.items()]
        else:
            info_parts = ['font="Generated"', 'size=0']
        f.write("info " + " ".join(info_parts) + "\n")

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
            w, h = g.get('width', 0), g.get('height', 0)
            # force offsets and advance = 0
            xoffset = 0
            yoffset = 0
            xadvance = 0
            page = ch.get('page', 0)
            chnl = ch.get('chnl', 0)
            f.write(f"char id={cid} x={x} y={y} width={w} height={h} ")
            f.write(f"xoffset={xoffset} yoffset={yoffset} xadvance={xadvance} page={page} chnl={chnl}\n")

    print(f"Saved merged FNT: {out_fnt}")

def port_folder(portme_folder, out_folder, suffix="", padding=0, max_texture_width=2048):
    """
    For each .fnt + its matching .png in portme_folder,
    make a lower-quality version (half size) in out_folder with suffix.
    """
    os.makedirs(out_folder, exist_ok=True)
    for fname in os.listdir(portme_folder):
        if not fname.lower().endswith(".fnt"):
            continue
        base = fname[:-4]
        fnt_path = os.path.join(portme_folder, fname)
        # find png
        possible_png = None
        for ext in (".png", ".PNG"):
            cand = base + ext
            cand_path = os.path.join(portme_folder, cand)
            if os.path.exists(cand_path):
                possible_png = cand_path
                break
        if not possible_png:
            print(f"Skipping {fname}: no matching .png found.")
            continue

        # Prepare temporary sep folder
        temp_sep = os.path.join(out_folder, base + suffix + "_sep")
        if os.path.exists(temp_sep):
            shutil.rmtree(temp_sep)
        os.makedirs(temp_sep, exist_ok=True)

        # Split
        split_font(fnt_path, possible_png, temp_sep)

        # Resize glyphs to half size (nearest neighbor)
        for g in os.listdir(temp_sep):
            if g.lower().endswith(".png") and g.startswith("char_"):
                p = os.path.join(temp_sep, g)
                im = Image.open(p).convert("RGBA")
                new_w = max(1, im.width // 2)
                new_h = max(1, im.height // 2)
                im2 = im.resize((new_w, new_h), Image.NEAREST)
                im2.save(p)

        # Merge back
        out_png = os.path.join(out_folder, base + suffix + ".png")
        out_fnt = os.path.join(out_folder, base + suffix + ".fnt")
        merge_font(temp_sep, out_fnt, out_png, padding=padding, max_texture_width=max_texture_width)

        # Clean up sep folder if you want
        shutil.rmtree(temp_sep)
        print(f"Ported {base} → {base + suffix}")

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
    p_merge.add_argument('--padding', type=int, default=0)
    p_merge.add_argument('--maxwidth', type=int, default=2048)

    p_port = sub.add_parser('port')
    p_port.add_argument('portme_folder')
    p_port.add_argument('out_folder')
    p_port.add_argument('--suffix', default='')
    p_port.add_argument('--padding', type=int, default=0)
    p_port.add_argument('--maxwidth', type=int, default=2048)

    args = parser.parse_args()

    if args.cmd == 'split':
        split_font(args.fnt, args.png, args.out_sep_folder)
    elif args.cmd == 'merge':
        merge_font(args.sep_folder, args.out_fnt, args.out_png, padding=args.padding, max_texture_width=args.maxwidth)
    elif args.cmd == 'port':
        port_folder(args.portme_folder, args.out_folder, suffix=args.suffix, padding=args.padding, max_texture_width=args.maxwidth)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
