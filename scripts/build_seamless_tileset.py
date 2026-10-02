import os
from PIL import Image

def create_seamless_tileset():
    grass_path = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\seamless_grass_tile_1790971324459.jpg"
    path_path  = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\seamless_stone_path_1790971352767.jpg"
    water_path = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\seamless_water_tile_1790971373542.jpg"
    wall_path  = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\seamless_wall_tile_1790971394459.jpg"

    def load_tile(path):
        img = Image.open(path).convert("RGBA")
        # Resize to 32x32 nearest neighbor for pixel crispness
        return img.resize((32, 32), Image.Resampling.NEAREST)

    grass_tile = load_tile(grass_path)
    path_tile  = load_tile(path_path)
    water_tile = load_tile(water_path)
    wall_tile  = load_tile(wall_path)

    # Build 3x3 tileset (96x96 pixels)
    # col 0, row 0 -> Grass
    # col 1, row 0 -> Dirt/Path
    # col 2, row 0 -> Water
    # col 0, row 1 -> Path (Cobblestone)
    # col 1, row 1 -> Wall (Stone Brick)
    # col 2, row 1 -> Grass
    # col 0, row 2 -> Water
    # col 1, row 2 -> Path
    # col 2, row 2 -> Wall

    tileset = Image.new("RGBA", (96, 96), (0, 0, 0, 255))

    # Row 0
    tileset.paste(grass_tile, (0 * 32, 0 * 32))
    tileset.paste(path_tile,  (1 * 32, 0 * 32))
    tileset.paste(water_tile, (2 * 32, 0 * 32))

    # Row 1
    tileset.paste(path_tile,  (0 * 32, 1 * 32))
    tileset.paste(wall_tile,  (1 * 32, 1 * 32))
    tileset.paste(grass_tile, (2 * 32, 1 * 32))

    # Row 2
    tileset.paste(water_tile, (0 * 32, 2 * 32))
    tileset.paste(path_tile,  (1 * 32, 2 * 32))
    tileset.paste(wall_tile,  (2 * 32, 2 * 32))

    out_dir = r"c:\Users\Jessie\Documents\GitHub\PixelGameTest\resources\tilesets"
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "world_tiles.png")
    tileset.save(out_path, "PNG")
    print(f"Successfully generated seamless tileset at {out_path} (96x96)")

if __name__ == "__main__":
    create_seamless_tileset()
