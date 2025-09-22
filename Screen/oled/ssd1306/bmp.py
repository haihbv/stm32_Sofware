import sys, os
import struct

# Lấy đường dẫn file BMP từ tham số dòng lệnh
file_path = sys.argv[1]                     

# Tách tên file và phần mở rộng
file_name, ext = os.path.splitext(file_path)

# Lấy tên file không kèm đường dẫn (vd: walkmen_0)
base_name = os.path.basename(file_name)

# Mở file BMP để đọc dữ liệu nhị phân
with open(file_path, 'rb') as bmp:
    # Đọc offset (vị trí dữ liệu pixel trong file)
    bmp.seek(10, 0)
    offset = struct.unpack('<I', bmp.read(4))[0]
    print("Offset:", offset)

    # Đọc chiều rộng và chiều cao ảnh (4 byte mỗi giá trị, unsigned int)
    bmp.seek(18, 0)
    bmp_width = struct.unpack('<I', bmp.read(4))[0]
    bmp_height = struct.unpack('<I', bmp.read(4))[0]
    print("Width x Height:", bmp_width, "x", bmp_height)

    # Đọc số bit trên mỗi pixel (2 byte, unsigned short)
    bmp.seek(28, 0)
    bits_per_pixel = struct.unpack('<H', bmp.read(2))[0]
    print("Bits per pixel:", bits_per_pixel)

    # Đọc kích thước dữ liệu pixel (không tính header)
    bmp.seek(34, 0)
    bmp_size = struct.unpack('<I', bmp.read(4))[0]
    print("Image size:", bmp_size)

    # Tính row_size (số byte mỗi hàng, bao gồm padding)
    row_size = ((bits_per_pixel * bmp_width + 31) // 32) * 4
    print("Row size:", row_size)

    # Nhảy đến vị trí dữ liệu pixel
    bmp.seek(offset, 0)

    bmp_line = ''       # lưu chuỗi bit của 1 dòng
    bmp_list = []       # dữ liệu nhị phân gốc của ảnh
    bmp_list_v = []     # dữ liệu để in ASCII ra console

    # Đọc từng dòng pixel
    for line in range(bmp_height):
        for _ in range(row_size):
            bmp_byte = bmp.read(1)
            # Đọc 1 byte, đảo màu (255 - giá trị) rồi convert sang chuỗi nhị phân
            bmp_line += format(255 - struct.unpack('B', bmp_byte)[0], "08b")

        # Cắt dữ liệu theo đúng chiều rộng ảnh
        bmp_list.append(bmp_line[:bmp_width])
        bmp_list_v.append(bmp_line[:bmp_width].replace("0", " "))  # đổi 0 -> space để in
        bmp_line = ''

    # Ảnh BMP lưu từ dưới lên → cần đảo ngược
    bmp_list_v.reverse()

    # In ra ảnh ASCII (preview ảnh dạng text)
    for line in bmp_list_v:
        print(line)

# ================================================
# Chuyển dữ liệu sang dạng SSD1306 (8 pixel dọc = 1 byte)
# ================================================

byte_word = ""       # lưu 8 bit (1 byte) theo cột
bitmap_line = []     # lưu 1 dòng bitmap (theo chiều ngang)
bitmap_array = []    # mảng bitmap hoàn chỉnh

# Quét ảnh theo block 8 pixel theo chiều dọc
for line in range(0, bmp_height, 8):
    for bit_num in range(bmp_width):
        for bit in range(line, line + 8):
            if bit > bmp_height - 1:
                byte_word += "0"
            else:
                byte_word += bmp_list[bit][bit_num]
        bitmap_line.append(hex(int(byte_word, 2)))
        byte_word = ''
    bitmap_array.append(bitmap_line)
    bitmap_line = []

# Đảo ngược để hiển thị đúng chiều trên OLED
bitmap_array.reverse()

# ================================================
# Xuất kết quả ra file .txt (mảng C cho SSD1306)
# ================================================
with open(f'{file_name}.txt', 'w') as text_file:
    text_file.write(
        f'static const uint8_t {base_name}_{bmp_width}x{bmp_height}[] = \n'
    )
    text_file.write('{\n')

    for l_cnt, lines in enumerate(bitmap_array):
        text_file.write('    ')  # indent 4 spaces
        for cnt, hexa in enumerate(lines):
            text_file.write(f'{hexa}')
            if cnt < len(lines) - 1:
                text_file.write(',')
        if l_cnt < len(bitmap_array) - 1:
            text_file.write(',\n')
        else:
            text_file.write('\n')

    text_file.write('};\n')
