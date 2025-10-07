#pragma once

#include <cstdint>

using shader_func_t = std::uint32_t (*)(int x, int y);

extern const shader_func_t shader_hue_shift;
extern const shader_func_t shader_hue_shift_dim;
extern const shader_func_t shader_white;
extern const shader_func_t shader_red;
extern const shader_func_t shader_yellow;
extern const shader_func_t shader_green;
extern const shader_func_t shader_turquoise;
extern const shader_func_t shader_blue;
extern const shader_func_t shader_purple;