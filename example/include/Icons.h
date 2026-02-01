#pragma once

#include <interactive-ui/components/PixelBufferComponent.h>

#include <util/ArrayView.h>

constexpr uint32_t battery_icon_data[10][16] = {
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

constexpr uint32_t speaker_icon_data[15][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};

constexpr Pixel x_icon_buffer_data[] = {
    {{0, 0}, 1},
    {{4, 0}, 1},
    {{1, 1}, 1},
    {{3, 1}, 1},
    {{2, 2}, 1},
    {{1, 3}, 1},
    {{3, 3}, 1},
    {{0, 4}, 1},
    {{4, 4}, 1}
};

constexpr Pixel volume_level_1_buffer_data[] = {
    {{0, 0}, 1},
    {{1, 1}, 1},
    {{1, 2}, 1},
    {{1, 3}, 1},
    {{1, 4}, 1},
    {{1, 5}, 1},
    {{0, 6}, 1}
};

constexpr Pixel volume_level_2_buffer_data[] = {
    {{0, 0}, 1},
    {{1, 1}, 1},
    {{2, 2}, 1},
    {{2, 3}, 1},
    {{2, 4}, 1},
    {{2, 5}, 1},
    {{2, 6}, 1},
    {{2, 7}, 1},
    {{2, 8}, 1},
    {{1, 9}, 1},
    {{0, 10}, 1}
};

constexpr Pixel volume_level_3_buffer_data[] = {
    {{0, 0}, 1},
    {{1, 1}, 1},
    {{2, 2}, 1},
    {{3, 3}, 1},
    {{3, 4}, 1},
    {{4, 5}, 1},
    {{4, 6}, 1},
    {{4, 7}, 1},
    {{4, 8}, 1},
    {{4, 9}, 1},
    {{3, 10}, 1},
    {{3, 11}, 1},
    {{2, 12}, 1},
    {{1, 13}, 1},
    {{0, 14}, 1},
};

constexpr uint32_t increment_icon_data[3][5] = {
    {0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0},
    {1, 0, 0, 0, 1}
};

constexpr ArrayView2D<uint32_t> battery_icon = make_array_view(battery_icon_data);
constexpr ArrayView2D<uint32_t> speaker_icon = make_array_view(speaker_icon_data);

constexpr ArrayView<Pixel> x_icon = make_array_view(x_icon_buffer_data);

constexpr ArrayView<Pixel> volume_level_1_icon = make_array_view(volume_level_1_buffer_data);
constexpr ArrayView<Pixel> volume_level_2_icon = make_array_view(volume_level_2_buffer_data);
constexpr ArrayView<Pixel> volume_level_3_icon = make_array_view(volume_level_3_buffer_data);

constexpr ArrayView2D<uint32_t> increment_icon = make_array_view(increment_icon_data);