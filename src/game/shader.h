#pragma once
#include "../engine/engine.h"
const char * vs = 
    SHADER_VERSION_STR
    "precision lowp float;\n"
    "in vec3 position;    \n"
    "in vec3 texture;    \n"
    "out vec3 texture_dts;    \n"
    "uniform mat4 projection;    \n"
    "void main() {               \n"
    "   gl_Position = projection * vec4(position.xyz, 1.0);  \n"
    "   texture_dts = texture; \n"
    "}";

const char * fs = 
    SHADER_VERSION_STR
    "precision lowp float;\n"
    "uniform sampler2D texture_slots[10];\n"
    "in vec3 texture_dts;    \n"
    "vec3 texture_details;    \n"
    "out vec4 frag_col;\n"
    "float rand(vec2 co){ \n"
    "   return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453); \n"
    "} \n"
    "void main() {                                \n"
    "   texture_details = texture_dts;\n"
    "   int slot = int(texture_details.z); \n "
    "   if (slot == 0) { frag_col = texture(texture_slots[0], texture_details.xy); } \n" 
    "   if (slot == 1) { frag_col = texture(texture_slots[1], texture_details.xy); } \n" 
    "   if (slot == 2) { frag_col = texture(texture_slots[2], texture_details.xy); } \n" 
    "   if (slot == 3) { frag_col = texture(texture_slots[3], texture_details.xy); } \n" 
    "   if (slot == 4) { frag_col = texture(texture_slots[4], texture_details.xy); } \n" 
    "   if (slot == 5) { frag_col = texture(texture_slots[5], texture_details.xy); } \n" 
    "   if (slot == 6) { frag_col = texture(texture_slots[6], texture_details.xy); } \n" 
    "   if (slot == 7) { frag_col = texture(texture_slots[7], texture_details.xy); } \n" 
    "   if (slot == 8) { frag_col = texture(texture_slots[8], texture_details.xy); } \n" 
    "   if (slot == 9) { frag_col = texture(texture_slots[9], texture_details.xy); } \n" 
    "   frag_col.a = 1.0; \n"
    "   if (frag_col.r == 0 && frag_col.g == 0 && frag_col.b == 0) { frag_col.a = 0; } \n"
    "}";

