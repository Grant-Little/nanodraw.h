/*
 * [API Usage]:
 *
 * In one file, you should have this code:
 *   #define NANODRAW_IMPLEMENTATION
 *   #include "nanodraw.h"
 * 
 * In all other files that need to use the library, you should only have:
 *   #include "nanodraw.h"
 *
 * Before using any functions from this library, you need to load an OpenGL 3.3 context.
 * For this, you can utilize SDL, GLFW, RGFW, or your system libraries.
 *
 * After you create the OpenGL context, you must call "nd_init()":
 *   bool nd_init(
 *       int32_t viewport_w, int32_t viewport_h,
 *       nd_Load_Proc get_gl_proc_address
 *   );
 *
 * "get_gl_proc_address" is a function that loads OpenGL functions at runtime.
 * In SDL, this is      "SDL_GL_GetProcAddress()".
 * In GLFW, this is     "glfwGetProcAddress()".
 * In RGFW, this is     "RGFW_getProcAddress_OpenGL()".
 * In Windows, this is  "wglGetProcAddress()".
 * In GLX, this is      "glXGetProcAddress()".
 * In EGL, this is      "eglGetProcAddress()".
 *
 * Your compiler will probably complain about the return types being incompatible.
 * You can just cast the function to "nd_Load_Proc" when you pass it.
 *   nd_init(800, 600, (nd_Load_Proc)glfwGetProcAddress);
 *
 * [Drawing Functions]:
 * 
 * nanodraw.h has ONE drawing function "nd_queue_triangles()".
 * All other functions like "nd_draw_rectangle()" are wrappers around "nd_queue_triangles()".
 *
 * "nd_queue_triangles()" only adds triangles to a cpu-side buffer.
 * To send those triangles to the gpu, you must call "nd_submit_draw_queue()".
 * You should try to minimize your calls to "nd_submit_draw_queue()".
 *
 * Do this:
 *   nd_draw_rectangle();
 *   nd_draw_rectangle();
 *   nd_draw_rectangle();
 *   nd_submit_draw_queue();
 *
 * Do NOT do this:
 *   nd_draw_rectangle();
 *   nd_submit_draw_queue();
 *   nd_draw_rectangle();
 *   nd_submit_draw_queue();
 *   nd_draw_rectangle();
 *   nd_submit_draw_queue();
 *
 * [Invalid Operations]:
 *
 * Because nanodraw.h keeps draw data in a buffer before sending it to the gpu,
 * there are a few operations that should not happen in certain scenarios.
 *
 * You should not:
 *   - free/deinit a texture or framebuffer while drawing operations are queued
 *   - read texture data while drawing operations are queued
 *   - clear a framebuffer while drawing operations are queued
 *
 * If you want to perform any of the above operations,
 * you must empty the draw queue by calling "nd_submit_draw_queue()".
 * From a game-loop perspective, you would want to perform the above operations
 * only at the beginning or end of a frame.
 * nanodraw.h will prevent you from performing invalid operations via ND_ASSERT.
 *
 * [Configuration Macros]:
 * nanodraw.h has a few macros you can define before including the library:
 *   NDDEF
 *   ND_REALLOC
 *   ND_FREE
 *   ND_ASSERT
 *   ND_INITIAL_DRAW_QUEUE_CAP
 */
#ifndef NANODRAW_H
#define NANODRAW_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef NDDEF
    #define NDDEF extern
#endif

typedef struct {
    uint8_t r, g, b, a;
} nd_Color;

#define ND_RED (nd_Color){255, 0, 0, 255}
#define ND_GREEN (nd_Color){0, 255, 0, 255}
#define ND_BLUE (nd_Color){0, 0, 255, 255}
#define ND_YELLOW (nd_Color){255, 255, 0, 255}
#define ND_MAGENTA (nd_Color){255, 0, 255, 255}
#define ND_AQUA (nd_Color){0, 255, 255, 255}
#define ND_ORANGE (nd_Color){255, 128, 0, 255}
#define ND_PURPLE (nd_Color){128, 0, 128, 255}
#define ND_WHITE (nd_Color){255, 255, 255, 255}
#define ND_BLACK (nd_Color){0, 0, 0, 255}
#define ND_GRAY (nd_Color){128, 128, 128, 255}
#define ND_NO_COLOR (nd_Color){0, 0, 0, 0}

typedef struct {
    uint32_t id;
    int32_t w;
    int32_t h;
} nd_Texture;

#define ND_TEXTURE_NULL ((nd_Texture){0})

typedef enum {
    ND_FILTER_NEAREST,
    ND_FILTER_LINEAR,
} nd_Texture_Filter;

typedef enum {
    ND_BLEND_NONE,
    ND_BLEND_ALPHA,
    ND_BLEND_ALPHA_PRE_MUL,
    ND_BLEND_ADD,
    ND_BLEND_ADD_PRE_ALPHA,
    ND_BLEND_MUL,
} nd_Blend;

typedef struct {
    uint32_t id;
    int32_t w;
    int32_t h;
} nd_Framebuffer;

#define ND_FRAMEBUFFER_NULL ((nd_Framebuffer){0})

typedef void (*(*nd_Load_Proc)(const char *))(void);

NDDEF bool nd_init(
    int32_t viewport_w, int32_t viewport_h,
    nd_Load_Proc get_gl_proc_address
);

NDDEF void nd_deinit(
    void
);

NDDEF void nd_update_viewport(
    int32_t w, int32_t h
);

NDDEF void nd_init_texture(
    nd_Texture *tex__out,
    int32_t w, int32_t h,
    nd_Texture_Filter min_filter, nd_Texture_Filter mag_filter
);

NDDEF void nd_deinit_texture(
    nd_Texture *tex
);

NDDEF void nd_init_texture_from_image_buffer(
    nd_Texture *tex__out,
    const uint8_t *image_rgba8888,
    int32_t image_w, int32_t image_h,
    nd_Texture_Filter min_filter, nd_Texture_Filter mag_filter
);

NDDEF void nd_texture_to_preallocated_image_buffer(
    uint8_t *image_rgba8888__out,
    nd_Texture tex
);

NDDEF bool nd_init_framebuffer_from_texture(
    nd_Framebuffer *fb__out,
    nd_Texture tex
);

NDDEF void nd_clear_framebuffer(
    nd_Framebuffer fb,
    nd_Color c
);

NDDEF void nd_deinit_framebuffer(
    nd_Framebuffer *fb
);

NDDEF bool nd_submit_draw_queue(
    void
);

NDDEF void nd_draw_triangle(
    nd_Framebuffer fb,
    nd_Blend blend,
    float x1, float y1,
    float x2, float y2,
    float x3, float y3,
    nd_Color c
);

NDDEF void nd_draw_triangle_3color(
    nd_Framebuffer fb,
    nd_Blend blend,
    float x1, float y1, nd_Color c1,
    float x2, float y2, nd_Color c2,
    float x3, float y3, nd_Color c3
);

NDDEF void nd_draw_triangle_textured(
    nd_Framebuffer fb,
    nd_Texture tex,
    nd_Blend blend,
    float x1, float y1, float tex_x1, float tex_y1,
    float x2, float y2, float tex_x2, float tex_y2,
    float x3, float y3, float tex_x3, float tex_y3
);

NDDEF void nd_draw_rectangle(
    nd_Framebuffer fb,
    nd_Blend blend,
    float min_x, float min_y,
    float w, float h,
    nd_Color c
);

NDDEF void nd_draw_rectangle_4color(
    nd_Framebuffer fb,
    nd_Blend blend,
    float min_x, float min_y,
    float w, float h,
    nd_Color c1, nd_Color c2, nd_Color c3, nd_Color c4
);

NDDEF void nd_draw_rectangle_textured(
    nd_Framebuffer fb,
    nd_Texture tex,
    nd_Blend blend,
    float min_x, float min_y,
    float w, float h,
    float tex_min_x, float tex_min_y,
    float tex_w, float tex_h
);

NDDEF void nd_draw_texture(
    nd_Framebuffer fb,
    nd_Texture tex,
    nd_Blend blend,
    float min_x, float min_y
);

typedef struct {
    int x_min;
    int width;
} nd__Rune_Descriptor;

typedef struct {
    nd_Texture tex;
    int character_spacing;
    nd__Rune_Descriptor runes[127]; // 32 to 126, representing ascii printable characters
} nd_Image_Font;

#define ND_IMAGE_FONT_NULL ((nd_Image_Font){0})

extern nd_Image_Font nd_default_image_font;

NDDEF void nd_init_image_font_from_image_buffer(
    nd_Image_Font *font__out,
    const uint8_t *image_rgba8888,
    int32_t image_w, int32_t image_h,
    nd_Color separator_color,
    int character_spacing,
    nd_Texture_Filter min_filter, nd_Texture_Filter mag_filter
);

NDDEF void nd_deinit_image_font(
    nd_Image_Font *font
);

NDDEF void nd_draw_string_from_image_font(
    nd_Framebuffer fb,
    nd_Blend blend,
    nd_Image_Font font,
    unsigned int scale,
    float min_x, float min_y,
    const char *string
);

NDDEF void nd_draw_string_from_image_font_len(
    nd_Framebuffer fb,
    nd_Blend blend,
    nd_Image_Font font,
    unsigned int scale,
    float min_x, float min_y,
    const char *string,
    size_t len
);

NDDEF size_t nd_measure_string_from_image_font(
    nd_Image_Font font,
    unsigned int scale,
    const char *string,
);

NDDEF size_t nd_measure_string_from_image_font_len(
    nd_Image_Font font,
    unsigned int scale,
    const char *string,
    size_t len
);

typedef struct {
    float x1; float y1; float tex_x1; float tex_y1; nd_Color c1;
    float x2; float y2; float tex_x2; float tex_y2; nd_Color c2;
    float x3; float y3; float tex_x3; float tex_y3; nd_Color c3;
} nd_Triangle_Data;

NDDEF void nd_queue_triangles(
    nd_Framebuffer fb,
    nd_Texture tex,
    nd_Blend blend,
    nd_Triangle_Data *triangles,
    uint32_t num_triangles
);

#endif // #ifndef NANODRAW_H

#ifdef NANODRAW_IMPLEMENTATION

#include <string.h>

#define ND_GL_ARRAY_BUFFER 0x8892
#define ND_GL_STREAM_DRAW 0x88E0
#define ND_GL_FLOAT 0x1406
#define ND_GL_UNSIGNED_SHORT 0x1403
#define ND_GL_UNSIGNED_BYTE 0x1401
#define ND_GL_VERTEX_SHADER 0x8B31
#define ND_GL_FRAGMENT_SHADER 0x8B30
#define ND_GL_SRC_ALPHA 0x0302
#define ND_GL_ONE_MINUS_SRC_ALPHA 0x0303
#define ND_GL_ZERO 0
#define ND_GL_ONE 1
#define ND_GL_SRC_COLOR 0x0300
#define ND_GL_TEXTURE_2D 0x0DE1
#define ND_GL_TEXTURE_WRAP_S 0x2802
#define ND_GL_CLAMP_TO_EDGE 0x812F
#define ND_GL_TEXTURE_WRAP_T 0x2803
#define ND_GL_TEXTURE_MIN_FILTER 0x2801
#define ND_GL_TEXTURE_MAG_FILTER 0x2800
#define ND_GL_RGBA8 0x8058
#define ND_GL_RGBA 0x1908
#define ND_GL_FRAMEBUFFER 0x8D40
#define ND_GL_COLOR_ATTACHEMENT0 0x8CE0
#define ND_GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define ND_GL_COLOR_BUFFER_BIT 0x4000
#define ND_GL_MAP_WRITE_BIT 0x0002
#define ND_GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define ND_GL_MAP_UNSYNCHRONIZED_BIT 0x0020
#define ND_GL_DRAW_FRAMEBUFFER 0x8CA9
#define ND_GL_BLEND 0x0BE2
#define ND_GL_TRIANGLES 0x0004
#define ND_GL_DEPTH_TEST 0x0B71
#define ND_GL_UNPACK_ALIGNMENT 0x0CF5
#define ND_GL_NEAREST 0x2600
#define ND_GL_LINEAR 0x2601

typedef uint32_t (*nd__PFNGLGETERRORPROC)(void);
typedef void (*nd__PFNGLGENVERTEXARRAYSPROC)(int32_t, uint32_t *);
typedef void (*nd__PFNGLGENBUFFERSPROC)(int32_t, uint32_t *);
typedef void (*nd__PFNGLBINDVERTEXARRAYPROC)(uint32_t);
typedef void (*nd__PFNGLBINDBUFFERPROC)(uint32_t, uint32_t);
typedef void (*nd__PFNGLBUFFERDATAPROC)(uint32_t, intptr_t, const void *, uint32_t);
typedef void (*nd__PFNGLENABLEVERTEXATTRIBARRAYPROC)(uint32_t);
typedef void (*nd__PFNGLVERTEXATTRIBPOINTERPROC)(uint32_t, int32_t, uint32_t, uint8_t, int32_t, const void *);
typedef uint32_t (*nd__PFNGLCREATESHADERPROC)(uint32_t);
typedef void (*nd__PFNGLSHADERSOURCEPROC)(uint32_t, int32_t, const char **, const int32_t *);
typedef void (*nd__PFNGLCOMPILERSHADERPROC)(uint32_t);
typedef uint32_t (*nd__PFNGLCREATEPROGRAMPROC)(void);
typedef void (*nd__PFNGLATTACHSHADERPROC)(uint32_t, uint32_t);
typedef void (*nd__PFNGLLINKPROGRAMPROC)(uint32_t);
typedef void (*nd__PFNGLUSEPROGRAMPROC)(uint32_t);
typedef void (*nd__PFNGLDELETESHADERPROC)(uint32_t);
typedef void (*nd__PFNGLVIEWPORTPROC)(int32_t, int32_t, int32_t, int32_t);
typedef void (*nd__PFNGLBLENDFUNCPROC)(uint32_t, uint32_t);
typedef void (*nd__PFNGLDELETETEXTURESPROC)(int32_t, const uint32_t *);
typedef void (*nd__PFNGLGENTEXTURESPROC)(int32_t, uint32_t *);
typedef void (*nd__PFNGLBINDTEXTUREPROC)(uint32_t, uint32_t);
typedef void (*nd__PFNGLTEXPARAMETERIPROC)(uint32_t, uint32_t, int32_t);
typedef void (*nd__PFNGLTEXIMAGE2DPROC)(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, const void *);
typedef void (*nd__PFNGLGETTEXIMAGEPROC)(uint32_t, int32_t, uint32_t, uint32_t, void *);
typedef void (*nd__PFNGLGENFRAMEBUFFERSPROC)(int32_t, uint32_t *);
typedef void (*nd__PFNGLBINDFRAMEBUFFERPROC)(uint32_t, uint32_t);
typedef void (*nd__PFNGLFRAMEBUFFERTEXTURE2DPROC)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t);
typedef uint32_t (*nd__PFNGLCHECKFRAMEBUFFERSTATUSPROC)(uint32_t);
typedef void (*nd__PFNGLCLEARCOLORPROC)(float, float, float, float);
typedef void (*nd__PFNGLCLEARPROC)(uint32_t);
typedef void (*nd__PFNGLDELETEFRAMEBUFFERSPROC)(int32_t, uint32_t *);
typedef void (*nd__PFNGLENABLEPROC)(uint32_t);
typedef void (*nd__PFNGLDISABLEPROC)(uint32_t);
typedef void *(*nd__PFNGLMAPBUFFERRANGEPROC)(uint32_t, intptr_t, intptr_t, uint32_t);
typedef bool (*nd__PFNGLUNMAPBUFFERPROC)(uint32_t);
typedef void (*nd__PFNGLDRAWARRAYSPROC)(uint32_t, int32_t, int32_t);

#define ND_GL_FUNCS_DEF(type, name) static type nd__ ## name;

#define ND_GL_FUNCS_LOAD(type, name) nd__ ## name = (type)get_gl_proc_address(#name);

#define ND_GL_FUNCS(X) \
    X(nd__PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays) \
    X(nd__PFNGLGENBUFFERSPROC, glGenBuffers) \
    X(nd__PFNGLBINDVERTEXARRAYPROC, glBindVertexArray) \
    X(nd__PFNGLBINDBUFFERPROC, glBindBuffer) \
    X(nd__PFNGLBUFFERDATAPROC, glBufferData) \
    X(nd__PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
    X(nd__PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer) \
    X(nd__PFNGLCREATESHADERPROC, glCreateShader) \
    X(nd__PFNGLSHADERSOURCEPROC, glShaderSource) \
    X(nd__PFNGLCOMPILERSHADERPROC, glCompileShader) \
    X(nd__PFNGLCREATEPROGRAMPROC, glCreateProgram) \
    X(nd__PFNGLATTACHSHADERPROC, glAttachShader) \
    X(nd__PFNGLLINKPROGRAMPROC, glLinkProgram) \
    X(nd__PFNGLUSEPROGRAMPROC, glUseProgram) \
    X(nd__PFNGLDELETESHADERPROC, glDeleteShader) \
    X(nd__PFNGLVIEWPORTPROC, glViewport) \
    X(nd__PFNGLBLENDFUNCPROC, glBlendFunc) \
    X(nd__PFNGLDELETETEXTURESPROC, glDeleteTextures) \
    X(nd__PFNGLGENTEXTURESPROC, glGenTextures) \
    X(nd__PFNGLBINDTEXTUREPROC, glBindTexture) \
    X(nd__PFNGLTEXPARAMETERIPROC, glTexParameteri) \
    X(nd__PFNGLTEXIMAGE2DPROC, glTexImage2D) \
    X(nd__PFNGLGETTEXIMAGEPROC, glGetTexImage) \
    X(nd__PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers) \
    X(nd__PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer) \
    X(nd__PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D) \
    X(nd__PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus) \
    X(nd__PFNGLCLEARCOLORPROC, glClearColor) \
    X(nd__PFNGLCLEARPROC, glClear) \
    X(nd__PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers) \
    X(nd__PFNGLENABLEPROC, glEnable) \
    X(nd__PFNGLDISABLEPROC, glDisable) \
    X(nd__PFNGLMAPBUFFERRANGEPROC, glMapBufferRange) \
    X(nd__PFNGLUNMAPBUFFERPROC, glUnmapBuffer) \
    X(nd__PFNGLDRAWARRAYSPROC, glDrawArrays)

ND_GL_FUNCS(ND_GL_FUNCS_DEF)

#if defined(ND_REALLOC) && !defined(ND_FREE) || !defined(ND_REALLOC) && defined(ND_FREE)
    #error "nanodraw.h requires defining both ND_REALLOC and ND_FREE for custom allocator support"
#endif

#ifndef ND_REALLOC
    #include <stdlib.h>
    #define ND_REALLOC(ptr, sz) realloc(ptr, sz)
    #define ND_FREE(ptr) free(ptr)
#endif

#ifndef ND_ASSERT
    #include <assert.h>
    #define ND_ASSERT(exp) assert(exp)
#endif

#ifndef ND_INITIAL_DRAW_QUEUE_CAP
    #define ND_INITIAL_DRAW_QUEUE_CAP 1024
#endif

typedef struct {
    float ndcx, ndcy; uint16_t u, v; nd_Color c;
} nd__Vertex_Layout;

typedef struct {
    nd_Framebuffer fb;
    uint32_t tex_id;
    nd_Blend blend;
} nd__Draw_Data;

typedef struct {
    nd__Draw_Data data;
    int32_t count;
} nd__Draw_Entry;

typedef struct {
    float ndc1[2]; uint16_t uv1[2]; nd_Color c1;
    float ndc2[2]; uint16_t uv2[2]; nd_Color c2;
    float ndc3[2]; uint16_t uv3[2]; nd_Color c3;
} nd__Vertex_Data;

typedef struct {
    nd__Draw_Entry *draw_entry_start;
    nd__Vertex_Data *vertex_data_start;
    int32_t len;
    int32_t cap;
    int32_t num_entries;
} nd__Draw_Queue;

typedef struct {
    int32_t viewport_w, viewport_h;
    nd__Draw_Queue draw_queue;
    uint32_t vao;
    uint32_t vbo;
    uint32_t shader_program;
    uint32_t textureless_id;
    uint32_t last_fb;
    uint32_t last_tex;
    nd_Blend last_blend;
} nd__Context;

static nd__Context nd__ctx;

static void nd__screen_to_ndc_yflip(float *ndc__out, float x, float y, float w, float h) {
    ndc__out[0] = ((x / w) * 2.0f) - 1.0f;
    ndc__out[1] = ((y / h) * -2.0f) + 1.0f;
}

static void nd__screen_to_ndc_no_yflip(float *ndc__out, float x, float y, float w, float h) {
    ndc__out[0] = ((x / w) * 2.0f) - 1.0f;
    ndc__out[1] = ((y / h) * 2.0f) - 1.0f;
}

static void nd__screen_to_uv_u16(uint16_t *uv__out, float x, float y, float w, float h) {
    uv__out[0] = (uint16_t)((x / w) * 65535.0f);
    uv__out[1] = (uint16_t)((y / h) * 65535.0f);
}

static size_t nd__draw_queue_mem_size(int32_t cap) {
    return cap * ((size_t)(sizeof(nd__Draw_Entry) + sizeof(nd__Vertex_Data)));
}

static bool nd__draw_queue_init(nd__Draw_Queue *queue__out, size_t cap) {
    size_t alloc_size = nd__draw_queue_mem_size(cap);
    uint8_t *alloc_result = ND_REALLOC(NULL, alloc_size);
    if (!alloc_result) {
        return false;
    }
    memset(alloc_result, 0, alloc_size);

    queue__out->draw_entry_start = (nd__Draw_Entry *)alloc_result;
    queue__out->vertex_data_start = (nd__Vertex_Data *)(alloc_result + cap * sizeof(nd__Draw_Entry));
    queue__out->len = 0;
    queue__out->cap = cap;

    return true;
}

static bool nd__draw_queue_grow(nd__Draw_Queue *queue) {
    size_t new_cap = queue->cap * 2;
    ptrdiff_t old_vertex_data_offset = ((uint8_t *)queue->vertex_data_start) - ((uint8_t *)queue->draw_entry_start);
    size_t alloc_size = nd__draw_queue_mem_size(new_cap);
    uint8_t *alloc_result = ND_REALLOC(queue->draw_entry_start, alloc_size);
    if (!alloc_result) {
        return false;
    }

    queue->draw_entry_start = (nd__Draw_Entry *)alloc_result;
    queue->vertex_data_start = (nd__Vertex_Data *)(alloc_result + new_cap * sizeof(nd__Draw_Entry));

    memmove(queue->vertex_data_start, alloc_result + old_vertex_data_offset, queue->cap * sizeof(nd__Vertex_Data));
    memset(queue->draw_entry_start + queue->cap, 0, sizeof(nd__Draw_Entry) * (new_cap - queue->cap));

    queue->cap = new_cap;

    return true;
}

static const char *nd__vertex_source = "#version 330 core\n"
    "layout(location = 0) in vec2 vert_pos;\n"
    "layout(location = 1) in vec2 vert_uv;\n"
    "layout(location = 2) in vec4 vert_col;\n"
    "out vec2 frag_uv;\n"
    "out vec4 frag_col;\n"
    "void main() {\n"
    "    gl_Position = vec4(vert_pos, 0, 1);\n"
    "    frag_uv = vert_uv;\n"
    "    frag_col = vert_col;\n"
    "}";

static const char *nd__fragment_source = "#version 330 core\n"
    "in vec2 frag_uv;\n"
    "in vec4 frag_col;\n"
    "uniform sampler2D tex;\n"
    "out vec4 color;\n"
    "void main() {\n"
    "    color = frag_col + texture(tex, frag_uv);\n"
    "}";

NDDEF bool nd_init(
    int32_t viewport_w, int32_t viewport_h,
    nd_Load_Proc get_gl_proc_address
) {
    ND_GL_FUNCS(ND_GL_FUNCS_LOAD)

    uint32_t vao;
    uint32_t vbo;
    {
        nd__Vertex_Layout dummy_vertex = {0};

        nd__glGenVertexArrays(1, &vao);
        nd__glGenBuffers(1, &vbo);

        nd__glBindVertexArray(vao);

        nd__glBindBuffer(ND_GL_ARRAY_BUFFER, vbo);
        nd__glBufferData(ND_GL_ARRAY_BUFFER, sizeof(nd__Vertex_Layout), &dummy_vertex, ND_GL_STREAM_DRAW);

        nd__glEnableVertexAttribArray(0);
        nd__glVertexAttribPointer(0, 2, ND_GL_FLOAT, false, sizeof(nd__Vertex_Layout), (void *)0);

        nd__glEnableVertexAttribArray(1);
        nd__glVertexAttribPointer(1, 2, ND_GL_UNSIGNED_SHORT, true, sizeof(nd__Vertex_Layout), (void *)offsetof(nd__Vertex_Layout, u));

        nd__glEnableVertexAttribArray(2);
        nd__glVertexAttribPointer(2, 4, ND_GL_UNSIGNED_BYTE, true, sizeof(nd__Vertex_Layout), (void *)offsetof(nd__Vertex_Layout, c));
    }

    uint32_t shader_program;
    {
        uint32_t vertex_shader = nd__glCreateShader(ND_GL_VERTEX_SHADER);
        nd__glShaderSource(vertex_shader, 1, &nd__vertex_source, NULL);
        nd__glCompileShader(vertex_shader);

        uint32_t fragment_shader = nd__glCreateShader(ND_GL_FRAGMENT_SHADER);
        nd__glShaderSource(fragment_shader, 1, &nd__fragment_source, NULL);
        nd__glCompileShader(fragment_shader);

        shader_program = nd__glCreateProgram();
        nd__glAttachShader(shader_program, vertex_shader);
        nd__glAttachShader(shader_program, fragment_shader);
        nd__glLinkProgram(shader_program);

        nd__glUseProgram(shader_program);
        nd__glDeleteShader(vertex_shader);
        nd__glDeleteShader(fragment_shader);
    }

    uint32_t textureless_id;
    {
        const nd_Color c = ND_NO_COLOR;
        nd_Texture tex;
        nd_init_texture_from_image_buffer(&tex, (const uint8_t *)&c, 1, 1, ND_FILTER_NEAREST, ND_FILTER_NEAREST);
        textureless_id = tex.id;
    }

    nd__glViewport(0, 0, viewport_w, viewport_h);
    nd__glDisable(ND_GL_DEPTH_TEST);
    nd__glDisable(ND_GL_BLEND);

    nd__Draw_Queue dq = {0};
    if (!nd__draw_queue_init(&dq, ND_INITIAL_DRAW_QUEUE_CAP)) {
        return false;
    }

    nd__ctx.viewport_w = viewport_w;
    nd__ctx.viewport_h = viewport_h;
    nd__ctx.draw_queue = dq;
    nd__ctx.vao = vao;
    nd__ctx.vbo = vbo;
    nd__ctx.shader_program = shader_program;
    nd__ctx.textureless_id = textureless_id;
    nd__ctx.last_fb = 0;
    nd__ctx.last_tex = textureless_id;
    nd__ctx.last_blend = 0;

    return true;
}

NDDEF void nd_deinit(
    void
) {
    ND_FREE(nd__ctx.draw_queue.draw_entry_start);
    nd__ctx = (nd__Context){0};
}


NDDEF void nd_update_viewport(
    int32_t w, int32_t h
) {
    nd__ctx.viewport_w = w;
    nd__ctx.viewport_h = h;
}

NDDEF void nd_init_texture(
    nd_Texture *tex__out,
    int32_t w, int32_t h,
    nd_Texture_Filter min_filter, nd_Texture_Filter mag_filter
) {
    return nd_init_texture_from_image_buffer(tex__out, NULL, w, h, min_filter, mag_filter);
}

NDDEF void nd_deinit_texture(
    nd_Texture *tex
) {
    ND_ASSERT(nd__ctx.draw_queue.num_entries == 0 && "Do not deinit textures when drawing operations are queued");
    nd__glDeleteTextures(1, &tex->id);
    if (nd__ctx.last_tex == tex->id) {
        nd__ctx.last_tex = 0;
    }
    *tex = (nd_Texture){0};
}

NDDEF void nd_init_texture_from_image_buffer(
    nd_Texture *tex__out,
    const uint8_t *image_rgba8888,
    int32_t image_w, int32_t image_h,
    nd_Texture_Filter min_filter, nd_Texture_Filter mag_filter
) {
    int32_t min_fil, mag_fil;
    if (min_filter == ND_FILTER_NEAREST) {
        min_fil = ND_GL_NEAREST;
    } else {
        min_fil = ND_GL_LINEAR;
    }
    if (mag_filter == ND_FILTER_LINEAR) {
        mag_fil = ND_GL_NEAREST;
    } else {
        mag_fil = ND_GL_LINEAR;
    }
    uint32_t gpu_texture;
    nd__glGenTextures(1, &gpu_texture);
    nd__glBindTexture(ND_GL_TEXTURE_2D, gpu_texture);
    nd__ctx.last_tex = gpu_texture;
    nd__glTexParameteri(ND_GL_TEXTURE_2D, ND_GL_TEXTURE_WRAP_S, ND_GL_CLAMP_TO_EDGE);
    nd__glTexParameteri(ND_GL_TEXTURE_2D, ND_GL_TEXTURE_WRAP_T, ND_GL_CLAMP_TO_EDGE);
    nd__glTexParameteri(ND_GL_TEXTURE_2D, ND_GL_TEXTURE_MIN_FILTER, min_fil);
    nd__glTexParameteri(ND_GL_TEXTURE_2D, ND_GL_TEXTURE_MAG_FILTER, mag_fil);
    nd__glTexImage2D(ND_GL_TEXTURE_2D, 0, ND_GL_RGBA8, image_w, image_h, 0, ND_GL_RGBA, ND_GL_UNSIGNED_BYTE, image_rgba8888);
    tex__out->id = gpu_texture;
    tex__out->w = image_w;
    tex__out->h = image_h;
}

NDDEF void nd_texture_to_preallocated_image_buffer(
    uint8_t *image_rgba8888__out,
    nd_Texture tex
) {
    ND_ASSERT(nd__ctx.draw_queue.num_entries == 0 && "Do not read texture data back to cpu memory when drawing operations are queued");
    nd__glBindTexture(ND_GL_TEXTURE_2D, tex.id);
    nd__ctx.last_tex = tex.id;
    nd__glGetTexImage(ND_GL_TEXTURE_2D, 0, ND_GL_RGBA8, ND_GL_UNSIGNED_BYTE, image_rgba8888__out);
}

NDDEF bool nd_init_framebuffer_from_texture(
    nd_Framebuffer *fb__out,
    nd_Texture tex
) {
    uint32_t fbo;
    nd__glGenFramebuffers(1, &fbo);
    nd__glBindFramebuffer(ND_GL_FRAMEBUFFER, fbo);
    nd__ctx.last_fb = fbo;
    nd__glFramebufferTexture2D(ND_GL_FRAMEBUFFER, ND_GL_COLOR_ATTACHEMENT0, ND_GL_TEXTURE_2D, tex.id, 0);

    if (nd__glCheckFramebufferStatus(ND_GL_FRAMEBUFFER) != ND_GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    fb__out->id = fbo;
    fb__out->w = tex.w;
    fb__out->h = tex.h;
    return true;
}

NDDEF void nd_clear_framebuffer(
    nd_Framebuffer fb,
    nd_Color c
) {
    ND_ASSERT(nd__ctx.draw_queue.num_entries == 0 && "Do not clear framebuffers when drawing operations are queued");
    nd__glBindFramebuffer(ND_GL_FRAMEBUFFER, fb.id);
    nd__ctx.last_fb = fb.id;
    nd__glClearColor(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
    nd__glClear(ND_GL_COLOR_BUFFER_BIT);
}

NDDEF void nd_deinit_framebuffer(
    nd_Framebuffer *fb
) {
    ND_ASSERT(nd__ctx.draw_queue.num_entries == 0 && "Do not deinit framebuffers when drawing operations are queued");
    nd__glDeleteFramebuffers(1, &fb->id);
    if (nd__ctx.last_fb == fb->id) {
        nd__ctx.last_fb = 0;
    }
    *fb = (nd_Framebuffer){0};
}

NDDEF bool nd_submit_draw_queue(
    void
) {
    {
        intptr_t buffer_size_bytes = sizeof(nd__Vertex_Data) * nd__ctx.draw_queue.len;
        nd__glBufferData(ND_GL_ARRAY_BUFFER, buffer_size_bytes, NULL, ND_GL_STREAM_DRAW);
        void *gpu_mem = nd__glMapBufferRange(ND_GL_ARRAY_BUFFER, 0, buffer_size_bytes, ND_GL_MAP_WRITE_BIT | ND_GL_MAP_INVALIDATE_BUFFER_BIT | ND_GL_MAP_UNSYNCHRONIZED_BIT);
        memcpy(gpu_mem, nd__ctx.draw_queue.vertex_data_start, buffer_size_bytes);
        if (!nd__glUnmapBuffer(ND_GL_ARRAY_BUFFER)) {
            return false;
        }
    }

    nd__Draw_Entry *entry = nd__ctx.draw_queue.draw_entry_start;
    int32_t first_vertex = 0;
    int32_t num_vertices;
    int32_t i = 0;
    nd__glViewport(0, 0, nd__ctx.viewport_w, nd__ctx.viewport_h);
    while (i < nd__ctx.draw_queue.num_entries) {
        if (entry->data.fb.id != nd__ctx.last_fb) {
            nd__glBindFramebuffer(ND_GL_DRAW_FRAMEBUFFER, entry->data.fb.id);
            nd__ctx.last_fb = entry->data.fb.id;
            nd__glViewport(0, 0, entry->data.fb.w, entry->data.fb.h);
        }
        if (entry->data.tex_id != nd__ctx.last_tex) {
            nd__glBindTexture(ND_GL_TEXTURE_2D, entry->data.tex_id);
            nd__ctx.last_tex = entry->data.fb.id;
        }
        if (entry->data.blend != nd__ctx.last_blend) {
            switch (entry->data.blend) {
            case ND_BLEND_NONE:
                nd__glDisable(ND_GL_BLEND);
                break;
            case ND_BLEND_ALPHA:
                if (nd__ctx.last_blend == ND_BLEND_NONE) nd__glEnable(ND_GL_BLEND);
                nd__glBlendFunc(ND_GL_SRC_ALPHA, ND_GL_ONE_MINUS_SRC_ALPHA);
                break;
            case ND_BLEND_ALPHA_PRE_MUL:
                if (nd__ctx.last_blend == ND_BLEND_NONE) nd__glEnable(ND_GL_BLEND);
                nd__glBlendFunc(ND_GL_ONE, ND_GL_ONE_MINUS_SRC_ALPHA);
                break;
            case ND_BLEND_ADD:
                if (nd__ctx.last_blend == ND_BLEND_NONE) nd__glEnable(ND_GL_BLEND);
                nd__glBlendFunc(ND_GL_ONE, ND_GL_ONE);
                break;
            case ND_BLEND_ADD_PRE_ALPHA:
                if (nd__ctx.last_blend == ND_BLEND_NONE) nd__glEnable(ND_GL_BLEND);
                nd__glBlendFunc(ND_GL_SRC_ALPHA, ND_GL_ONE);
                break;
            case ND_BLEND_MUL:
                if (nd__ctx.last_blend == ND_BLEND_NONE) nd__glEnable(ND_GL_BLEND);
                nd__glBlendFunc(ND_GL_ZERO, ND_GL_SRC_COLOR);
                break;
            }
            nd__ctx.last_blend = entry->data.blend;
        }
        
        num_vertices = entry->count * 3;
        nd__glDrawArrays(ND_GL_TRIANGLES, first_vertex, num_vertices);
        first_vertex += num_vertices;
        entry->count = 0;
        ++i;
        ++entry;
    }

    nd__ctx.draw_queue.len = 0;
    nd__ctx.draw_queue.num_entries = 0;

    return true;
}

NDDEF void nd_draw_triangle(
    nd_Framebuffer fb,
    nd_Blend blend,
    float x1, float y1,
    float x2, float y2,
    float x3, float y3,
    nd_Color c
) {
    nd_Triangle_Data tri = (nd_Triangle_Data){
        x1, y1, 0, 0, c,
        x2, y2, 0, 0, c,
        x3, y3, 0, 0, c
    };
    nd_queue_triangles(
        fb,
        ND_TEXTURE_NULL,
        blend,
        &tri,
        1
    );
}

NDDEF void nd_draw_triangle_3color(
    nd_Framebuffer fb,
    nd_Blend blend,
    float x1, float y1, nd_Color c1,
    float x2, float y2, nd_Color c2,
    float x3, float y3, nd_Color c3
) {
    nd_Triangle_Data tri = (nd_Triangle_Data){
        x1, y1, 0, 0, c1,
        x2, y2, 0, 0, c2,
        x3, y3, 0, 0, c3
    };
    nd_queue_triangles(
        fb,
        ND_TEXTURE_NULL,
        blend,
        &tri,
        1
    );
}

NDDEF void nd_draw_triangle_textured(
    nd_Framebuffer fb,
    nd_Texture tex,
    nd_Blend blend,
    float x1, float y1, float tex_x1, float tex_y1,
    float x2, float y2, float tex_x2, float tex_y2,
    float x3, float y3, float tex_x3, float tex_y3
) {
    nd_Triangle_Data tri = (nd_Triangle_Data){
        x1, y1, tex_x1, tex_y1, ND_NO_COLOR,
        x2, y2, tex_x2, tex_y2, ND_NO_COLOR,
        x3, y3, tex_x3, tex_y3, ND_NO_COLOR
    };
    nd_queue_triangles(
        fb,
        tex,
        blend,
        &tri,
        1
    );
}

NDDEF void nd_draw_rectangle(
    nd_Framebuffer fb,
    nd_Blend blend,
    float min_x, float min_y,
    float w, float h,
    nd_Color c
) {
    nd_Triangle_Data quad[2];
    quad[0] = (nd_Triangle_Data){
        min_x, min_y, 0, 0, c,
        min_x, min_y + h, 0, 0, c,
        min_x + w, min_y, 0, 0, c
    };
    quad[1] = (nd_Triangle_Data){
        min_x + w, min_y, 0, 0, c,
        min_x, min_y + h, 0, 0, c,
        min_x + w, min_y + h, 0, 0, c
    };
    nd_queue_triangles(
        fb,
        ND_TEXTURE_NULL,
        blend,
        quad,
        2
    );
}

NDDEF void nd_draw_rectangle_4color(
    nd_Framebuffer fb,
    nd_Blend blend,
    float min_x, float min_y,
    float w, float h,
    nd_Color c1, nd_Color c2, nd_Color c3, nd_Color c4
) {
    nd_Triangle_Data quad[2];
    quad[0] = (nd_Triangle_Data){
        min_x, min_y, 0, 0, c1,
        min_x, min_y + h, 0, 0, c2,
        min_x + w, min_y, 0, 0, c4
    };
    quad[1] = (nd_Triangle_Data){
        min_x + w, min_y, 0, 0, c4,
        min_x, min_y + h, 0, 0, c2,
        min_x + w, min_y + h, 0, 0, c3
    };
    nd_queue_triangles(
        fb,
        ND_TEXTURE_NULL,
        blend,
        quad,
        2
    );
}

NDDEF void nd_draw_rectangle_textured(
    nd_Framebuffer fb,
    nd_Texture tex,
    nd_Blend blend,
    float min_x, float min_y,
    float w, float h,
    float tex_min_x, float tex_min_y,
    float tex_w, float tex_h
) {
    nd_Triangle_Data quad[2];
    quad[0] = (nd_Triangle_Data){
        min_x, min_y, tex_min_x, tex_min_y, ND_NO_COLOR,
        min_x, min_y + h, tex_min_x, tex_min_y + tex_h, ND_NO_COLOR,
        min_x + w, min_y, tex_min_x + tex_w, tex_min_y, ND_NO_COLOR
    };
    quad[1] = (nd_Triangle_Data){
        min_x + w, min_y, tex_min_x + tex_w, tex_min_y, ND_NO_COLOR,
        min_x, min_y + h, tex_min_x, tex_min_y + tex_h, ND_NO_COLOR,
        min_x + w, min_y + h, tex_min_x + tex_w, tex_min_y + tex_h, ND_NO_COLOR
    };
    nd_queue_triangles(
        fb,
        tex,
        blend,
        quad,
        2
    );
}

NDDEF void nd_draw_texture(
    nd_Framebuffer fb,
    nd_Texture tex,
    nd_Blend blend,
    float min_x, float min_y
) {
    nd_Triangle_Data quad[2];
    quad[0] = (nd_Triangle_Data){
        min_x, min_y, 0, 0, ND_NO_COLOR,
        min_x, min_y + tex.h, 0, tex.h, ND_NO_COLOR,
        min_x + tex.w, min_y, tex.w, 0, ND_NO_COLOR
    };
    quad[1] = (nd_Triangle_Data){
        min_x + tex.w, min_y, tex.w, 0, ND_NO_COLOR,
        min_x, min_y + tex.h, 0, tex.h, ND_NO_COLOR,
        min_x + tex.w, min_y + tex.h, tex.w, tex.h, ND_NO_COLOR
    };
    nd_queue_triangles(
        fb,
        tex,
        blend,
        quad,
        2
    );
}

NDDEF void nd_init_image_font_from_image_buffer(
    nd_Image_Font *font__out,
    const uint8_t *image_rgba8888,
    int32_t image_w, int32_t image_h,
    nd_Color separator_color,
    int character_spacing,
    nd_Texture_Filter min_filter, nd_Texture_Filter mag_filter
) {
    char code = ' ';
    int code_xmin = 0;
    int code_width = 0;
    const nd_Color *pixels = (nd_Color *)image_rgba8888;
    nd_Color c;
    for (int i = 0; i < image_w && code <= '~'; ++i) {
        c = pixels[i];
        if (!memcmp(&c, &separator_color, sizeof(Color))) {
            font__out->runes[(int)code] = (nd__Rune_Descriptor){code_xmin, code_width};
            ++code;
            code_xmin = i + 1;
            code_width = 0;
        } else {
            ++code_width;
        }
    }

    if (code == '~') {
        font__out->runes[(int)code] = (nd__Rune_Descriptor){code_xmin, code_width};
    }

    nd_init_texture_from_image_buffer(&font__out->tex, image_w, image_h, min_filter, mag_filter);
    font__out->character_spacing = character_spacing;
}

NDDEF void nd_deinit_image_font(
    nd_Image_Font *font
) {
    nd_deinit_texture(font->tex);
    *font = (nd_Image_Font){0};
}

NDDEF void nd_draw_string_from_image_font(
    nd_Framebuffer fb,
    nd_Blend blend,
    nd_Image_Font font,
    unsigned int scale,
    float min_x, float min_y,
    const char *string
) {
    nd_draw_string_from_image_font_len(
        fb,
        vlend,
        font,
        scale,
        min_x, min_y,
        string,
        strlen(string)
    );
}

#define ND__DRAW_STRING_BUF_LEN
NDDEF void nd_draw_string_from_image_font_len(
    nd_Framebuffer fb,
    nd_Blend blend,
    nd_Image_Font font,
    unsigned int scale,
    float min_x, float min_y,
    const char *string,
    size_t len
) {
    if (font.tex.id == 0) {
        font = nd_default_image_font;
    }
    nd_Triangle_Data [ND__DRAW_STRING_BUF_LEN]tri_buf;
    nd_Triangle_Data temp;
    temp.c1 = ND_NO_COLOR;
    temp.c2 = ND_NO_COLOR;
    temp.c3 = ND_NO_COLOR;
    uint32_t queued_tris = 0;
    char code;
    float tex_height = font.tex.h;
    float max_y = min_y + scale * font.tex.h;
    for (size_t i = 0; i < len; ++i) {
        code = string[i];
        ND_ASSERT(code >= ' ' && code <= '~' && "can only draw ascii printable characters from nd_ImageFont");
        
        float tex_min_x = font.runes[code].xmin;
        float tex_width = font.runes[code].width;
        float fb_width = tex_width * scale;
        temp.x1 = min_x;
        temp.y1 = min_y;
        temp.tex_x1 = tex_min_x;
        temp.tex_y1 = 0;

        temp.x2 = min_x;
        temp.y2 = max_y;
        temp.tex_x2 = tex_min_x;
        temp.tex_y2 = tex_height;

        temp.x3 = min_x + fb_width;
        temp.y3 = min_y;
        temp.tex_x3 = tex_min_x + tex_width;
        temp.tex_y3 = 0;

        memcpy(tri_buf + queued_tris, &temp, sizeof(nd_Triangle_Data));

        temp.x1 = min_x + fb_width;
        temp.y1 = min_y;
        temp.tex_x1 = tex_min_x + tex_width;
        temp.tex_y1 = 0;

        temp.x2 = min_x;
        temp.y2 = max_y;
        temp.tex_x2 = tex_min_x;
        temp.tex_y2 = tex_height;

        temp.x3 = min_x + fb_width;
        temp.y3 = max_y;
        temp.tex_x3 = tex_min_x + tex_width;
        temp.tex_y3 = tex_height;

        memcpy(tri_buf + queued_tris + 1, &temp, sizeof(nd_Triangle_Data));

        queued_tris += 2;
        if (queued_tris == ND__DRAW_STRING_BUF_LEN) {
            nd_queue_triangles(
                fb,
                font.tex,
                blend,
                tri_buf,
                queued_tris
            );
            queued_tris = 0;
        }

        min_x += fb_width + font.character_spacing * scale;
    }

    if (queued_tris != 0) {
        nd_queue_triangles(
            fb,
            font.tex,
            blend,
            tri_buf,
            queued_tris
        );
    }
}

NDDEF size_t nd_measure_string_from_image_font(
    nd_Image_Font font,
    unsigned int scale,
    const char *string,
) {
    nd_measure_string_from_image_font_len(
        font,
        scale,
        string,
        strlen(string)
    );
}

NDDEF size_t nd_measure_string_from_image_font_len(
    nd_Image_Font font,
    unsigned int scale,
    const char *string,
    size_t len
) {
    size_t width;
    for (size_t i = 0; i < len; ++i) {
        char code = string[i];
        ND_ASSERT(code >= ' ' && code <= '~' && "can only draw ascii printable characters from nd_ImageFont");
        width += (font.runes[(int)code].width + font.character_spacing)* scale;
    }
    width -= character_spacing * scale;

    return width;
}

#define ND__NDC_DISPATCH(NDC_FUNC) \
     NDC_FUNC(tri_data->ndc1, triangles->x1, triangles->y1, new_draw_data.fb.w, new_draw_data.fb.h); \
     NDC_FUNC(tri_data->ndc2, triangles->x2, triangles->y2, new_draw_data.fb.w, new_draw_data.fb.h); \
     NDC_FUNC(tri_data->ndc3, triangles->x3, triangles->y3, new_draw_data.fb.w, new_draw_data.fb.h); \
     nd__screen_to_uv_u16(tri_data->uv1, triangles->tex_x1, triangles->tex_y1, tex.w, tex.h); \
     nd__screen_to_uv_u16(tri_data->uv2, triangles->tex_x2, triangles->tex_y2, tex.w, tex.h); \
     nd__screen_to_uv_u16(tri_data->uv3, triangles->tex_x3, triangles->tex_y3, tex.w, tex.h); \
     tri_data->c1 = triangles->c1; \
     tri_data->c2 = triangles->c2; \
     tri_data->c3 = triangles->c3; \
     ++tri_data; \
     ++triangles; \
     ++i; 

NDDEF void nd_queue_triangles(
    nd_Framebuffer fb,
    nd_Texture tex,
    nd_Blend blend,
    nd_Triangle_Data *triangles,
    uint32_t num_triangles
) {
    int32_t next_len = nd__ctx.draw_queue.len + num_triangles;
    bool ok;
    while (next_len > nd__ctx.draw_queue.cap) {
        ok = nd__draw_queue_grow(&nd__ctx.draw_queue);
        ND_ASSERT(ok && "memory allocation failure");
    }

    if (tex.id == 0) {
        tex.id = nd__ctx.textureless_id;
        tex.w = 1;
        tex.h = 1;
    }

    nd__Draw_Data new_draw_data = {0};
    new_draw_data.fb = fb;
    if (fb.id == 0) {
        new_draw_data.fb.w = nd__ctx.viewport_w;
        new_draw_data.fb.h = nd__ctx.viewport_h;
    }
    new_draw_data.tex_id = tex.id;
    new_draw_data.blend = blend;

    nd__Draw_Entry *draw_entry;
    
    if (nd__ctx.draw_queue.num_entries == 0) {
        draw_entry = nd__ctx.draw_queue.draw_entry_start;
        draw_entry->data = new_draw_data;
        draw_entry->count = num_triangles;
        nd__ctx.draw_queue.num_entries = 1;
    } else {
        draw_entry = nd__ctx.draw_queue.draw_entry_start + nd__ctx.draw_queue.num_entries - 1;
        if (memcmp(draw_entry, &new_draw_data, sizeof(nd__Draw_Data)) == 0) {
            draw_entry->count += num_triangles;
        } else {
            ++draw_entry;
            draw_entry->data = new_draw_data;
            draw_entry->count = num_triangles;
            ++nd__ctx.draw_queue.num_entries;
        }
    }

    nd__Vertex_Data *tri_data = nd__ctx.draw_queue.vertex_data_start + (nd__ctx.draw_queue.len);

    uint32_t i = 0;
    if (fb.id == 0) {
        while (i < num_triangles) {
            ND__NDC_DISPATCH(nd__screen_to_ndc_yflip)
        }
    } else {
        while (i < num_triangles) {
            ND__NDC_DISPATCH(nd__screen_to_ndc_no_yflip)
        }
    }

    nd__ctx.draw_queue.len += num_triangles;
}

#endif // #ifdef NANODRAW_IMPLEMENTATION

/*
Permission to use, copy, modify, and/or distribute this software for
any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
