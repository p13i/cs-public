// cs/net/http/favicon_test.gpt.cc
#include "cs/net/http/favicon.hh"

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {  // use_usings
using ::cs::net::http::GenerateWhiteIco;
using ::testing::Eq;
using ::testing::SizeIs;
}  // namespace

namespace {

const int kExpectedWidth = 16;
const int kExpectedHeight = 16;
const int kBytesPerPixel = 4;
const int kPixelDataSize = kExpectedWidth *
                           kExpectedHeight *
                           kBytesPerPixel;  // 1024
const int kAndMaskSize =
    (kExpectedWidth * kExpectedHeight + 7) / 8;  // 32
const int kBitmapDataSize =
    40 + kPixelDataSize + kAndMaskSize;  // 1096
const int kIcoHeaderSize = 6;
const int kIconDirEntrySize = 16;
const int kExpectedIcoSize = kIcoHeaderSize +
                             kIconDirEntrySize +
                             kBitmapDataSize;  // 1118

}  // namespace

// -----------------------------------------------------------------------------
// Size and structure
// -----------------------------------------------------------------------------

TEST(FaviconTest, GenerateWhiteIcoReturnsCorrectTotalSize) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  EXPECT_THAT(ico, SizeIs(kExpectedIcoSize));
}

TEST(FaviconTest, GenerateWhiteIcoSizeIs1118) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  EXPECT_THAT(ico.size(), Eq(1118u));
}

// -----------------------------------------------------------------------------
// ICO file header (6 bytes, offset 0)
// -----------------------------------------------------------------------------

TEST(FaviconTest, IcoHeaderReservedZero) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  EXPECT_THAT(ico[0], Eq(0));
  EXPECT_THAT(ico[1], Eq(0));
}

TEST(FaviconTest, IcoHeaderTypeIsOne) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  EXPECT_THAT(ico[2], Eq(1));
}

TEST(FaviconTest, IcoHeaderNumImagesIsOne) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint16_t num = static_cast<uint16_t>(ico[4]) |
                 (static_cast<uint16_t>(ico[5]) << 8);
  EXPECT_THAT(num, Eq(1));
}

// -----------------------------------------------------------------------------
// Icon directory entry (16 bytes, offset 6)
// -----------------------------------------------------------------------------

TEST(FaviconTest, IconDirEntryWidthHeight) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  EXPECT_THAT(ico[6], Eq(16));
  EXPECT_THAT(ico[7], Eq(16));
}

TEST(FaviconTest, IconDirEntryColorCountZero) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  EXPECT_THAT(ico[8], Eq(0));
}

TEST(FaviconTest, IconDirEntryBitCount32) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint16_t bit_count =
      static_cast<uint16_t>(ico[12]) |
      (static_cast<uint16_t>(ico[13]) << 8);
  EXPECT_THAT(bit_count, Eq(32));
}

TEST(FaviconTest, IconDirEntryBitmapDataSize) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint32_t size = static_cast<uint32_t>(ico[14]) |
                  (static_cast<uint32_t>(ico[15]) << 8) |
                  (static_cast<uint32_t>(ico[16]) << 16) |
                  (static_cast<uint32_t>(ico[17]) << 24);
  EXPECT_THAT(size, Eq(1096u));
}

// -----------------------------------------------------------------------------
// BITMAPINFOHEADER (40 bytes, offset 22)
// -----------------------------------------------------------------------------

TEST(FaviconTest, BitmapInfoHeaderSize40) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint32_t header_size =
      static_cast<uint32_t>(ico[22]) |
      (static_cast<uint32_t>(ico[23]) << 8) |
      (static_cast<uint32_t>(ico[24]) << 16) |
      (static_cast<uint32_t>(ico[25]) << 24);
  EXPECT_THAT(header_size, Eq(40u));
}

TEST(FaviconTest, BitmapInfoHeaderWidth16) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint32_t width = static_cast<uint32_t>(ico[26]) |
                   (static_cast<uint32_t>(ico[27]) << 8) |
                   (static_cast<uint32_t>(ico[28]) << 16) |
                   (static_cast<uint32_t>(ico[29]) << 24);
  EXPECT_THAT(width, Eq(16u));
}

TEST(FaviconTest, BitmapInfoHeaderHeight32) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint32_t height = static_cast<uint32_t>(ico[30]) |
                    (static_cast<uint32_t>(ico[31]) << 8) |
                    (static_cast<uint32_t>(ico[32]) << 16) |
                    (static_cast<uint32_t>(ico[33]) << 24);
  EXPECT_THAT(height, Eq(32u));
}

TEST(FaviconTest, BitmapInfoHeaderPlanes1) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint16_t planes = static_cast<uint16_t>(ico[34]) |
                    (static_cast<uint16_t>(ico[35]) << 8);
  EXPECT_THAT(planes, Eq(1));
}

TEST(FaviconTest, BitmapInfoHeaderBitCount32) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint16_t bit_count =
      static_cast<uint16_t>(ico[36]) |
      (static_cast<uint16_t>(ico[37]) << 8);
  EXPECT_THAT(bit_count, Eq(32));
}

TEST(FaviconTest, BitmapInfoHeaderCompressionZero) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  uint32_t comp = static_cast<uint32_t>(ico[38]) |
                  (static_cast<uint32_t>(ico[39]) << 8) |
                  (static_cast<uint32_t>(ico[40]) << 16) |
                  (static_cast<uint32_t>(ico[41]) << 24);
  EXPECT_THAT(comp, Eq(0u));
}

// -----------------------------------------------------------------------------
// Pixel data (RGBA, offset 62, 1024 bytes)
// -----------------------------------------------------------------------------

TEST(FaviconTest, PixelDataAllWhite) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  const int pixel_offset = 62;
  for (int i = 0; i < kPixelDataSize; i += 4) {
    EXPECT_THAT(ico[pixel_offset + i], Eq(255))
        << "B at pixel " << (i / 4);
    EXPECT_THAT(ico[pixel_offset + i + 1], Eq(255))
        << "G at pixel " << (i / 4);
    EXPECT_THAT(ico[pixel_offset + i + 2], Eq(255))
        << "R at pixel " << (i / 4);
    EXPECT_THAT(ico[pixel_offset + i + 3], Eq(255))
        << "A at pixel " << (i / 4);
  }
}

TEST(FaviconTest, PixelDataSize1024) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  int count = 0;
  for (size_t i = 62; i < 62 + kPixelDataSize; ++i) {
    count++;
  }
  EXPECT_THAT(count, Eq(kPixelDataSize));
}

// -----------------------------------------------------------------------------
// AND mask (1 bit per pixel, after pixel data)
// -----------------------------------------------------------------------------

TEST(FaviconTest, AndMaskAllZero) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  const int and_mask_offset = 62 + kPixelDataSize;
  for (int i = 0; i < kAndMaskSize; i++) {
    EXPECT_THAT(ico[and_mask_offset + i], Eq(0))
        << "AND mask byte " << i;
  }
}

TEST(FaviconTest, AndMaskSize32) {
  std::vector<unsigned char> ico = GenerateWhiteIco();
  EXPECT_THAT(kAndMaskSize, Eq(32));
}

// -----------------------------------------------------------------------------
// Idempotence / consistency
// -----------------------------------------------------------------------------

TEST(FaviconTest, GenerateWhiteIcoIdempotent) {
  std::vector<unsigned char> ico1 = GenerateWhiteIco();
  std::vector<unsigned char> ico2 = GenerateWhiteIco();
  EXPECT_THAT(ico1.size(), Eq(ico2.size()));
  for (size_t i = 0; i < ico1.size(); i++) {
    EXPECT_THAT(ico1[i], Eq(ico2[i])) << "byte " << i;
  }
}
