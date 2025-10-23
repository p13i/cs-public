#include "cs/net/http/favicon.hh"

#include <vector>

std::vector<unsigned char>
cs::net::http::GenerateWhiteIco() {
  const int width = 16, height = 16, bytesPerPixel = 4;
  const int pixelDataSize = width * height * bytesPerPixel;
  const int icoSize = 6 + 16 + 40 + pixelDataSize;

  std::vector<unsigned char> ico(icoSize, 0);

  // ICO Header (6 bytes)
  ico[2] = 1;  // Image type (1 = icon)
  ico[4] = 1;  // Number of images (1)

  // Icon directory entry (16 bytes)
  ico[6] = width;   // Width (16)
  ico[7] = height;  // Height (16)
  ico[8] = 0;       // Color count (0 = 24-bit)
  ico[12] = 40;     // Offset to image data (6 + 16)

  // Bitmap InfoHeader (40 bytes)
  ico[22] =
      height * 2;  // ICO stores height * 2 for AND mask

  ico[14] = 40;  // Info header size
  ico[18] = width;
  ico[22] =
      height * 2;  // Height (16 for color + 16 for mask)
  ico[26] = 1;     // Planes
  ico[28] = 32;    // Bits per pixel (RGBA)

  // Pixel data (white, 16x16 RGBA)
  int pixelDataOffset = 6 + 16 + 40;
  for (int i = 0; i < pixelDataSize; i += 4) {
    ico[pixelDataOffset + i] = 255;      // Blue
    ico[pixelDataOffset + i + 1] = 255;  // Green
    ico[pixelDataOffset + i + 2] = 255;  // Red
    ico[pixelDataOffset + i + 3] = 255;  // Alpha
  }

  return ico;
}