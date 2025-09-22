import sys
from PIL import Image

# ==== Cấu hình màn hình ST7735 ====
LCD_WIDTH_PORTRAIT = 128
LCD_HEIGHT_PORTRAIT = 160

LCD_WIDTH_LANDSCAPE = 160
LCD_HEIGHT_LANDSCAPE = 128

# ==== Xử lý tham số dòng lệnh ====
if len(sys.argv) < 2:
    print("Usage: python convert_image.py <image_path> [--landscape]")
    sys.exit(1)

file_path = sys.argv[1]
landscape_mode = False

if len(sys.argv) > 2 and sys.argv[2] == "--landscape":
    landscape_mode = True

file_name = file_path.rsplit('.', 1)[0]
base_name = file_name.split('/')[-1]

# ==== Mở ảnh ====
if landscape_mode:
    LCD_WIDTH = LCD_WIDTH_LANDSCAPE
    LCD_HEIGHT = LCD_HEIGHT_LANDSCAPE
else:
    LCD_WIDTH = LCD_WIDTH_PORTRAIT
    LCD_HEIGHT = LCD_HEIGHT_PORTRAIT

img = Image.open(file_path).convert("RGB").resize((LCD_WIDTH, LCD_HEIGHT))

width, height = img.size
pixels = list(img.getdata())

rgb565_data = []
for y in range(height):
    row = []
    for x in range(width):
        r, g, b = pixels[y * width + x]
        r5 = (r >> 3) & 0x1F
        g6 = (g >> 2) & 0x3F
        b5 = (b >> 3) & 0x1F
        rgb565 = (r5 << 11) | (g6 << 5) | b5
        row.append(rgb565)
    rgb565_data.append(row)

# ==== Xuất ra file .txt ====
with open(f"{file_name}.txt", "w") as f:
    f.write(f"static const uint16_t {base_name}[] = {{\n")
    for row in rgb565_data:
        f.write("    ")
        for px in row:
            f.write(f"0x{px:04X}, ")
        f.write("\n")
    f.write("};\n")

print(width, height)
print(f"Xuất file thành công: {file_name}.txt")
print(f"Ảnh đã được resize về {width}x{height} pixel, định dạng RGB565.")
print(f"Chế độ: {'Landscape (160x128)' if landscape_mode else 'Portrait (128x160)'}")
