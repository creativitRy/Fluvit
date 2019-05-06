//
// Created by gahwon on 5/5/19.
//

#include <png.h>
#include "pngio.h"

bool pngio::load(const std::string &file_name, std::vector<uint32_t> &image, int &width, int &height) {
    FILE *file = fopen(file_name.c_str(), "rb");
    if (file == nullptr)
        return false;

    auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png)
        return false;

    auto info = png_create_info_struct(png);
    if (!info)
        return false;

    if (setjmp(png_jmpbuf(png)))
        return false;

    png_init_io(png, file);
    png_read_info(png, info);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    auto color_type = png_get_color_type(png, info);
    auto bit_depth = png_get_bit_depth(png, info);

    // read any color_type into 8bit depth, RGBA format

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // these color_type don't have an alpha channel then fill it with 0xff
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    auto rows = new png_bytep[height];
    for (int y = 0; y < height; ++y) {
        rows[y] = (png_byte *) malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, rows);

    image.clear();
    for (int y = 0; y < height; ++y) {
        auto row = rows[y];
        for (int x = 0; x < width; ++x) {
            auto px = &(row[x * 4]);
            image.emplace_back((uint32_t) px[0] | ((uint32_t) px[1] << 8u) | ((uint32_t) px[2] << 16u) |
                               ((uint32_t) px[3] << 24u));
        }
    }

    fclose(file);
    png_destroy_read_struct(&png, &info, nullptr);

    return true;
}
