#pragma once

#include "render_types.h"

extern VertexBufferID g_quad_vbo;
extern IndexArrayID g_quad_ibo;
extern VertSpec g_sprite_batch_vert_spec;

#define SPRITE_ATTRIB_POSITION 0
#define SPRITE_ATTRIB_COLOR 1
#define SPRITE_ATTRIB_SPRITE_UV 2
#define SPRITE_ATTRIB_MODEL 3

void init_rendering();
void deinit_rendering();
