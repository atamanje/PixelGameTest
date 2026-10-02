import os
import sys
from PIL import Image

def process_sprite_frame(image_path, target_size=(32, 32), transparent_color=None):
    """
    Loads an image, resizes it using Nearest Neighbor resampling to preserve crisp pixels,
    and returns a transparent RGBA image of target_size.
    """
    img = Image.open(image_path).convert("RGBA")
    
    # Nearest neighbor resize to preserve pixel art clarity
    img_resized = img.resize(target_size, Image.Resampling.NEAREST)
    
    if transparent_color:
        # Convert specific background color to transparent if needed
        datas = img_resized.getdata()
        new_data = []
        for item in datas:
            if item[:3] == transparent_color:
                new_data.append((0, 0, 0, 0))
            else:
                new_data.append(item)
        img_resized.putdata(new_data)
        
    return img_resized

def create_spritesheet(frame_paths, output_path, frame_size=(32, 32)):
    """
    Combines a list of frame image paths into a single horizontal spritesheet.
    """
    num_frames = len(frame_paths)
    sheet_width = frame_size[0] * num_frames
    sheet_height = frame_size[1]
    
    spritesheet = Image.new("RGBA", (sheet_width, sheet_height), (0, 0, 0, 0))
    
    for idx, path in enumerate(frame_paths):
        frame = process_sprite_frame(path, target_size=frame_size)
        spritesheet.paste(frame, (idx * frame_size[0], 0), mask=frame)
        
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    spritesheet.save(output_path, "PNG")
    print(f"Saved spritesheet ({sheet_width}x{sheet_height}) to {output_path}")

if __name__ == "__main__":
    print("Pixel Art Processor initialized.")
