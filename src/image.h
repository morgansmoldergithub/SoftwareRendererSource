#ifndef IMAGE_H
#define IMAGE_H

#include "maths.h"

/*
 * NOTE: This struct represents hue in the range 0-1, not 0-360 degrees as
 * some might expect.
 *
 * For this particular application 0-1 was the most convenient representation,
 * so that is what I went with. If I were to do more color work I would probably
 * consider changing it.
 */
struct hsla
{
    union{
        struct{
            float h, s, l, a;
        };
        float e[4];
    };
    bool operator == (const hsla& rhs) const;
};

#pragma pack(push)
struct rgba
{
    union {
        struct {
            unsigned char r, g, b, a;
        };
        unsigned char e[4];
    };
    rgba operator * (float rhs) const;
    rgba operator + (float rhs) const;
    rgba operator + (const rgba& rhs) const;
    bool operator == (const rgba& rhs) const;
};
#pragma pack(pop)

const rgba red{ 255, 0, 0, 255 };
const rgba green{ 0, 255, 0, 255 };
const rgba blue{ 0, 0,   255, 255 };
const rgba orange{ 0, 169, 255, 255 };
const rgba white{ 255, 255, 255, 255 };

rgba hsl_to_rgb(const hsla& val);
hsla rgb_to_hsl(const rgba& val);

struct image {
    int width{}, height{};
    int n_channels = 4;
    unsigned char* data{};

    int stride() const;
};

inline void set_pixel(image& out, const rgba& col, int x, int y);
inline rgba get_pixel(image& out, int x, int y);
inline v3 get_normal(image& out, int x, int y);
bool load_image(const char* path, image& out);
#endif
