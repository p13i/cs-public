// cs/net/http/favicon.cc
#include "cs/net/http/favicon.hh"

#include <cstdint>
#include <vector>

std::vector<unsigned char>
cs::net::http::GenerateWhiteIco() {
  const int width = 16, height = 16, bytesPerPixel = 4;
  const int pixelDataSize =
      width * height * bytesPerPixel;  // 1024 bytes
  const int andMaskSize = (width * height + 7) /
                          8;  // 32 bytes (1 bit per pixel)
  const int bitmapDataSize =
      40 + pixelDataSize + andMaskSize;  // 1096 bytes
  const int icoSize =
      6 + 16 +
      bitmapDataSize;  // 6 + 16 + 1096 = 1118 bytes

  std::vector<unsigned char> ico(icoSize, 0);

  // Helper lambda to write little-endian values
  auto write_le16 = [&ico](int offset, uint16_t value) {
    ico[offset] = value & 0xFF;
    ico[offset + 1] = (value >> 8) & 0xFF;
  };
  auto write_le32 = [&ico](int offset, uint32_t value) {
    ico[offset] = value & 0xFF;
    ico[offset + 1] = (value >> 8) & 0xFF;
    ico[offset + 2] = (value >> 16) & 0xFF;
    ico[offset + 3] = (value >> 24) & 0xFF;
  };

  // ICO Header (6 bytes)
  // Bytes 0-1: Reserved (0)
  // Byte 2: Type (1 = ICO)
  ico[2] = 1;
  // Byte 3: Reserved (0)
  // Bytes 4-5: Number of images (1)
  write_le16(4, 1);

  // Icon directory entry (16 bytes, starting at offset 6)
  ico[6] = width;      // Width (16)
  ico[7] = height;     // Height (16)
  ico[8] = 0;          // Color count (0 = no palette)
  ico[9] = 0;          // Reserved
  write_le16(10, 1);   // Planes (1)
  write_le16(12, 32);  // Bit count (32 = RGBA)
  write_le32(14,
             bitmapDataSize);  // Size of image data (1096)
  write_le32(18, 62);  // Offset to image data (6 + 16 = 22,
                       // but BITMAPINFOHEADER starts at 22,
                       // so offset is 62)

  // BITMAPINFOHEADER (40 bytes, starting at offset 22)
  write_le32(22, 40);     // Header size (40)
  write_le32(26, width);  // Width (16)
  write_le32(
      30,
      height *
          2);  // Height (32 = 16 for color + 16 for mask)
  write_le16(34, 1);   // Planes (1)
  write_le16(36, 32);  // Bit count (32 = RGBA)
  write_le32(38, 0);   // Compression (0 = BI_RGB)
  write_le32(42, pixelDataSize);  // Image size (1024)
  // Bytes 46-49: X pixels per meter (0)
  // Bytes 50-53: Y pixels per meter (0)
  // Bytes 54-57: Colors used (0)
  // Bytes 58-61: Important colors (0)

  // Pixel data (RGBA, starting at offset 62)
  int pixelDataOffset = 62;
  for (int i = 0; i < pixelDataSize; i += 4) {
    ico[pixelDataOffset + i] = 255;      // Blue
    ico[pixelDataOffset + i + 1] = 255;  // Green
    ico[pixelDataOffset + i + 2] = 255;  // Red
    ico[pixelDataOffset + i + 3] = 255;  // Alpha
  }

  // AND mask (1 bit per pixel, starting after pixel data)
  // For a fully opaque white image, all mask bits should be
  // 0 (transparent areas would be 1) But for a solid white
  // icon, we set all bits to 0 (fully opaque)
  int andMaskOffset = pixelDataOffset + pixelDataSize;
  for (int i = 0; i < andMaskSize; i++) {
    ico[andMaskOffset + i] =
        0x00;  // All bits 0 = fully opaque
  }

  return ico;
}