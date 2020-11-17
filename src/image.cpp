#include "image.h"

#include <cassert>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

bool hsla::operator == (const hsla& rhs) const
{
    for (auto i = 0; i < 4; i++)
    {
        if (e[i] - rhs.e[i] > 0.001f) return false;
    }

    return true;
}

rgba rgba::operator * (const float rhs) const
{
    rgba ret{};
    for (auto y = 0; y < 4; y++) {
        auto mult = static_cast<float>(e[y]) * rhs;

        if (mult > 255) mult = 255;
        if (mult < 0) mult = 0;

        ret.e[y] = static_cast<unsigned char>(mult);
    }
    return ret;
}

rgba rgba::operator + (const float rhs) const
{
    rgba ret{};
    for (auto y = 0; y < 4; y++) {
        auto res = static_cast<float>(e[y]) + rhs;

        if (res > 255) res = 255;
        if (res < 0) res = 0;

        ret.e[y] = static_cast<unsigned char>(res);
    }
    return ret;
}

rgba rgba::operator + (const rgba& rhs) const
{
    rgba ret{};

    for (auto y = 0; y < 4; y++) {
        int res = e[y] + rhs.e[y];

        if (res > 255) res = 255;
        if (res < 0) res = 0;

        ret.e[y] = static_cast<unsigned char>(res);
    }
    return ret;
}

bool rgba::operator == (const rgba& rhs) const
{
    for (auto i = 0; i < 4; i++)
    {
        if (e[i] != rhs.e[i]) return false;
    }

    return true;
}


/*
    Converts an rgb value in the range 0 - 255 to hlsl.

    Based on the algorithm here:

    https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_RGB
*/
hsla rgb_to_hsl(const rgba& val)
{
    const auto r = static_cast<float>(val.r) / 255.0f;
    const auto g = static_cast<float>(val.g) / 255.0f;
    const auto b = static_cast<float>(val.b) / 255.0f;

    const auto max = std::max(std::max(r, g), b);
    const auto min = std::min(std::min(r, g), b);
    assert(max >= min);

    auto hue = 0.0f;
    auto saturation = 0.0f;
    const auto lightness = (max + min) / 2.0f;
    const auto chroma = max - min;

    if (chroma > 0)
    {
        if (lightness != 0 && lightness != 1)
        {
            saturation = (max - lightness) / std::min(lightness, 1 - lightness);
        }

        if (r > b && r > g)
        {
            hue = (g - b) / chroma;
        }
        else if (g > b)
        {
            hue = (b - r) / chroma + 2.0f;
        }
        else
        {
            hue = (r - g) / chroma + 4.0f;
        }

        hue *= 60.0f;

        //transform negative values into a positive space
        if (hue < 0) hue = 360.0f + hue;
    }

    hsla res{ roundf(hue) / 360.0f, saturation, lightness };

    assert(res.h >= 0 && res.h <= 1);
    assert(res.s >= 0 && res.s <= 1);
    assert(res.l >= 0 && res.l <= 1);

    return res;
}

float hue_to_rgb(const float p, const float q, float t)
{
    if (t < 0.0f) t += 1;
    if (t > 1.0f) t -= 1;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

/*
    Converts hsl in the range of 0-1 to rgb in
    range 0-255

    Based on the algorithm here:

        https://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c
*/
rgba hsl_to_rgb(const hsla& val)
{
    float r = 0, g = 0, b = 0;

    //zero saturation, so the final color is greyscale
    if (val.s == 0)
    {
        r = g = b = val.l;
    }
    else
    {
        const auto q = val.l < 0.5f ? val.l * (1.0f + val.s) : val.l + val.s - val.l * val.s;
        const auto p = 2.0f * val.l - q;
        r = hue_to_rgb(p, q, val.h + 1.0f / 3.0f);
        g = hue_to_rgb(p, q, val.h);
        b = hue_to_rgb(p, q, val.h - 1.0f / 3.0f);
    }

    return {
        static_cast<unsigned char>(std::min(floorf(r * 256.0f),255.0f)),
        static_cast<unsigned char>(std::min(floorf(g * 256.0f),255.0f)),
        static_cast<unsigned char>(std::min(floorf(b * 256.0f),255.0f)),
        255
    };
}

int image::stride() const 
{ 
    return width * n_channels;
}

inline void set_pixel(image& out, const rgba& col, int x, int y)
{
    //only support rgba images
	assert(out.n_channels == 4);

	//read from bottom up
	y = out.width - y - 1;	

    const auto idx = (y * out.width + x);
    const auto max_size = out.width * out.height;

	//bounds check
    assert(idx < max_size);
    assert(idx >= 0);

    auto* accessor = reinterpret_cast<rgba*>(out.data) + idx;
    *accessor = col;
}

inline rgba get_pixel(image& out, int x, int y) 
{
    //only support rgba images
    assert(out.n_channels == 4);

    //read from bottom up
	y = out.height - y - 1;

    const auto idx = y * out.width + x;
    const auto max_size = out.width * out.height;

	//bounds check
    assert(idx < max_size);
    assert(idx >= 0);

    const auto* accessor = reinterpret_cast<rgba*>(out.data) + idx;
    return *accessor;
}

inline  v3 get_normal(image& out, int x, int y) 
{
	const auto pixel = get_pixel(out, x, y);

    return {
        static_cast<float>(pixel.r) / 255.0f * 2.0f - 1.0f,
        static_cast<float>(pixel.g) / 255.0f * 2.0f - 1.0f,
        static_cast<float>(pixel.b) / 255.0f * 2.0f - 1.0f
    };
}

bool load_image(const char* path, image& out)
{
    auto width = 0, height = 0, comp = 0;
    auto* pixels = stbi_load(path, &width, &height, &comp, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        return false;
    }

    out.width = width;
    out.height = height;
    out.n_channels = 4;
    out.data = pixels;

    return true;
}
