import os
from PIL import Image

def process_tileset(tileset_jpg_path, output_path):
    img = Image.open(tileset_jpg_path).convert("RGBA")
    w, h = img.size
    
    cols, rows = 3, 3
    tile_w, tile_h = w / cols, h / rows
    
    # 3x3 tileset, 32x32 each tile -> total 96x96
    out_tileset = Image.new("RGBA", (96, 96), (0, 0, 0, 255))
    
    for r in range(rows):
        for c in range(cols):
            box = (c * tile_w, r * tile_h, (c + 1) * tile_w, (r + 1) * tile_h)
            tile_crop = img.crop(box)
            tile_resized = tile_crop.resize((32, 32), Image.Resampling.NEAREST)
            out_tileset.paste(tile_resized, (c * 32, r * 32))
            
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    out_tileset.save(output_path, "PNG")
    print(f"Saved tileset to {output_path} (96x96)")

def process_elder_npc(elder_jpg_path, output_path):
    img = Image.open(elder_jpg_path).convert("RGBA")
    w, h = img.size
    
    cols, rows = 2, 2
    frame_w, frame_h = w / cols, h / rows
    
    frames = []
    
    for r in range(rows):
        for c in range(cols):
            box = (c * frame_w, r * frame_h, (c + 1) * frame_w, (r + 1) * frame_h)
            cropped = img.crop(box)
            
            # Remove white/near-white background
            datas = cropped.get_flattened_data()
            new_data = []
            for item in datas:
                if item[0] > 230 and item[1] > 230 and item[2] > 230:
                    new_data.append((0, 0, 0, 0))
                else:
                    new_data.append(item)
            cropped.putdata(new_data)
            
            bbox = cropped.getbbox()
            if bbox:
                cropped_char = cropped.crop(bbox)
            else:
                cropped_char = cropped
                
            canvas = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
            cw, ch = cropped_char.size
            scale = min(30.0 / cw, 30.0 / ch)
            new_size = (max(1, int(cw * scale)), max(1, int(ch * scale)))
            
            resized_char = cropped_char.resize(new_size, Image.Resampling.NEAREST)
            offset_x = (32 - new_size[0]) // 2
            offset_y = 32 - new_size[1] - 1
            canvas.paste(resized_char, (offset_x, offset_y), mask=resized_char)
            frames.append(canvas)
            
    # Save Elder Idle strip (128x32)
    elder_sheet = Image.new("RGBA", (128, 32), (0, 0, 0, 0))
    for i, frame in enumerate(frames):
        elder_sheet.paste(frame, (i * 32, 0), mask=frame)
        
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    elder_sheet.save(output_path, "PNG")
    print(f"Saved Elder NPC sprite sheet to {output_path} (128x32)")

if __name__ == "__main__":
    tileset_jpg = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\pixel_tileset_1790970526093.jpg"
    elder_jpg   = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\elder_npc_sheet_1790970545067.jpg"
    
    out_tileset = r"c:\Users\Jessie\Documents\GitHub\PixelGameTest\resources\tilesets\world_tiles.png"
    out_elder   = r"c:\Users\Jessie\Documents\GitHub\PixelGameTest\resources\sprites\elder_idle.png"
    
    process_tileset(tileset_jpg, out_tileset)
    process_elder_npc(elder_jpg, out_elder)
