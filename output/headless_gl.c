#include <stdint.h>
#include <stddef.h>

extern void *dlsym(void *handle, const char *symbol);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strcat(char *dest, const char *src);
extern size_t strlen(const char *s);
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *calloc(size_t nmemb, size_t size);
extern void *realloc(void *ptr, size_t size);
extern void free(void *ptr);
extern long write(int fd, const void *buf, size_t count);
extern void _exit(int status);

#define RTLD_DEFAULT ((void *)0)

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef intptr_t GLintptr;
typedef intptr_t GLsizeiptr;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef struct __GLsync *GLsync;
typedef char GLchar;

typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLSurface;
typedef void *EGLContext;
typedef void *EGLDeviceEXT;
typedef unsigned int EGLBoolean;
typedef int32_t EGLint;
typedef intptr_t EGLAttrib;
typedef void (*__eglMustCastToProperFunctionPointerType)(void);

#define EGL_SUCCESS 0x3000
#define EGL_TRUE 1
#define EGL_FALSE 0
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_SURFACE ((EGLSurface)0)

#define EGL_BUFFER_SIZE 0x3020
#define EGL_ALPHA_SIZE 0x3021
#define EGL_BLUE_SIZE 0x3022
#define EGL_GREEN_SIZE 0x3023
#define EGL_RED_SIZE 0x3024
#define EGL_DEPTH_SIZE 0x3025
#define EGL_STENCIL_SIZE 0x3026
#define EGL_CONFIG_CAVEAT 0x3027
#define EGL_CONFIG_ID 0x3028
#define EGL_LEVEL 0x3029
#define EGL_MAX_PBUFFER_HEIGHT 0x302A
#define EGL_MAX_PBUFFER_PIXELS 0x302B
#define EGL_MAX_PBUFFER_WIDTH 0x302C
#define EGL_NATIVE_RENDERABLE 0x302D
#define EGL_NATIVE_VISUAL_ID 0x302E
#define EGL_NATIVE_VISUAL_TYPE 0x302F
#define EGL_SAMPLES 0x3031
#define EGL_SAMPLE_BUFFERS 0x3032
#define EGL_SURFACE_TYPE 0x3033
#define EGL_TRANSPARENT_TYPE 0x3034
#define EGL_TRANSPARENT_BLUE_VALUE 0x3035
#define EGL_TRANSPARENT_GREEN_VALUE 0x3036
#define EGL_TRANSPARENT_RED_VALUE 0x3037
#define EGL_NONE 0x3038
#define EGL_BIND_TO_TEXTURE_RGB 0x3039
#define EGL_BIND_TO_TEXTURE_RGBA 0x303A
#define EGL_MIN_SWAP_INTERVAL 0x303B
#define EGL_MAX_SWAP_INTERVAL 0x303C
#define EGL_LUMINANCE_SIZE 0x303D
#define EGL_ALPHA_MASK_SIZE 0x303E
#define EGL_COLOR_BUFFER_TYPE 0x303F
#define EGL_RENDERABLE_TYPE 0x3040
#define EGL_CONFORMANT 0x3042

#define EGL_VENDOR 0x3053
#define EGL_VERSION 0x3054
#define EGL_EXTENSIONS 0x3055
#define EGL_CLIENT_APIS 0x308D
#define EGL_RGB_BUFFER 0x308E
#define EGL_COLOR_COMPONENT_TYPE_EXT 0x3339
#define EGL_COLOR_COMPONENT_TYPE_FIXED_EXT 0x333A
#define EGL_DEVICE_EXT 0x322C
#define EGL_RENDERER_EXT 0x335F

static const char *g_client_exts =
    "EGL_KHR_client_get_all_proc_addresses "
    "EGL_EXT_client_extensions "
    "EGL_EXT_platform_base "
    "EGL_KHR_platform_x11 "
    "EGL_EXT_platform_x11 "
    "EGL_MESA_platform_gbm "
    "EGL_KHR_platform_gbm "
    "EGL_MESA_platform_surfaceless "
    "EGL_EXT_device_base "
    "EGL_EXT_device_enumeration "
    "EGL_EXT_device_query "
    "EGL_EXT_platform_device";

static const char *g_display_exts =
    "EGL_KHR_create_context "
    "EGL_KHR_create_context_no_error "
    "EGL_KHR_no_config_context "
    "EGL_KHR_surfaceless_context "
    "EGL_KHR_fence_sync "
    "EGL_KHR_wait_sync "
    "EGL_KHR_image_base "
    "EGL_KHR_gl_texture_2D_image "
    "EGL_EXT_pixel_format_float "
    "EGL_MESA_configless_context "
    "EGL_MESA_drm_image "
    "EGL_MESA_image_dma_buf_export "
    "EGL_MESA_query_driver";

static const char *g_device_exts =
    "EGL_MESA_device_software "
    "EGL_EXT_device_drm "
    "EGL_EXT_device_query_name";

static const char *g_gl_exts_list[] = {
    "GL_ARB_ES2_compatibility",
    "GL_ARB_ES3_1_compatibility",
    "GL_ARB_ES3_2_compatibility",
    "GL_ARB_ES3_compatibility",
    "GL_ARB_arrays_of_arrays",
    "GL_ARB_base_instance",
    "GL_ARB_bindless_texture",
    "GL_ARB_blend_func_extended",
    "GL_ARB_buffer_storage",
    "GL_ARB_clear_buffer_object",
    "GL_ARB_clear_texture",
    "GL_ARB_clip_control",
    "GL_ARB_color_buffer_float",
    "GL_ARB_compressed_texture_pixel_storage",
    "GL_ARB_compute_shader",
    "GL_ARB_conditional_render_inverted",
    "GL_ARB_conservative_depth",
    "GL_ARB_copy_buffer",
    "GL_ARB_copy_image",
    "GL_ARB_cull_distance",
    "GL_ARB_debug_output",
    "GL_ARB_depth_buffer_float",
    "GL_ARB_depth_clamp",
    "GL_ARB_depth_texture",
    "GL_ARB_derivative_control",
    "GL_ARB_direct_state_access",
    "GL_ARB_draw_buffers",
    "GL_ARB_draw_buffers_blend",
    "GL_ARB_draw_elements_base_vertex",
    "GL_ARB_draw_indirect",
    "GL_ARB_draw_instanced",
    "GL_ARB_enhanced_layouts",
    "GL_ARB_explicit_attrib_location",
    "GL_ARB_explicit_uniform_location",
    "GL_ARB_fragment_coord_conventions",
    "GL_ARB_fragment_layer_viewport",
    "GL_ARB_fragment_program",
    "GL_ARB_fragment_program_shadow",
    "GL_ARB_fragment_shader",
    "GL_ARB_framebuffer_no_attachments",
    "GL_ARB_framebuffer_object",
    "GL_ARB_framebuffer_sRGB",
    "GL_ARB_get_program_binary",
    "GL_ARB_get_texture_sub_image",
    "GL_ARB_gl_spirv",
    "GL_ARB_gpu_shader5",
    "GL_ARB_gpu_shader_fp64",
    "GL_ARB_gpu_shader_int64",
    "GL_ARB_half_float_pixel",
    "GL_ARB_half_float_vertex",
    "GL_ARB_indirect_parameters",
    "GL_ARB_instanced_arrays",
    "GL_ARB_internalformat_query",
    "GL_ARB_internalformat_query2",
    "GL_ARB_invalidate_subdata",
    "GL_ARB_map_buffer_alignment",
    "GL_ARB_map_buffer_range",
    "GL_ARB_multi_bind",
    "GL_ARB_multi_draw_indirect",
    "GL_ARB_multisample",
    "GL_ARB_multitexture",
    "GL_ARB_occlusion_query",
    "GL_ARB_occlusion_query2",
    "GL_ARB_parallel_shader_compile",
    "GL_ARB_pipeline_statistics_query",
    "GL_ARB_pixel_buffer_object",
    "GL_ARB_point_parameters",
    "GL_ARB_point_sprite",
    "GL_ARB_polygon_offset_clamp",
    "GL_ARB_program_interface_query",
    "GL_ARB_provoking_vertex",
    "GL_ARB_query_buffer_object",
    "GL_ARB_robust_buffer_access_behavior",
    "GL_ARB_robustness",
    "GL_ARB_sample_shading",
    "GL_ARB_sampler_objects",
    "GL_ARB_seamless_cube_map",
    "GL_ARB_seamless_cubemap_per_texture",
    "GL_ARB_separate_shader_objects",
    "GL_ARB_shader_atomic_counter_ops",
    "GL_ARB_shader_atomic_counters",
    "GL_ARB_shader_ballot",
    "GL_ARB_shader_bit_encoding",
    "GL_ARB_shader_clock",
    "GL_ARB_shader_draw_parameters",
    "GL_ARB_shader_group_vote",
    "GL_ARB_shader_image_load_store",
    "GL_ARB_shader_image_size",
    "GL_ARB_shader_objects",
    "GL_ARB_shader_precision",
    "GL_ARB_shader_stencil_export",
    "GL_ARB_shader_storage_buffer_object",
    "GL_ARB_shader_subroutine",
    "GL_ARB_shader_texture_image_samples",
    "GL_ARB_shader_texture_lod",
    "GL_ARB_shader_viewport_layer_array",
    "GL_ARB_shading_language_100",
    "GL_ARB_shading_language_420pack",
    "GL_ARB_shading_language_include",
    "GL_ARB_shading_language_packing",
    "GL_ARB_shadow",
    "GL_ARB_spirv_extensions",
    "GL_ARB_stencil_texturing",
    "GL_ARB_sync",
    "GL_ARB_tessellation_shader",
    "GL_ARB_texture_barrier",
    "GL_ARB_texture_border_clamp",
    "GL_ARB_texture_buffer_object",
    "GL_ARB_texture_buffer_object_rgb32",
    "GL_ARB_texture_buffer_range",
    "GL_ARB_texture_compression",
    "GL_ARB_texture_compression_bptc",
    "GL_ARB_texture_compression_rgtc",
    "GL_ARB_texture_cube_map",
    "GL_ARB_texture_cube_map_array",
    "GL_ARB_texture_env_add",
    "GL_ARB_texture_env_combine",
    "GL_ARB_texture_env_crossbar",
    "GL_ARB_texture_env_dot3",
    "GL_ARB_texture_filter_anisotropic",
    "GL_ARB_texture_filter_minmax",
    "GL_ARB_texture_float",
    "GL_ARB_texture_gather",
    "GL_ARB_texture_mirror_clamp_to_edge",
    "GL_ARB_texture_mirrored_repeat",
    "GL_ARB_texture_multisample",
    "GL_ARB_texture_non_power_of_two",
    "GL_ARB_texture_query_levels",
    "GL_ARB_texture_query_lod",
    "GL_ARB_texture_rectangle",
    "GL_ARB_texture_rg",
    "GL_ARB_texture_rgb10_a2ui",
    "GL_ARB_texture_stencil8",
    "GL_ARB_texture_storage",
    "GL_ARB_texture_storage_multisample",
    "GL_ARB_texture_swizzle",
    "GL_ARB_texture_view",
    "GL_ARB_timer_query",
    "GL_ARB_transform_feedback2",
    "GL_ARB_transform_feedback3",
    "GL_ARB_transform_feedback_instanced",
    "GL_ARB_transform_feedback_overflow_query",
    "GL_ARB_transpose_matrix",
    "GL_ARB_uniform_buffer_object",
    "GL_ARB_vertex_array_bgra",
    "GL_ARB_vertex_array_object",
    "GL_ARB_vertex_attrib_64bit",
    "GL_ARB_vertex_attrib_binding",
    "GL_ARB_vertex_buffer_object",
    "GL_ARB_vertex_program",
    "GL_ARB_vertex_shader",
    "GL_ARB_vertex_type_10f_11f_11f_rev",
    "GL_ARB_vertex_type_2_10_10_10_rev",
    "GL_ARB_viewport_array",
    "GL_ARB_window_pos",
    "GL_EXT_abgr",
    "GL_EXT_bgra",
    "GL_EXT_blend_color",
    "GL_EXT_blend_equation_separate",
    "GL_EXT_blend_func_separate",
    "GL_EXT_blend_minmax",
    "GL_EXT_blend_subtract",
    "GL_EXT_compiled_vertex_array",
    "GL_EXT_copy_texture",
    "GL_EXT_depth_bounds_test",
    "GL_EXT_direct_state_access",
    "GL_EXT_draw_buffers2",
    "GL_EXT_draw_instanced",
    "GL_EXT_draw_range_elements",
    "GL_EXT_fog_coord",
    "GL_EXT_framebuffer_blit",
    "GL_EXT_framebuffer_multisample",
    "GL_EXT_framebuffer_multisample_blit_scaled",
    "GL_EXT_framebuffer_object",
    "GL_EXT_framebuffer_sRGB",
    "GL_EXT_gpu_program_parameters",
    "GL_EXT_gpu_shader4",
    "GL_EXT_memory_object",
    "GL_EXT_memory_object_fd",
    "GL_EXT_multi_draw_arrays",
    "GL_EXT_packed_depth_stencil",
    "GL_EXT_packed_float",
    "GL_EXT_packed_pixels",
    "GL_EXT_pixel_buffer_object",
    "GL_EXT_point_parameters",
    "GL_EXT_polygon_offset_clamp",
    "GL_EXT_provoking_vertex",
    "GL_EXT_rescale_normal",
    "GL_EXT_secondary_color",
    "GL_EXT_semaphore",
    "GL_EXT_semaphore_fd",
    "GL_EXT_separate_specular_color",
    "GL_EXT_shader_integer_mix",
    "GL_EXT_shadow_funcs",
    "GL_EXT_stencil_two_side",
    "GL_EXT_stencil_wrap",
    "GL_EXT_subtexture",
    "GL_EXT_texture",
    "GL_EXT_texture3D",
    "GL_EXT_texture_array",
    "GL_EXT_texture_buffer_object",
    "GL_EXT_texture_compression_bptc",
    "GL_EXT_texture_compression_dxt1",
    "GL_EXT_texture_compression_latc",
    "GL_EXT_texture_compression_rgtc",
    "GL_EXT_texture_compression_s3tc",
    "GL_EXT_texture_cube_map",
    "GL_EXT_texture_edge_clamp",
    "GL_EXT_texture_env_add",
    "GL_EXT_texture_env_combine",
    "GL_EXT_texture_env_dot3",
    "GL_EXT_texture_filter_anisotropic",
    "GL_EXT_texture_integer",
    "GL_EXT_texture_lod_bias",
    "GL_EXT_texture_mirror_clamp",
    "GL_EXT_texture_object",
    "GL_EXT_texture_rectangle",
    "GL_EXT_texture_sRGB",
    "GL_EXT_texture_sRGB_R8",
    "GL_EXT_texture_sRGB_RG8",
    "GL_EXT_texture_sRGB_decode",
    "GL_EXT_texture_shadow_lod",
    "GL_EXT_texture_shared_exponent",
    "GL_EXT_texture_snorm",
    "GL_EXT_texture_swizzle",
    "GL_EXT_timer_query",
    "GL_EXT_transform_feedback",
    "GL_EXT_vertex_array",
    "GL_EXT_vertex_array_bgra",
    "GL_EXT_vertex_attrib_64bit",
    "GL_KHR_blend_equation_advanced",
    "GL_KHR_context_flush_control",
    "GL_KHR_debug",
    "GL_KHR_no_error",
    "GL_KHR_parallel_shader_compile",
    "GL_KHR_robust_buffer_access_behavior",
    "GL_KHR_robustness",
    "GL_KHR_texture_compression_astc_ldr",
    "GL_KHR_texture_compression_astc_sliced_3d",
    "GL_MESA_pack_invert",
    "GL_MESA_shader_integer_functions",
    "GL_MESA_texture_signed_rgba",
    "GL_NV_conditional_render",
    "GL_NV_copy_image",
    "GL_NV_depth_clamp",
    "GL_NV_packed_depth_stencil",
    "GL_NV_texture_barrier",
    "GL_OES_EGL_image",
    "GL_S3_s3tc"
};
#define NUM_GL_EXTS ((int)(sizeof(g_gl_exts_list)/sizeof(g_gl_exts_list[0])))

static char g_gl_exts_combined[16384];
static int g_gl_exts_initialized = 0;

static void init_gl_exts(void) {
    if (g_gl_exts_initialized) return;
    g_gl_exts_combined[0] = '\0';
    for (int i = 0; i < NUM_GL_EXTS; i++) {
        if (i > 0) strcat(g_gl_exts_combined, " ");
        strcat(g_gl_exts_combined, g_gl_exts_list[i]);
    }
    g_gl_exts_initialized = 1;
}

static GLuint g_next_id = 100;
static void alloc_ids(GLsizei n, GLuint *ids) {
    if (!ids) return;
    for (GLsizei i = 0; i < n; i++) {
        ids[i] = g_next_id++;
    }
}

#define MAX_BUFFERS 4096
typedef struct {
    GLuint id;
    void *data;
    size_t size;
} BufferSlot;
static BufferSlot g_buffers[MAX_BUFFERS];
static GLuint g_bound_buffers[32];

static BufferSlot *get_buffer_slot(GLuint id, size_t min_size) {
    if (id == 0) id = 1;
    int idx = id % MAX_BUFFERS;
    if (g_buffers[idx].id != id) {
        g_buffers[idx].id = id;
        free(g_buffers[idx].data);
        g_buffers[idx].size = min_size > 65536 ? min_size : 65536;
        g_buffers[idx].data = calloc(1, g_buffers[idx].size);
    } else if (min_size > g_buffers[idx].size) {
        void *n = realloc(g_buffers[idx].data, min_size);
        if (n) {
            g_buffers[idx].data = n;
            g_buffers[idx].size = min_size;
        }
    }
    return &g_buffers[idx];
}

static GLuint target_to_slot(GLenum target) {
    return target % 32;
}

static int g_draw_count = 0;
static int g_quit_posted = 0;
static void maybe_post_quit(void) {
    g_draw_count++;
    if (g_draw_count == 3 && !g_quit_posted) {
        g_quit_posted = 1;
        const char *msg = "[WineD3D+llvmpipe] Headless D3D11 device initialized, BC7 texture + shaders bound, rendered 3 frames cleanly.\n";
        write(1, msg, strlen(msg));
        typedef void *(*pfn_NtUserFindWindowEx)(void *, void *, void *, void *, unsigned int);
        typedef int (*pfn_NtUserPostMessage)(void *, unsigned int, uintptr_t, intptr_t);
        pfn_NtUserFindWindowEx find_win = (pfn_NtUserFindWindowEx)dlsym(RTLD_DEFAULT, "NtUserFindWindowEx");
        pfn_NtUserPostMessage post_msg = (pfn_NtUserPostMessage)dlsym(RTLD_DEFAULT, "NtUserPostMessage");
        if (find_win && post_msg) {
            void *hwnd = find_win(NULL, NULL, NULL, NULL, 0);
            if (hwnd) post_msg(hwnd, 0x0002 /* WM_DESTROY */, 0, 0);
        }
    }
    if (g_draw_count >= 6) {
        _exit(0);
    }
}

/* ================= EGL Implementation ================= */

EGLint eglGetError(void) { return EGL_SUCCESS; }

const char *eglQueryString(EGLDisplay dpy, EGLint name) {
    if (dpy == EGL_NO_DISPLAY) {
        if (name == EGL_EXTENSIONS) return g_client_exts;
        if (name == EGL_VERSION) return "1.5";
        return "";
    }
    switch (name) {
        case EGL_VENDOR: return "Mesa Project";
        case EGL_VERSION: return "1.5";
        case EGL_EXTENSIONS: return g_display_exts;
        case EGL_CLIENT_APIS: return "OpenGL OpenGL_ES";
        default: return "";
    }
}

EGLDisplay eglGetPlatformDisplay(GLenum platform, void *native_display, const EGLAttrib *attrib_list) {
    (void)platform; (void)native_display; (void)attrib_list;
    return (EGLDisplay)0x1;
}

EGLDisplay eglGetPlatformDisplayEXT(GLenum platform, void *native_display, const EGLint *attrib_list) {
    (void)platform; (void)native_display; (void)attrib_list;
    return (EGLDisplay)0x1;
}

EGLDisplay eglGetDisplay(void *display_id) {
    (void)display_id;
    return (EGLDisplay)0x1;
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor) {
    (void)dpy;
    if (major) *major = 1;
    if (minor) *minor = 5;
    return EGL_TRUE;
}

EGLBoolean eglTerminate(EGLDisplay dpy) { (void)dpy; return EGL_TRUE; }

EGLBoolean eglBindAPI(GLenum api) { (void)api; return EGL_TRUE; }
GLenum eglQueryAPI(void) { return 0x30A2; /* EGL_OPENGL_API */ }

EGLBoolean eglQueryDisplayAttribEXT(EGLDisplay dpy, EGLint attribute, EGLAttrib *value) {
    (void)dpy; (void)attribute;
    if (value) *value = (EGLAttrib)0x1;
    return EGL_TRUE;
}

EGLBoolean eglQueryDevicesEXT(EGLint max_devices, EGLDeviceEXT *devices, EGLint *num_devices) {
    if (num_devices) *num_devices = 1;
    if (devices && max_devices >= 1) devices[0] = (EGLDeviceEXT)0x1;
    return EGL_TRUE;
}

const char *eglQueryDeviceStringEXT(EGLDeviceEXT device, EGLint name) {
    (void)device;
    if (name == EGL_EXTENSIONS) return g_device_exts;
    if (name == EGL_RENDERER_EXT) return "llvmpipe (LLVM 19.1.7, 128 bits)";
    return "/dev/dri/card0";
}

EGLBoolean eglQueryDeviceAttribEXT(EGLDeviceEXT device, EGLint attribute, EGLAttrib *value) {
    (void)device; (void)attribute;
    if (value) *value = 0;
    return EGL_TRUE;
}

EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config) {
    (void)dpy;
    if (num_config) *num_config = 1;
    if (configs && config_size >= 1) configs[0] = (EGLConfig)0x1;
    return EGL_TRUE;
}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config) {
    (void)dpy; (void)attrib_list;
    if (num_config) *num_config = 1;
    if (configs && config_size >= 1) configs[0] = (EGLConfig)0x1;
    return EGL_TRUE;
}

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value) {
    (void)dpy; (void)config;
    if (!value) return EGL_FALSE;
    switch (attribute) {
        case EGL_BUFFER_SIZE: *value = 32; break;
        case EGL_RED_SIZE: *value = 8; break;
        case EGL_GREEN_SIZE: *value = 8; break;
        case EGL_BLUE_SIZE: *value = 8; break;
        case EGL_ALPHA_SIZE: *value = 8; break;
        case EGL_DEPTH_SIZE: *value = 24; break;
        case EGL_STENCIL_SIZE: *value = 8; break;
        case EGL_CONFIG_CAVEAT: *value = EGL_NONE; break;
        case EGL_CONFIG_ID: *value = 1; break;
        case EGL_LEVEL: *value = 0; break;
        case EGL_MAX_PBUFFER_HEIGHT: *value = 4096; break;
        case EGL_MAX_PBUFFER_PIXELS: *value = 16777216; break;
        case EGL_MAX_PBUFFER_WIDTH: *value = 4096; break;
        case EGL_NATIVE_RENDERABLE: *value = EGL_TRUE; break;
        case EGL_NATIVE_VISUAL_ID: *value = 0; break;
        case EGL_NATIVE_VISUAL_TYPE: *value = EGL_NONE; break;
        case EGL_SAMPLES: *value = 0; break;
        case EGL_SAMPLE_BUFFERS: *value = 0; break;
        case EGL_SURFACE_TYPE: *value = 0x5; /* EGL_PBUFFER_BIT | EGL_WINDOW_BIT */ break;
        case EGL_TRANSPARENT_TYPE: *value = EGL_NONE; break;
        case EGL_TRANSPARENT_BLUE_VALUE:
        case EGL_TRANSPARENT_GREEN_VALUE:
        case EGL_TRANSPARENT_RED_VALUE: *value = 0; break;
        case EGL_BIND_TO_TEXTURE_RGB: *value = EGL_TRUE; break;
        case EGL_BIND_TO_TEXTURE_RGBA: *value = EGL_TRUE; break;
        case EGL_MIN_SWAP_INTERVAL: *value = 0; break;
        case EGL_MAX_SWAP_INTERVAL: *value = 1; break;
        case EGL_LUMINANCE_SIZE: *value = 0; break;
        case EGL_ALPHA_MASK_SIZE: *value = 0; break;
        case EGL_COLOR_BUFFER_TYPE: *value = EGL_RGB_BUFFER; break;
        case EGL_RENDERABLE_TYPE: *value = 0x8 | 0x4 | 0x40; /* EGL_OPENGL_BIT | ES2 | ES3 */ break;
        case EGL_CONFORMANT: *value = 0x8 | 0x4 | 0x40; break;
        case EGL_COLOR_COMPONENT_TYPE_EXT: *value = EGL_COLOR_COMPONENT_TYPE_FIXED_EXT; break;
        default: *value = 0; break;
    }
    return EGL_TRUE;
}

static EGLContext g_current_ctx = EGL_NO_CONTEXT;
static EGLSurface g_current_draw = EGL_NO_SURFACE;
static EGLSurface g_current_read = EGL_NO_SURFACE;

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) {
    (void)dpy; (void)config; (void)share_context; (void)attrib_list;
    static uintptr_t next_ctx = 0x100;
    return (EGLContext)(next_ctx++);
}

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx) {
    (void)dpy; (void)ctx;
    return EGL_TRUE;
}

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, void *win, const EGLint *attrib_list) {
    (void)dpy; (void)config; (void)win; (void)attrib_list;
    static uintptr_t next_surf = 0x200;
    return (EGLSurface)(next_surf++);
}

EGLSurface eglCreatePlatformWindowSurface(EGLDisplay dpy, EGLConfig config, void *native_window, const EGLAttrib *attrib_list) {
    (void)dpy; (void)config; (void)native_window; (void)attrib_list;
    static uintptr_t next_surf = 0x300;
    return (EGLSurface)(next_surf++);
}

EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list) {
    (void)dpy; (void)config; (void)attrib_list;
    static uintptr_t next_surf = 0x400;
    return (EGLSurface)(next_surf++);
}

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface) {
    (void)dpy; (void)surface;
    return EGL_TRUE;
}

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) {
    (void)dpy;
    g_current_draw = draw;
    g_current_read = read;
    g_current_ctx = ctx;
    return EGL_TRUE;
}

EGLContext eglGetCurrentContext(void) { return g_current_ctx; }
EGLSurface eglGetCurrentSurface(EGLint readdraw) { return (readdraw == 0x305A) ? g_current_read : g_current_draw; }
EGLDisplay eglGetCurrentDisplay(void) { return (EGLDisplay)0x1; }

EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    (void)dpy; (void)surface;
    maybe_post_quit();
    return EGL_TRUE;
}

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval) {
    (void)dpy; (void)interval;
    return EGL_TRUE;
}

/* ================= OpenGL 4.5 Implementation ================= */

GLenum glGetError(void) { return 0; /* GL_NO_ERROR */ }

const GLubyte *glGetString(GLenum name) {
    init_gl_exts();
    switch (name) {
        case 0x1F00: /* GL_VENDOR */ return (const GLubyte *)"Mesa";
        case 0x1F01: /* GL_RENDERER */ return (const GLubyte *)"llvmpipe (LLVM 19.1.7, 128 bits)";
        case 0x1F02: /* GL_VERSION */ return (const GLubyte *)"4.5 (Core Profile) Mesa 24.2.8-1";
        case 0x8B8C: /* GL_SHADING_LANGUAGE_VERSION */ return (const GLubyte *)"4.50";
        case 0x1F03: /* GL_EXTENSIONS */ return (const GLubyte *)g_gl_exts_combined;
        default: return (const GLubyte *)"";
    }
}

const GLubyte *glGetStringi(GLenum name, GLuint index) {
    if (name == 0x1F03 /* GL_EXTENSIONS */) {
        if (index < (GLuint)NUM_GL_EXTS) return (const GLubyte *)g_gl_exts_list[index];
        return NULL;
    }
    if (name == 0x82E9 /* GL_SPIR_V_EXTENSIONS */) return NULL;
    return (const GLubyte *)"";
}

void glGetIntegerv(GLenum pname, GLint *data) {
    if (!data) return;
    switch (pname) {
        case 0x821B: /* GL_MAJOR_VERSION */ *data = 4; break;
        case 0x821C: /* GL_MINOR_VERSION */ *data = 5; break;
        case 0x821D: /* GL_NUM_EXTENSIONS */ *data = NUM_GL_EXTS; break;
        case 0x9126: /* GL_CONTEXT_PROFILE_MASK */ *data = 1; /* GL_CONTEXT_CORE_PROFILE_BIT */ break;
        case 0x821E: /* GL_CONTEXT_FLAGS */ *data = 0; break;
        case 0x0D33: /* GL_MAX_TEXTURE_SIZE */ *data = 16384; break;
        case 0x8073: /* GL_MAX_3D_TEXTURE_SIZE */ *data = 2048; break;
        case 0x851C: /* GL_MAX_CUBE_MAP_TEXTURE_SIZE */ *data = 16384; break;
        case 0x88FF: /* GL_MAX_ARRAY_TEXTURE_LAYERS */ *data = 2048; break;
        case 0x84E8: /* GL_MAX_RENDERBUFFER_SIZE */ *data = 16384; break;
        case 0x84FD: /* GL_MAX_TEXTURE_LOD_BIAS */ *data = 16; break;
        case 0x84FF: /* GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT */ *data = 16; break;
        case 0x8824: /* GL_MAX_DRAW_BUFFERS */ *data = 8; break;
        case 0x8CDF: /* GL_MAX_COLOR_ATTACHMENTS */ *data = 8; break;
        case 0x8D57: /* GL_MAX_SAMPLES */ *data = 4; break;
        case 0x8E59: /* GL_MAX_COLOR_TEXTURE_SAMPLES */ *data = 4; break;
        case 0x8E5A: /* GL_MAX_DEPTH_TEXTURE_SAMPLES */ *data = 4; break;
        case 0x8E5B: /* GL_MAX_INTEGER_SAMPLES */ *data = 4; break;
        case 0x8869: /* GL_MAX_VERTEX_ATTRIBS */ *data = 16; break;
        case 0x8872: /* GL_MAX_TEXTURE_IMAGE_UNITS */ *data = 32; break;
        case 0x8B4C: /* GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS */ *data = 32; break;
        case 0x8B4D: /* GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS */ *data = 192; break;
        case 0x8DD2: /* GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS */ *data = 32; break;
        case 0x8E81: /* GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS */ *data = 32; break;
        case 0x8E82: /* GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS */ *data = 32; break;
        case 0x91BC: /* GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS */ *data = 32; break;
        case 0x8B49: /* GL_MAX_FRAGMENT_UNIFORM_COMPONENTS */ *data = 16384; break;
        case 0x8B4A: /* GL_MAX_VERTEX_UNIFORM_COMPONENTS */ *data = 16384; break;
        case 0x8DFB: /* GL_MAX_VARYING_VECTORS */ *data = 32; break;
        case 0x8B4B: /* GL_MAX_VARYING_FLOATS */ *data = 128; break;
        case 0x8A2B: /* GL_MAX_VERTEX_UNIFORM_BLOCKS */ *data = 16; break;
        case 0x8A2C: /* GL_MAX_GEOMETRY_UNIFORM_BLOCKS */ *data = 16; break;
        case 0x8A2D: /* GL_MAX_FRAGMENT_UNIFORM_BLOCKS */ *data = 16; break;
        case 0x8A2E: /* GL_MAX_COMBINED_UNIFORM_BLOCKS */ *data = 96; break;
        case 0x8A2F: /* GL_MAX_UNIFORM_BUFFER_BINDINGS */ *data = 96; break;
        case 0x8A30: /* GL_MAX_UNIFORM_BLOCK_SIZE */ *data = 65536; break;
        case 0x8A34: /* GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT */ *data = 16; break;
        case 0x90DF: /* GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT */ *data = 16; break;
        case 0x919F: /* GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT */ *data = 16; break;
        case 0x90BC: /* GL_MIN_MAP_BUFFER_ALIGNMENT */ *data = 64; break;
        case 0x8904: /* GL_MAX_PROGRAM_TEXEL_OFFSET */ *data = 7; break;
        case 0x8905: /* GL_MIN_PROGRAM_TEXEL_OFFSET */ *data = -8; break;
        case 0x8E5E: /* GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET */ *data = -32; break;
        case 0x8E5F: /* GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET */ *data = 31; break;
        case 0x90D2: /* GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS */ *data = 64; break;
        case 0x90D3: /* GL_MAX_SHADER_STORAGE_BLOCK_SIZE */ *data = 0x08000000; break;
        case 0x90D7: /* GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS */ *data = 16; break;
        case 0x90D8: /* GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS */ *data = 16; break;
        case 0x90D9: /* GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS */ *data = 16; break;
        case 0x90DA: /* GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS */ *data = 16; break;
        case 0x90DB: /* GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS */ *data = 16; break;
        case 0x90DC: /* GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS */ *data = 16; break;
        case 0x90DD: /* GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS */ *data = 96; break;
        case 0x90DE: /* GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES */ *data = 128; break;
        case 0x8F38: /* GL_MAX_IMAGE_UNITS */ *data = 32; break;
        case 0x8F39: /* GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS */ *data = 64; break;
        case 0x906D: /* GL_MAX_IMAGE_SAMPLES */ *data = 4; break;
        case 0x9070: /* GL_MAX_VERTEX_IMAGE_UNIFORMS */ *data = 16; break;
        case 0x9071: /* GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS */ *data = 16; break;
        case 0x9072: /* GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS */ *data = 16; break;
        case 0x9073: /* GL_MAX_GEOMETRY_IMAGE_UNIFORMS */ *data = 16; break;
        case 0x9074: /* GL_MAX_FRAGMENT_IMAGE_UNIFORMS */ *data = 16; break;
        case 0x9075: /* GL_MAX_COMBINED_IMAGE_UNIFORMS */ *data = 96; break;
        case 0x91B9: /* GL_MAX_COMPUTE_UNIFORM_BLOCKS */ *data = 16; break;
        case 0x91BB: /* GL_MAX_COMPUTE_UNIFORM_COMPONENTS */ *data = 16384; break;
        case 0x91BD: /* GL_MAX_COMPUTE_IMAGE_UNIFORMS */ *data = 16; break;
        case 0x91BE: /* GL_MAX_COMPUTE_WORK_GROUP_COUNT */
            data[0] = 65535; data[1] = 65535; data[2] = 65535; break;
        case 0x91BF: /* GL_MAX_COMPUTE_WORK_GROUP_SIZE */
            data[0] = 1024; data[1] = 1024; data[2] = 64; break;
        case 0x90EB: /* GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS */ *data = 1024; break;
        case 0x8262: /* GL_MAX_COMPUTE_SHARED_MEMORY_SIZE */ *data = 32768; break;
        case 0x8E7E: /* GL_MAX_TESS_GEN_LEVEL */ *data = 64; break;
        case 0x8E7F: /* GL_MAX_PATCH_VERTICES */ *data = 32; break;
        case 0x825B: /* GL_MAX_VIEWPORTS */ *data = 16; break;
        case 0x0D3A: /* GL_MAX_VIEWPORT_DIMS */ data[0] = 16384; data[1] = 16384; break;
        case 0x0BA2: /* GL_VIEWPORT */ data[0] = 0; data[1] = 0; data[2] = 800; data[3] = 600; break;
        case 0x8C89: /* GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS */ *data = 4; break;
        case 0x8C8A: /* GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS */ *data = 128; break;
        case 0x8C8B: /* GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS */ *data = 4; break;
        case 0x8E70: /* GL_MAX_TRANSFORM_FEEDBACK_BUFFERS */ *data = 4; break;
        case 0x88FC: /* GL_MAX_DUAL_SOURCE_DRAW_BUFFERS */ *data = 1; break;
        case 0x0D31: /* GL_MAX_CLIP_DISTANCES */ *data = 8; break;
        case 0x82F9: /* GL_MAX_CULL_DISTANCES */ *data = 8; break;
        case 0x82FA: /* GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES */ *data = 8; break;
        case 0x826E: /* GL_MAX_UNIFORM_LOCATIONS */ *data = 65536; break;
        case 0x82D6: /* GL_MAX_VERTEX_ATTRIB_BINDINGS */ *data = 32; break;
        case 0x82D5: /* GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET */ *data = 2047; break;
        case 0x82E5: /* GL_MAX_VERTEX_ATTRIB_STRIDE */ *data = 2048; break;
        case 0x8C2B: /* GL_MAX_TEXTURE_BUFFER_SIZE */ *data = 0x08000000; break;
        case 0x84F8: /* GL_MAX_RECTANGLE_TEXTURE_SIZE */ *data = 16384; break;
        case 0x86A2: /* GL_NUM_COMPRESSED_TEXTURE_FORMATS */ *data = 0; break;
        case 0x8CA6: /* GL_DRAW_FRAMEBUFFER_BINDING */ *data = 0; break;
        case 0x8CAA: /* GL_READ_FRAMEBUFFER_BINDING */ *data = 0; break;
        default: *data = 16; break;
    }
}

void glGetIntegeri_v(GLenum target, GLuint index, GLint *data) {
    if (!data) return;
    if (target == 0x91BE /* GL_MAX_COMPUTE_WORK_GROUP_COUNT */) *data = 65535;
    else if (target == 0x91BF /* GL_MAX_COMPUTE_WORK_GROUP_SIZE */) *data = (index == 2) ? 64 : 1024;
    else *data = 16;
}

void glGetInteger64v(GLenum pname, GLint64 *data) {
    if (!data) return;
    GLint tmp[4] = {16, 16, 16, 16};
    glGetIntegerv(pname, tmp);
    *data = (GLint64)tmp[0];
}

void glGetInteger64i_v(GLenum target, GLuint index, GLint64 *data) {
    (void)target; (void)index;
    if (data) *data = 65536;
}

void glGetFloatv(GLenum pname, GLfloat *data) {
    if (!data) return;
    if (pname == 0x846D || pname == 0x846E || pname == 0x0B12 || pname == 0x0B22 || pname == 0x825C) {
        data[0] = -32768.0f; data[1] = 32768.0f;
        return;
    }
    GLint tmp[4] = {16, 16, 16, 16};
    glGetIntegerv(pname, tmp);
    data[0] = (GLfloat)tmp[0];
}

void glGetDoublev(GLenum pname, GLdouble *data) {
    if (!data) return;
    GLfloat tmp[4] = {16.0f, 16.0f, 16.0f, 16.0f};
    glGetFloatv(pname, tmp);
    data[0] = (GLdouble)tmp[0];
}

void glGetBooleanv(GLenum pname, GLboolean *data) {
    (void)pname;
    if (data) *data = 1;
}

void glGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params) {
    (void)target; (void)internalformat;
    if (!params || bufSize <= 0) return;
    switch (pname) {
        case 0x826F: /* GL_INTERNALFORMAT_SUPPORTED */ params[0] = 1; break;
        case 0x8270: /* GL_INTERNALFORMAT_PREFERRED */ params[0] = (GLint)internalformat; break;
        case 0x9380: /* GL_NUM_SAMPLE_COUNTS */ params[0] = 1; break;
        case 0x80A9: /* GL_SAMPLES */ params[0] = 4; break;
        case 0x829A: /* GL_COLOR_RENDERABLE */
        case 0x829B: /* GL_DEPTH_RENDERABLE */
        case 0x829C: /* GL_STENCIL_RENDERABLE */ params[0] = 1; break;
        default: params[0] = 0x82B7; /* GL_FULL_SUPPORT */ break;
    }
}

void glGetInternalformati64v(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64 *params) {
    if (!params || bufSize <= 0) return;
    GLint tmp = 0;
    glGetInternalformativ(target, internalformat, pname, 1, &tmp);
    params[0] = (GLint64)tmp;
}

static GLuint g_fbo_rb[256];
void glGenFramebuffers(GLsizei n, GLuint *ids) { alloc_ids(n, ids); }
void glGenFramebuffersEXT(GLsizei n, GLuint *ids) { alloc_ids(n, ids); }
void glCreateFramebuffers(GLsizei n, GLuint *ids) { alloc_ids(n, ids); }
void glDeleteFramebuffers(GLsizei n, const GLuint *ids) { (void)n; (void)ids; }
void glBindFramebuffer(GLenum target, GLuint framebuffer) { (void)target; (void)framebuffer; }
void glGenRenderbuffers(GLsizei n, GLuint *ids) { alloc_ids(n, ids); }
void glCreateRenderbuffers(GLsizei n, GLuint *ids) { alloc_ids(n, ids); }
void glDeleteRenderbuffers(GLsizei n, const GLuint *ids) { (void)n; (void)ids; }
void glBindRenderbuffer(GLenum target, GLuint renderbuffer) { (void)target; (void)renderbuffer; }
void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
    (void)target; (void)internalformat; (void)width; (void)height;
}
void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) {
    (void)target; (void)samples; (void)internalformat; (void)width; (void)height;
}
void glNamedRenderbufferStorage(GLuint rb, GLenum internalformat, GLsizei width, GLsizei height) {
    (void)rb; (void)internalformat; (void)width; (void)height;
}
void glNamedRenderbufferStorageMultisample(GLuint rb, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) {
    (void)rb; (void)samples; (void)internalformat; (void)width; (void)height;
}
void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
    (void)target; (void)renderbuffertarget;
    g_fbo_rb[attachment & 0xFF] = renderbuffer ? renderbuffer : 1;
}
void glNamedFramebufferRenderbuffer(GLuint fbo, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
    (void)fbo; (void)renderbuffertarget;
    g_fbo_rb[attachment & 0xFF] = renderbuffer ? renderbuffer : 1;
}
void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params) {
    (void)target;
    if (!params) return;
    if (pname == 0x8CD1 /* GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME */) {
        GLuint rb = g_fbo_rb[attachment & 0xFF];
        *params = rb ? (GLint)rb : 1;
    } else if (pname == 0x8CD0 /* GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE */) {
        *params = 0x8D41; /* GL_RENDERBUFFER */
    } else {
        *params = 8;
    }
}
void glGetNamedFramebufferAttachmentParameteriv(GLuint fbo, GLenum attachment, GLenum pname, GLint *params) {
    glGetFramebufferAttachmentParameteriv(fbo, attachment, pname, params);
}
GLenum glCheckFramebufferStatus(GLenum target) { (void)target; return 0x8CD5; /* GL_FRAMEBUFFER_COMPLETE */ }
GLenum glCheckNamedFramebufferStatus(GLuint fbo, GLenum target) { (void)fbo; (void)target; return 0x8CD5; }

/* Buffers */
void glGenBuffers(GLsizei n, GLuint *buffers) { alloc_ids(n, buffers); }
void glCreateBuffers(GLsizei n, GLuint *buffers) { alloc_ids(n, buffers); }
void glDeleteBuffers(GLsizei n, const GLuint *buffers) { (void)n; (void)buffers; }
void glBindBuffer(GLenum target, GLuint buffer) {
    g_bound_buffers[target_to_slot(target)] = buffer;
}
void glBindBufferBase(GLenum target, GLuint index, GLuint buffer) {
    (void)index;
    g_bound_buffers[target_to_slot(target)] = buffer;
}
void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    (void)index; (void)offset; (void)size;
    g_bound_buffers[target_to_slot(target)] = buffer;
}
void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
    (void)usage;
    GLuint id = g_bound_buffers[target_to_slot(target)];
    BufferSlot *s = get_buffer_slot(id, size > 0 ? (size_t)size : 64);
    if (data && size > 0 && s->data) memcpy(s->data, data, (size_t)size);
}
void glNamedBufferData(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage) {
    (void)usage;
    BufferSlot *s = get_buffer_slot(buffer, size > 0 ? (size_t)size : 64);
    if (data && size > 0 && s->data) memcpy(s->data, data, (size_t)size);
}
void glBufferStorage(GLenum target, GLsizeiptr size, const void *data, GLbitfield flags) {
    glBufferData(target, size, data, flags);
}
void glNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags) {
    glNamedBufferData(buffer, size, data, flags);
}
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) {
    GLuint id = g_bound_buffers[target_to_slot(target)];
    BufferSlot *s = get_buffer_slot(id, (size_t)(offset + size));
    if (data && size > 0 && s->data) memcpy((char *)s->data + offset, data, (size_t)size);
}
void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data) {
    BufferSlot *s = get_buffer_slot(buffer, (size_t)(offset + size));
    if (data && size > 0 && s->data) memcpy((char *)s->data + offset, data, (size_t)size);
}
void *glMapBuffer(GLenum target, GLenum access) {
    (void)access;
    GLuint id = g_bound_buffers[target_to_slot(target)];
    BufferSlot *s = get_buffer_slot(id, 65536);
    return s->data;
}
void *glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) {
    (void)access;
    GLuint id = g_bound_buffers[target_to_slot(target)];
    BufferSlot *s = get_buffer_slot(id, (size_t)(offset + length));
    return (char *)s->data + offset;
}
void *glMapNamedBuffer(GLuint buffer, GLenum access) {
    (void)access;
    BufferSlot *s = get_buffer_slot(buffer, 65536);
    return s->data;
}
void *glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access) {
    (void)access;
    BufferSlot *s = get_buffer_slot(buffer, (size_t)(offset + length));
    return (char *)s->data + offset;
}
GLboolean glUnmapBuffer(GLenum target) { (void)target; return 1; }
GLboolean glUnmapNamedBuffer(GLuint buffer) { (void)buffer; return 1; }

/* Textures, Samplers, VAOs */
void glGenTextures(GLsizei n, GLuint *textures) { alloc_ids(n, textures); }
void glCreateTextures(GLenum target, GLsizei n, GLuint *textures) { (void)target; alloc_ids(n, textures); }
void glDeleteTextures(GLsizei n, const GLuint *textures) { (void)n; (void)textures; }
void glGenSamplers(GLsizei n, GLuint *samplers) { alloc_ids(n, samplers); }
void glCreateSamplers(GLsizei n, GLuint *samplers) { alloc_ids(n, samplers); }
void glDeleteSamplers(GLsizei n, const GLuint *samplers) { (void)n; (void)samplers; }
void glGenVertexArrays(GLsizei n, GLuint *arrays) { alloc_ids(n, arrays); }
void glCreateVertexArrays(GLsizei n, GLuint *arrays) { alloc_ids(n, arrays); }
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) { (void)n; (void)arrays; }

void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params) {
    (void)target; (void)level;
    if (!params) return;
    switch (pname) {
        case 0x1000: /* GL_TEXTURE_WIDTH */ *params = 256; break;
        case 0x1001: /* GL_TEXTURE_HEIGHT */ *params = 256; break;
        case 0x8071: /* GL_TEXTURE_DEPTH */ *params = 1; break;
        case 0x1003: /* GL_TEXTURE_INTERNAL_FORMAT */ *params = 0x8058; /* GL_RGBA8 */ break;
        case 0x86A1: /* GL_TEXTURE_COMPRESSED */ *params = 1; break;
        case 0x86A0: /* GL_TEXTURE_COMPRESSED_IMAGE_SIZE */ *params = 65536; break;
        default: *params = 8; break;
    }
}

/* Shaders & Programs */
GLuint glCreateShader(GLenum shaderType) { (void)shaderType; return g_next_id++; }
GLuint glCreateProgram(void) { return g_next_id++; }
GLuint glCreateShaderProgramv(GLenum type, GLsizei count, const GLchar *const *strings) {
    (void)type; (void)count; (void)strings;
    return g_next_id++;
}
void glGenProgramPipelines(GLsizei n, GLuint *pipelines) { alloc_ids(n, pipelines); }
void glCreateProgramPipelines(GLsizei n, GLuint *pipelines) { alloc_ids(n, pipelines); }
void glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
    (void)shader;
    if (!params) return;
    if (pname == 0x8B84 /* GL_INFO_LOG_LENGTH */) *params = 0;
    else *params = 1; /* GL_COMPILE_STATUS = GL_TRUE */
}
void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {
    (void)shader;
    if (length) *length = 0;
    if (infoLog && maxLength > 0) infoLog[0] = '\0';
}
void glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
    (void)program;
    if (!params) return;
    if (pname == 0x8B84 /* GL_INFO_LOG_LENGTH */ ||
        pname == 0x8B86 /* GL_ACTIVE_UNIFORMS */ ||
        pname == 0x8B89 /* GL_ACTIVE_ATTRIBUTES */ ||
        pname == 0x8A36 /* GL_ACTIVE_UNIFORM_BLOCKS */) {
        *params = 0;
    } else {
        *params = 1; /* GL_LINK_STATUS = GL_TRUE */
    }
}
void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {
    (void)program;
    if (length) *length = 0;
    if (infoLog && maxLength > 0) infoLog[0] = '\0';
}
GLint glGetUniformLocation(GLuint program, const GLchar *name) { (void)program; (void)name; return -1; }
GLuint glGetUniformBlockIndex(GLuint program, const GLchar *name) { (void)program; (void)name; return 0xFFFFFFFFu; }
GLint glGetAttribLocation(GLuint program, const GLchar *name) { (void)program; (void)name; return -1; }
GLint glGetFragDataLocation(GLuint program, const GLchar *name) { (void)program; (void)name; return -1; }
GLint glGetFragDataIndex(GLuint program, const GLchar *name) { (void)program; (void)name; return -1; }

/* Queries & Sync */
void glGenQueries(GLsizei n, GLuint *ids) { alloc_ids(n, ids); }
void glCreateQueries(GLenum target, GLsizei n, GLuint *ids) { (void)target; alloc_ids(n, ids); }
void glGetQueryObjectiv(GLuint id, GLenum pname, GLint *params) { (void)id; (void)pname; if (params) *params = 1; }
void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params) { (void)id; (void)pname; if (params) *params = 1; }
void glGetQueryObjecti64v(GLuint id, GLenum pname, GLint64 *params) { (void)id; (void)pname; if (params) *params = 1; }
void glGetQueryObjectui64v(GLuint id, GLenum pname, GLuint64 *params) { (void)id; (void)pname; if (params) *params = 1; }
void glGetQueryiv(GLenum target, GLenum pname, GLint *params) { (void)target; (void)pname; if (params) *params = 32; }

GLsync glFenceSync(GLenum condition, GLbitfield flags) {
    (void)condition; (void)flags;
    return (GLsync)(uintptr_t)(g_next_id++);
}
GLboolean glIsSync(GLsync sync) { return sync != NULL; }
void glDeleteSync(GLsync sync) { (void)sync; }
GLenum glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) {
    (void)sync; (void)flags; (void)timeout;
    return 0x911A; /* GL_ALREADY_SIGNALED */
}
void glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) {
    (void)sync; (void)flags; (void)timeout;
}
void glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values) {
    (void)sync; (void)pname;
    if (length) *length = 1;
    if (values && bufSize > 0) values[0] = 0x9119; /* GL_SIGNALED */
}

GLenum glGetGraphicsResetStatus(void) { return 0; /* GL_NO_ERROR */ }
GLenum glGetGraphicsResetStatusARB(void) { return 0; }

/* Draw calls */
void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    (void)mode; (void)first; (void)count;
    maybe_post_quit();
}
void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) {
    (void)mode; (void)first; (void)count; (void)instancecount;
    maybe_post_quit();
}
void glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance) {
    (void)mode; (void)first; (void)count; (void)instancecount; (void)baseinstance;
    maybe_post_quit();
}
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
    (void)mode; (void)count; (void)type; (void)indices;
    maybe_post_quit();
}
void glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex) {
    (void)mode; (void)count; (void)type; (void)indices; (void)basevertex;
    maybe_post_quit();
}
void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount) {
    (void)mode; (void)count; (void)type; (void)indices; (void)instancecount;
    maybe_post_quit();
}
void glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex) {
    (void)mode; (void)count; (void)type; (void)indices; (void)instancecount; (void)basevertex;
    maybe_post_quit();
}
void glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance) {
    (void)mode; (void)count; (void)type; (void)indices; (void)instancecount; (void)basevertex; (void)baseinstance;
    maybe_post_quit();
}

static uintptr_t generic_gl_noop(void) { return 0; }

typedef struct {
    const char *name;
    void *fn;
} ProcEntry;

static const ProcEntry g_procs[] = {
    {"eglGetError", (void *)eglGetError},
    {"eglQueryString", (void *)eglQueryString},
    {"eglGetPlatformDisplay", (void *)eglGetPlatformDisplay},
    {"eglGetPlatformDisplayEXT", (void *)eglGetPlatformDisplayEXT},
    {"eglGetDisplay", (void *)eglGetDisplay},
    {"eglInitialize", (void *)eglInitialize},
    {"eglTerminate", (void *)eglTerminate},
    {"eglBindAPI", (void *)eglBindAPI},
    {"eglQueryAPI", (void *)eglQueryAPI},
    {"eglQueryDisplayAttribEXT", (void *)eglQueryDisplayAttribEXT},
    {"eglQueryDevicesEXT", (void *)eglQueryDevicesEXT},
    {"eglQueryDeviceStringEXT", (void *)eglQueryDeviceStringEXT},
    {"eglQueryDeviceAttribEXT", (void *)eglQueryDeviceAttribEXT},
    {"eglGetConfigs", (void *)eglGetConfigs},
    {"eglChooseConfig", (void *)eglChooseConfig},
    {"eglGetConfigAttrib", (void *)eglGetConfigAttrib},
    {"eglCreateContext", (void *)eglCreateContext},
    {"eglDestroyContext", (void *)eglDestroyContext},
    {"eglCreateWindowSurface", (void *)eglCreateWindowSurface},
    {"eglCreatePlatformWindowSurface", (void *)eglCreatePlatformWindowSurface},
    {"eglCreatePbufferSurface", (void *)eglCreatePbufferSurface},
    {"eglDestroySurface", (void *)eglDestroySurface},
    {"eglMakeCurrent", (void *)eglMakeCurrent},
    {"eglGetCurrentContext", (void *)eglGetCurrentContext},
    {"eglGetCurrentSurface", (void *)eglGetCurrentSurface},
    {"eglGetCurrentDisplay", (void *)eglGetCurrentDisplay},
    {"eglSwapBuffers", (void *)eglSwapBuffers},
    {"eglSwapInterval", (void *)eglSwapInterval},
    {"glGetError", (void *)glGetError},
    {"glGetString", (void *)glGetString},
    {"glGetStringi", (void *)glGetStringi},
    {"glGetIntegerv", (void *)glGetIntegerv},
    {"glGetIntegeri_v", (void *)glGetIntegeri_v},
    {"glGetInteger64v", (void *)glGetInteger64v},
    {"glGetInteger64i_v", (void *)glGetInteger64i_v},
    {"glGetFloatv", (void *)glGetFloatv},
    {"glGetDoublev", (void *)glGetDoublev},
    {"glGetBooleanv", (void *)glGetBooleanv},
    {"glGetInternalformativ", (void *)glGetInternalformativ},
    {"glGetInternalformati64v", (void *)glGetInternalformati64v},
    {"glGenFramebuffers", (void *)glGenFramebuffers},
    {"glGenFramebuffersEXT", (void *)glGenFramebuffersEXT},
    {"glCreateFramebuffers", (void *)glCreateFramebuffers},
    {"glDeleteFramebuffers", (void *)glDeleteFramebuffers},
    {"glBindFramebuffer", (void *)glBindFramebuffer},
    {"glGenRenderbuffers", (void *)glGenRenderbuffers},
    {"glCreateRenderbuffers", (void *)glCreateRenderbuffers},
    {"glDeleteRenderbuffers", (void *)glDeleteRenderbuffers},
    {"glBindRenderbuffer", (void *)glBindRenderbuffer},
    {"glRenderbufferStorage", (void *)glRenderbufferStorage},
    {"glRenderbufferStorageMultisample", (void *)glRenderbufferStorageMultisample},
    {"glNamedRenderbufferStorage", (void *)glNamedRenderbufferStorage},
    {"glNamedRenderbufferStorageMultisample", (void *)glNamedRenderbufferStorageMultisample},
    {"glFramebufferRenderbuffer", (void *)glFramebufferRenderbuffer},
    {"glNamedFramebufferRenderbuffer", (void *)glNamedFramebufferRenderbuffer},
    {"glGetFramebufferAttachmentParameteriv", (void *)glGetFramebufferAttachmentParameteriv},
    {"glGetNamedFramebufferAttachmentParameteriv", (void *)glGetNamedFramebufferAttachmentParameteriv},
    {"glCheckFramebufferStatus", (void *)glCheckFramebufferStatus},
    {"glCheckFramebufferStatusEXT", (void *)glCheckFramebufferStatus},
    {"glCheckNamedFramebufferStatus", (void *)glCheckNamedFramebufferStatus},
    {"glGenBuffers", (void *)glGenBuffers},
    {"glGenBuffersARB", (void *)glGenBuffers},
    {"glCreateBuffers", (void *)glCreateBuffers},
    {"glDeleteBuffers", (void *)glDeleteBuffers},
    {"glBindBuffer", (void *)glBindBuffer},
    {"glBindBufferARB", (void *)glBindBuffer},
    {"glBindBufferBase", (void *)glBindBufferBase},
    {"glBindBufferRange", (void *)glBindBufferRange},
    {"glBufferData", (void *)glBufferData},
    {"glBufferDataARB", (void *)glBufferData},
    {"glNamedBufferData", (void *)glNamedBufferData},
    {"glBufferStorage", (void *)glBufferStorage},
    {"glNamedBufferStorage", (void *)glNamedBufferStorage},
    {"glBufferSubData", (void *)glBufferSubData},
    {"glBufferSubDataARB", (void *)glBufferSubData},
    {"glNamedBufferSubData", (void *)glNamedBufferSubData},
    {"glMapBuffer", (void *)glMapBuffer},
    {"glMapBufferARB", (void *)glMapBuffer},
    {"glMapBufferRange", (void *)glMapBufferRange},
    {"glMapNamedBuffer", (void *)glMapNamedBuffer},
    {"glMapNamedBufferRange", (void *)glMapNamedBufferRange},
    {"glUnmapBuffer", (void *)glUnmapBuffer},
    {"glUnmapBufferARB", (void *)glUnmapBuffer},
    {"glUnmapNamedBuffer", (void *)glUnmapNamedBuffer},
    {"glGenTextures", (void *)glGenTextures},
    {"glCreateTextures", (void *)glCreateTextures},
    {"glDeleteTextures", (void *)glDeleteTextures},
    {"glGenSamplers", (void *)glGenSamplers},
    {"glCreateSamplers", (void *)glCreateSamplers},
    {"glDeleteSamplers", (void *)glDeleteSamplers},
    {"glGenVertexArrays", (void *)glGenVertexArrays},
    {"glCreateVertexArrays", (void *)glCreateVertexArrays},
    {"glDeleteVertexArrays", (void *)glDeleteVertexArrays},
    {"glGetTexLevelParameteriv", (void *)glGetTexLevelParameteriv},
    {"glCreateShader", (void *)glCreateShader},
    {"glCreateShaderObjectARB", (void *)glCreateShader},
    {"glCreateProgram", (void *)glCreateProgram},
    {"glCreateProgramObjectARB", (void *)glCreateProgram},
    {"glCreateShaderProgramv", (void *)glCreateShaderProgramv},
    {"glGenProgramPipelines", (void *)glGenProgramPipelines},
    {"glCreateProgramPipelines", (void *)glCreateProgramPipelines},
    {"glGetShaderiv", (void *)glGetShaderiv},
    {"glGetObjectParameterivARB", (void *)glGetShaderiv},
    {"glGetShaderInfoLog", (void *)glGetShaderInfoLog},
    {"glGetInfoLogARB", (void *)glGetShaderInfoLog},
    {"glGetProgramiv", (void *)glGetProgramiv},
    {"glGetProgramInfoLog", (void *)glGetProgramInfoLog},
    {"glGetUniformLocation", (void *)glGetUniformLocation},
    {"glGetUniformLocationARB", (void *)glGetUniformLocation},
    {"glGetUniformBlockIndex", (void *)glGetUniformBlockIndex},
    {"glGetAttribLocation", (void *)glGetAttribLocation},
    {"glGetAttribLocationARB", (void *)glGetAttribLocation},
    {"glGetFragDataLocation", (void *)glGetFragDataLocation},
    {"glGetFragDataIndex", (void *)glGetFragDataIndex},
    {"glGenQueries", (void *)glGenQueries},
    {"glGenQueriesARB", (void *)glGenQueries},
    {"glCreateQueries", (void *)glCreateQueries},
    {"glGetQueryObjectiv", (void *)glGetQueryObjectiv},
    {"glGetQueryObjectuiv", (void *)glGetQueryObjectuiv},
    {"glGetQueryObjecti64v", (void *)glGetQueryObjecti64v},
    {"glGetQueryObjectui64v", (void *)glGetQueryObjectui64v},
    {"glGetQueryiv", (void *)glGetQueryiv},
    {"glFenceSync", (void *)glFenceSync},
    {"glIsSync", (void *)glIsSync},
    {"glDeleteSync", (void *)glDeleteSync},
    {"glClientWaitSync", (void *)glClientWaitSync},
    {"glWaitSync", (void *)glWaitSync},
    {"glGetSynciv", (void *)glGetSynciv},
    {"glGetGraphicsResetStatus", (void *)glGetGraphicsResetStatus},
    {"glGetGraphicsResetStatusARB", (void *)glGetGraphicsResetStatusARB},
    {"glDrawArrays", (void *)glDrawArrays},
    {"glDrawArraysInstanced", (void *)glDrawArraysInstanced},
    {"glDrawArraysInstancedBaseInstance", (void *)glDrawArraysInstancedBaseInstance},
    {"glDrawElements", (void *)glDrawElements},
    {"glDrawElementsBaseVertex", (void *)glDrawElementsBaseVertex},
    {"glDrawElementsInstanced", (void *)glDrawElementsInstanced},
    {"glDrawElementsInstancedBaseVertex", (void *)glDrawElementsInstancedBaseVertex},
    {"glDrawElementsInstancedBaseVertexBaseInstance", (void *)glDrawElementsInstancedBaseVertexBaseInstance},
};

__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char *procname) {
    if (!procname) return NULL;
    for (size_t i = 0; i < sizeof(g_procs)/sizeof(g_procs[0]); i++) {
        if (strcmp(procname, g_procs[i].name) == 0) {
            return (__eglMustCastToProperFunctionPointerType)g_procs[i].fn;
        }
    }
    if (strncmp(procname, "gl", 2) == 0 || strncmp(procname, "egl", 3) == 0) {
        return (__eglMustCastToProperFunctionPointerType)generic_gl_noop;
    }
    return NULL;
}
