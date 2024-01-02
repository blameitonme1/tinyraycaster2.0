#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cassert>
uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
{
    // returns a 4 bytes integer, each byte represent a color channel.
    return (a << 24) + (b << 16) + (g << 8) + r;
}
void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a)
{
    // from a given color, unpack all the components.
    r = (color >> 0) & 255;
    g = (color >> 8) & 255;
    b = (color >> 16) & 255;
    a = (color >> 24) & 255;
}
void drop_ppm_image(const std::string filename, const std::vector<uint32_t> &image, const size_t w, const size_t h)
{
    assert(image.size() == w * h);
    std::ofstream ofs(filename, std::ios::binary);
    ofs << "P6\n"
        << w << " " << h << "\n255\n";
    for (size_t i = 0; i < h * w; ++i)
    {
        uint8_t r, g, b, a;
        unpack_color(image[i], r, g, b, a);
        ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }
    ofs.close();
}

void draw_rectangle(std::vector<uint32_t> &img, const size_t img_w, const size_t img_h,
                    const size_t x, const size_t y, const size_t w, const size_t h,
                    const uint32_t color)
{

    assert(img.size() == img_w * img_h);
    for (size_t i = 0; i < w; ++i)
    {
        for (size_t j = 0; j < h; ++j)
        {
            size_t cx = x + i;
            size_t cy = y + j;
            assert(cx < img_w && cy < img_h);
            img[cx + cy * img_w] = color;
        }
    }
}
int main()
{
    const size_t win_w = 512;
    const size_t win_h = 512;
    std::vector<uint32_t> framebuffer(win_w * win_h, 255); // initialising to red
    for (size_t j = 0; j < win_h; ++j)
    {
        for (size_t i = 0; i < win_w; ++i)
        {
            // show color gradient across the image
            uint8_t r = 255 * j / float(win_h);
            uint8_t g = 255 * i / float(win_w);
            uint8_t b = 0;
            framebuffer[i + j * win_w] = pack_color(r, g, b);
        }
    }
    drop_ppm_image("./demo.ppm", framebuffer, win_w, win_h);
}