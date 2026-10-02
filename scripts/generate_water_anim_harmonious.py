import os
import math
from PIL import Image

def generate_harmonious_water_tileset():
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

    # Unified wave motion function
    def wave_offset(frame):
        return math.sin((frame / 4.0) * 2.0 * math.pi) * 2.2

    def wave_ripple(pos, frame):
        return math.sin(pos * 0.35 + frame * 1.5) * 1.2

    foam_col1 = (240, 250, 255, 255)
    foam_col2 = (170, 225, 245, 230)

    # Helper for rendering coast frame with unified boundary math
    def make_coast_tile(frame, direction):
        img = Image.new("RGBA", (32, 32), (0, 0, 0, 255))
        water_f = water_frames[frame]
        pixels = img.load()

        offset = wave_offset(frame)

        for y in range(32):
            for x in range(32):
                dist_land = 0.0

                if direction == 'N':
                    shore_y = 13.5 + offset + wave_ripple(x, frame)
                    dist_land = shore_y - y
                elif direction == 'S':
                    shore_y = 17.5 - offset - wave_ripple(x, frame)
                    dist_land = y - shore_y
                elif direction == 'W':
                    shore_x = 13.5 + offset + wave_ripple(y, frame)
                    dist_land = shore_x - x
                elif direction == 'E':
                    shore_x = 17.5 - offset - wave_ripple(y, frame)
                    dist_land = x - shore_x
                elif direction == 'NW':
                    shore_x = 13.5 + offset + wave_ripple(y, frame)
                    shore_y = 13.5 + offset + wave_ripple(x, frame)
                    dx = max(0.0, shore_x - x)
                    dy = max(0.0, shore_y - y)
                    if dx > 0 or dy > 0:
                        dist_land = math.sqrt(dx*dx + dy*dy)
                    else:
                        dist_land = -math.sqrt((x - shore_x)**2 + (y - shore_y)**2)
                elif direction == 'NE':
                    shore_x = 17.5 - offset - wave_ripple(y, frame)
                    shore_y = 13.5 + offset + wave_ripple(x, frame)
                    dx = max(0.0, x - shore_x)
                    dy = max(0.0, shore_y - y)
                    if dx > 0 or dy > 0:
                        dist_land = math.sqrt(dx*dx + dy*dy)
                    else:
                        dist_land = -math.sqrt((shore_x - x)**2 + (y - shore_y)**2)
                elif direction == 'SW':
                    shore_x = 13.5 + offset + wave_ripple(y, frame)
                    shore_y = 17.5 - offset - wave_ripple(x, frame)
                    dx = max(0.0, shore_x - x)
                    dy = max(0.0, y - shore_y)
                    if dx > 0 or dy > 0:
                        dist_land = math.sqrt(dx*dx + dy*dy)
                    else:
                        dist_land = -math.sqrt((x - shore_x)**2 + (shore_y - y)**2)
                elif direction == 'SE':
                    shore_x = 17.5 - offset - wave_ripple(y, frame)
                    shore_y = 17.5 - offset - wave_ripple(x, frame)
                    dx = max(0.0, x - shore_x)
                    dy = max(0.0, y - shore_y)
                    if dx > 0 or dy > 0:
                        dist_land = math.sqrt(dx*dx + dy*dy)
                    else:
                        dist_land = -math.sqrt((shore_x - x)**2 + (shore_y - y)**2)

                # Color assignment based on unified dist_land threshold
                if dist_land > 0.8:
                    pixels[x, y] = base_grass.getpixel((x, y))
                elif dist_land >= -0.6:
                    pixels[x, y] = foam_col1
                elif dist_land >= -2.0:
                    pixels[x, y] = foam_col2
                else:
                    pixels[x, y] = water_f.getpixel((x, y))

        return img

    coast_n  = [make_coast_tile(f, 'N') for f in range(4)]
    coast_s  = [make_coast_tile(f, 'S') for f in range(4)]
    coast_w  = [make_coast_tile(f, 'W') for f in range(4)]
    coast_e  = [make_coast_tile(f, 'E') for f in range(4)]
    corner_nw = [make_coast_tile(f, 'NW') for f in range(4)]
    corner_ne = [make_coast_tile(f, 'NE') for f in range(4)]
    corner_sw = [make_coast_tile(f, 'SW') for f in range(4)]
    corner_se = [make_coast_tile(f, 'SE') for f in range(4)]

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
    print(f"Successfully generated harmonious 4-sided water tileset at {out_path} (128x320)")

if __name__ == "__main__":
    generate_harmonious_water_tileset()
