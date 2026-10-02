import os
import math
from PIL import Image

def generate_water_and_coast_tileset():
    # Load base seamless water, grass, and dirt tiles
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
        
        # Create animated wave shimmer glints
        for y in range(32):
            for x in range(32):
                # Sample base color with horizontal wave scroll
                src_x = (x + shift + int(math.sin(y * 0.3 + phase) * 2)) % 32
                r, g, b, a = base_water.getpixel((src_x, y))
                
                # Add sparkling specular glints at wave crests
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

    # 2. Generate 4 Coast / Lapping Shore Frames (Grass Top, Water Bottom with moving foam wave)
    coast_frames = []
    for frame in range(4):
        # Top half grass (y = 0..13), bottom half water (y = 14..31)
        img = Image.new("RGBA", (32, 32), (0, 0, 0, 255))
        
        # Copy top grass
        for y in range(14):
            for x in range(32):
                img.putpixel((x, y), base_grass.getpixel((x, y)))
                
        # Copy water frames for bottom
        water_f = water_frames[frame]
        for y in range(14, 32):
            for x in range(32):
                img.putpixel((x, y), water_f.getpixel((x, y)))
                
        # Draw lapping wave foam line at coast boundary (y = 12..16)
        # Shore line height oscillates with sine wave across 4 frames
        foam_y_offset = int(math.sin((frame / 4.0) * 2.0 * math.pi) * 2.5) # -2 to +2 px
        shore_y = 13 + foam_y_offset
        
        pixels = img.load()
        for x in range(32):
            # Wavy foam edge
            wave_x = int(math.sin(x * 0.4 + frame) * 1.5)
            curr_y = max(10, min(20, shore_y + wave_x))
            
            # Foam color (bright seafoam white/cyan)
            foam_col1 = (240, 250, 255, 255)
            foam_col2 = (180, 230, 250, 220)
            
            if 0 <= curr_y < 32:
                pixels[x, curr_y] = foam_col1
            if 0 <= curr_y + 1 < 32:
                pixels[x, curr_y + 1] = foam_col2

        coast_frames.append(img)

    # Combine into a 4x4 Tileset Sheet (128 x 128 px):
    # Row 0: Open Water Shimmer Frames 0, 1, 2, 3 (col 0..3)
    # Row 1: Coast Lapping Shore Frames 0, 1, 2, 3 (col 0..3)
    # Row 2: Grass, Path, Wall, Dirt
    # Row 3: Decorative variations

    tileset = Image.new("RGBA", (128, 128), (0, 0, 0, 255))

    # Row 0: Open Water Shimmer 0..3
    for i in range(4):
        tileset.paste(water_frames[i], (i * 32, 0 * 32))

    # Row 1: Coast Lapping Wave 0..3
    for i in range(4):
        tileset.paste(coast_frames[i], (i * 32, 1 * 32))

    # Row 2: Grass, Path, Wall, Dirt
    tileset.paste(base_grass, (0 * 32, 2 * 32))
    tileset.paste(base_path,  (1 * 32, 2 * 32))
    tileset.paste(base_wall,  (2 * 32, 2 * 32))
    tileset.paste(base_path,  (3 * 32, 2 * 32))

    # Row 3: Variations
    tileset.paste(base_grass, (0 * 32, 3 * 32))
    tileset.paste(base_path,  (1 * 32, 3 * 32))
    tileset.paste(base_wall,  (2 * 32, 3 * 32))
    tileset.paste(base_wall,  (3 * 32, 3 * 32))

    out_dir = r"c:\Users\Jessie\Documents\GitHub\PixelGameTest\resources\tilesets"
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "world_tiles.png")
    tileset.save(out_path, "PNG")
    print(f"Successfully created 4x4 animated water tileset at {out_path} (128x128)")

if __name__ == "__main__":
    generate_water_and_coast_tileset()
