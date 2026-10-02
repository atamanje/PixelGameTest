import os
from PIL import Image

def process_knight_sheet(input_image_path, output_dir):
    img = Image.open(input_image_path).convert("RGBA")
    w, h = img.size
    
    # 4 columns, 2 rows
    cols = 4
    rows = 2
    
    frame_w = w / cols
    frame_h = h / rows
    
    idle_frames = []
    walk_frames = []
    
    os.makedirs(output_dir, exist_ok=True)
    
    def crop_and_clean_frame(box):
        cropped = img.crop(box)
        # Convert white/near-white background to transparent
        datas = cropped.getdata()
        new_data = []
        for item in datas:
            # Check if color is white/near-white (R, G, B > 240)
            if item[0] > 240 and item[1] > 240 and item[2] > 240:
                new_data.append((0, 0, 0, 0))
            else:
                new_data.append(item)
        cropped.putdata(new_data)
        
        # Bounding box of non-transparent area
        bbox = cropped.getbbox()
        if bbox:
            cropped_char = cropped.crop(bbox)
        else:
            cropped_char = cropped
            
        # Create a centered 32x32 sprite frame
        canvas = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
        # Scale character to fit 30x30 while maintaining aspect ratio
        char_w, char_h = cropped_char.size
        scale = min(30.0 / char_w, 30.0 / char_h)
        new_size = (max(1, int(char_w * scale)), max(1, int(char_h * scale)))
        
        resized_char = cropped_char.resize(new_size, Image.Resampling.NEAREST)
        
        # Paste centered
        offset_x = (32 - new_size[0]) // 2
        offset_y = 32 - new_size[1] - 1  # align to bottom
        canvas.paste(resized_char, (offset_x, offset_y), mask=resized_char)
        
        return canvas

    # Row 0: Idle frames
    for col in range(cols):
        box = (col * frame_w, 0 * frame_h, (col + 1) * frame_w, 1 * frame_h)
        idle_frames.append(crop_and_clean_frame(box))
        
    # Row 1: Walk frames
    for col in range(cols):
        box = (col * frame_w, 1 * frame_h, (col + 1) * frame_w, 2 * frame_h)
        walk_frames.append(crop_and_clean_frame(box))
        
    # Save Idle Spritesheet (128 x 32)
    idle_sheet = Image.new("RGBA", (32 * cols, 32), (0, 0, 0, 0))
    for i, frame in enumerate(idle_frames):
        idle_sheet.paste(frame, (i * 32, 0), mask=frame)
    idle_sheet.save(os.path.join(output_dir, "hero_idle.png"), "PNG")
    print(f"Saved {output_dir}/hero_idle.png (128x32)")

    # Save Walk Spritesheet (128 x 32)
    walk_sheet = Image.new("RGBA", (32 * cols, 32), (0, 0, 0, 0))
    for i, frame in enumerate(walk_frames):
        walk_sheet.paste(frame, (i * 32, 0), mask=frame)
    walk_sheet.save(os.path.join(output_dir, "hero_walk.png"), "PNG")
    print(f"Saved {output_dir}/hero_walk.png (128x32)")

if __name__ == "__main__":
    jpg_path = r"C:\Users\Jessie\.gemini\antigravity\brain\a3ec5565-13f9-4aa2-ac2e-cda28526721b\hero_character_sheet_1790970045928.jpg"
    out_dir = r"c:\Users\Jessie\Documents\GitHub\PixelGameTest\resources\sprites"
    process_knight_sheet(jpg_path, out_dir)
