import os
import math
from PIL import Image

def generate_all_sides_water_tileset():
    water_base_path = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\seamless_water_tile_1790971373542.jpg"
    grass_base_path = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\seamless_grass_tile_1790971324459.jpg"
    path_base_path  = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\seamless_stone_path_1790971352767.jpg"
    wall_base_path  = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\seamless_wall_tile_1790971394459.jpg"

    def load_32(path):
        return Image.open(path).convert("RGBA").resize((32, 32), Image.Resampling.NEAREST)

    base_water = load_32(water_base_path)
    base_grass = load_32(grass_base_path)
    base_path  = load_32(path_base_path)
    base_wall  = load_32(wall_base_path)

    # 1. Generate 4 Open Water Shimmer Frames
    water_frames = []
    for frame in range(4):
        img = base_water.copy()
        pixels = img.load()
        shift = frame * 8
        phase = (frame / 4.0) * 2.0 * math.pi
        
        for y in range(32):
            for x in range(32):
                src_x = (x + shift + int(math.sin(y * 0.3 + phase) * 2)) % 32
                r, g, b, a = base_water.getpixel((src_x, y))
                
                wave_val = math.sin((x * 0.4 + y * 0.3) + phase) + math.cos((x * 0.2 - y * 0.5) - phase)
                if wave_val > 1.4:
                    r = min(255, r + 90)
                    g = min(255, g + 110)
                    b = min(255, b + 120)
                elif wave_val > 1.1:
                    r = min(255, r + 40)
                    g = min(255, g + 60)
                    b = min(255, b + 70)
                    
                pixels[x, y] = (r, g, b, a)
        water_frames.append(img)

    # Helper for directional coast frame (N, S, W, E)
    def make_coast_frame(frame, direction):
        img = Image.new("RGBA", (32, 32), (0, 0, 0, 255))
        water_f = water_frames[frame]
        
        foam_y_offset = int(math.sin((frame / 4.0) * 2.0 * math.pi) * 2.5)
        
        foam_col1 = (240, 250, 255, 255)
        foam_col2 = (180, 230, 250, 220)

        pixels = img.load()

        if direction == 'N': # Grass top (y < 14), Water bottom
            for y in range(32):
                for x in range(32):
                    if y < 14: pixels[x, y] = base_grass.getpixel((x, y))
                    else: pixels[x, y] = water_f.getpixel((x, y))
            shore_y = 13 + foam_y_offset
            for x in range(32):
                wave_x = int(math.sin(x * 0.4 + frame) * 1.5)
                curr_y = max(10, min(20, shore_y + wave_x))
                if 0 <= curr_y < 32: pixels[x, curr_y] = foam_col1
                if 0 <= curr_y + 1 < 32: pixels[x, curr_y + 1] = foam_col2

        elif direction == 'S': # Water top, Grass bottom (y > 18)
            for y in range(32):
                for x in range(32):
                    if y > 18: pixels[x, y] = base_grass.getpixel((x, y))
                    else: pixels[x, y] = water_f.getpixel((x, y))
            shore_y = 18 - foam_y_offset
            for x in range(32):
                wave_x = int(math.sin(x * 0.4 + frame) * 1.5)
                curr_y = max(12, min(22, shore_y + wave_x))
                if 0 <= curr_y < 32: pixels[x, curr_y] = foam_col1
                if 0 <= curr_y - 1 < 32: pixels[x, curr_y - 1] = foam_col2

        elif direction == 'W': # Grass left (x < 14), Water right
            for y in range(32):
                for x in range(32):
                    if x < 14: pixels[x, y] = base_grass.getpixel((x, y))
                    else: pixels[x, y] = water_f.getpixel((x, y))
            shore_x = 13 + foam_y_offset
            for y in range(32):
                wave_y = int(math.sin(y * 0.4 + frame) * 1.5)
                curr_x = max(10, min(20, shore_x + wave_y))
                if 0 <= curr_x < 32: pixels[curr_x, y] = foam_col1
                if 0 <= curr_x + 1 < 32: pixels[curr_x + 1, y] = foam_col2

        elif direction == 'E': # Water left, Grass right (x > 18)
            for y in range(32):
                for x in range(32):
                    if x > 18: pixels[x, y] = base_grass.getpixel((x, y))
                    else: pixels[x, y] = water_f.getpixel((x, y))
            shore_x = 18 - foam_y_offset
            for y in range(32):
                wave_y = int(math.sin(y * 0.4 + frame) * 1.5)
                curr_x = max(12, min(22, shore_x + wave_y))
                if 0 <= curr_x < 32: pixels[curr_x, y] = foam_col1
                if 0 <= curr_x - 1 < 32: pixels[curr_x - 1, y] = foam_col2

        return img

    # Helper for corner coast wave animation (NW, NE, SW, SE)
    def make_corner_frame(frame, corner):
        img = Image.new("RGBA", (32, 32), (0, 0, 0, 255))
        water_f = water_frames[frame]
        pixels = img.load()

        foam_offset = math.sin((frame / 4.0) * 2.0 * math.pi) * 2.5
        foam_col1 = (240, 250, 255, 255)
        foam_col2 = (180, 230, 250, 220)

        for y in range(32):
            for x in range(32):
                dx, dy = 0.0, 0.0
                if corner == 'NW':
                    dx = max(0, 13 - x)
                    dy = max(0, 13 - y)
                elif corner == 'NE':
                    dx = max(0, x - 18)
                    dy = max(0, 13 - y)
                elif corner == 'SW':
                    dx = max(0, 13 - x)
                    dy = max(0, y - 18)
                elif corner == 'SE':
                    dx = max(0, x - 18)
                    dy = max(0, y - 18)

                dist = math.sqrt(dx * dx + dy * dy)
                r_wave = 1.0 + foam_offset

                if dist > r_wave + 1.5:
                    pixels[x, y] = base_grass.getpixel((x, y))
                elif dist > r_wave - 0.8:
                    pixels[x, y] = foam_col1
                elif dist > r_wave - 2.2:
                    pixels[x, y] = foam_col2
                else:
                    pixels[x, y] = water_f.getpixel((x, y))

        return img

    coast_n = [make_coast_frame(f, 'N') for f in range(4)]
    coast_s = [make_coast_frame(f, 'S') for f in range(4)]
    coast_w = [make_coast_frame(f, 'W') for f in range(4)]
    coast_e = [make_coast_frame(f, 'E') for f in range(4)]

    corner_nw = [make_corner_frame(f, 'NW') for f in range(4)]
    corner_ne = [make_corner_frame(f, 'NE') for f in range(4)]
    corner_sw = [make_corner_frame(f, 'SW') for f in range(4)]
    corner_se = [make_corner_frame(f, 'SE') for f in range(4)]

    # 4 cols x 10 rows tileset sheet (128 x 320 px)
    sheet = Image.new("RGBA", (128, 320), (0, 0, 0, 255))

    # Row 0: Water Open Shimmer (0..3)
    for i in range(4): sheet.paste(water_frames[i], (i * 32, 0 * 32))

    # Row 1: Coast North (0..3)
    for i in range(4): sheet.paste(coast_n[i], (i * 32, 1 * 32))

    # Row 2: Coast South (0..3)
    for i in range(4): sheet.paste(coast_s[i], (i * 32, 2 * 32))

    # Row 3: Coast West (0..3)
    for i in range(4): sheet.paste(coast_w[i], (i * 32, 3 * 32))

    # Row 4: Coast East (0..3)
    for i in range(4): sheet.paste(coast_e[i], (i * 32, 4 * 32))

    # Row 5: Corner NW (0..3)
    for i in range(4): sheet.paste(corner_nw[i], (i * 32, 5 * 32))

    # Row 6: Corner NE (0..3)
    for i in range(4): sheet.paste(corner_ne[i], (i * 32, 6 * 32))

    # Row 7: Corner SW (0..3)
    for i in range(4): sheet.paste(corner_sw[i], (i * 32, 7 * 32))

    # Row 8: Corner SE (0..3)
    for i in range(4): sheet.paste(corner_se[i], (i * 32, 8 * 32))

    # Row 9: Grass(0,9), Path(1,9), Wall(2,9), Dirt(3,9)
    sheet.paste(base_grass, (0 * 32, 9 * 32))
    sheet.paste(base_path,  (1 * 32, 9 * 32))
    sheet.paste(base_wall,  (2 * 32, 9 * 32))
    sheet.paste(base_path,  (3 * 32, 9 * 32))

    out_dir = r"c:\Users\Jessie\Documents\GitHub\PixelGameTest\resources\tilesets"
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "world_tiles.png")
    sheet.save(out_path, "PNG")
    print(f"Successfully generated corrected corner & 4-sided water tileset at {out_path} (128x320)")

if __name__ == "__main__":
    generate_all_sides_water_tileset()
