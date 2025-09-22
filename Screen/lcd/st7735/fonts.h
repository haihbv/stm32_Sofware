#ifndef FONTS_H
#define FONTS_H

#include "stdint.h"

#pragma pack(push, 1)
typedef struct
{
  const uint8_t width;
  const uint8_t height;
  const uint16_t *data;
} Font_Define_t;
#pragma pack(pop)

#define FONT_6x8
#define FONT_7x10
#define FONT_11x18

#ifdef FONT_6x8
extern const Font_Define_t Font_6x8;
#endif

#ifdef FONT_7x10
extern const Font_Define_t Font_7x10;
#endif

#ifdef FONT_11x18
extern const Font_Define_t Font_11x18;
#endif

#ifdef FONT_16x15
extern const Font_Define_t Font_16x15;
#endif

#ifdef FONT_16x26
extern const Font_Define_t Font_16x26;
#endif

#ifdef FONT_16x24
extern const Font_Define_t Font_16x24;
#endif

#endif /* FONTS_H */
