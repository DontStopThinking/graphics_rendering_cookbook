#include <chrono>

#include <glm/ext.hpp>

#define SOKOL_IMPL
#define SOKOL_D3D11
#include <sokol/sokol_app.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_log.h>
#include <imgui/imgui.h>
#include <sokol/util/sokol_imgui.h>

#include "base/base_types.h"
#include "base/base_arena.h"
#include "base/base_file.h"
#include "base/base_utils.h"

struct State
{
    sg_pipeline m_FillPipe;
    sg_pipeline m_WirePipe;
    sg_bindings m_Bind;
    sg_pass_action m_PassAction;
};

static State g_State = {};

struct VertexUniforms
{
    glm::mat4 m_MVP;
    bool m_IsWireframe;
};

struct PixelUniforms
{
    float m_Time; // Time elapsed since load
    glm::vec2 m_Resolution; // Window size (width, height)
    glm::vec2 m_Mouse; // Mouse coordinates
};

static PixelUniforms g_PixelUniforms = {};

static bool g_ShowImgui = false;

static const auto START_TIMESTAMP = std::chrono::high_resolution_clock::now();

static void InitCB()
{
    Arena* arena = ArenaCreate(GB(1));

    g_PixelUniforms.m_Resolution = glm::vec2(sapp_widthf(), sapp_heightf());

    sg_desc setupDesc = {};
    setupDesc.environment = sglue_environment();
    setupDesc.logger.func = slog_func;
    sg_setup(setupDesc);

    // Setup Sokol Imgui
    simgui_desc_t imguiDesc = {};
    imguiDesc.logger.func = slog_func;
    simgui_setup(&imguiDesc);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Create a checkerboard texture
    constexpr u32 TEXTURE_PIXELS[] =
    {
        0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
        0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
        0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
        0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    };

    sg_image_desc imageDesc = {};
    imageDesc.width = 4;
    imageDesc.height = 4;
    imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    imageDesc.data.subimage[0][0] = SG_RANGE(TEXTURE_PIXELS);

    sg_image img = sg_make_image(imageDesc);

    // Create a sampler
    sg_sampler sampler = sg_make_sampler(
    {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
    });

    const char* const shaderSource = ReadFileAndNullTerminateIt(arena, "shaders/shader.hlsl");

    sg_shader_desc shaderDesc = {};
    shaderDesc.vertex_func.source = shaderSource;
    shaderDesc.vertex_func.d3d11_filepath = "shaders/shader.hlsl";
    shaderDesc.vertex_func.entry = "VSMain";
    shaderDesc.vertex_func.d3d11_target = "vs_5_0";
    shaderDesc.fragment_func.source = shaderSource;
    shaderDesc.fragment_func.d3d11_filepath = "shaders/shader.hlsl";
    shaderDesc.fragment_func.entry = "PSMain";
    shaderDesc.fragment_func.d3d11_target = "ps_5_0";
    shaderDesc.uniform_blocks[0] =
    {
        .stage = SG_SHADERSTAGE_VERTEX,
        .size = sizeof(VertexUniforms),
        .hlsl_register_b_n = 0,
    };
    shaderDesc.uniform_blocks[1] =
    {
        .stage = SG_SHADERSTAGE_FRAGMENT,
        .size = sizeof(PixelUniforms),
        .hlsl_register_b_n = 1,
    };
    shaderDesc.images[0] =
    {
        .stage = SG_SHADERSTAGE_FRAGMENT,
        .hlsl_register_t_n = 0,
    };
    shaderDesc.samplers[0] =
    {
        .stage = SG_SHADERSTAGE_FRAGMENT,
        .hlsl_register_s_n = 0,
    };
    shaderDesc.image_sampler_pairs[0] =
    {
        .stage = SG_SHADERSTAGE_FRAGMENT,
        .image_slot = 0,
        .sampler_slot = 0,
    };

    // Resource bindings
    sg_bindings bind = {};
    bind.images[0] = img;
    bind.samplers[0] = sampler;

    g_State.m_Bind = bind;

    sg_pipeline_desc fillPipeDesc = {};
    fillPipeDesc.shader = sg_make_shader(shaderDesc);
    fillPipeDesc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    fillPipeDesc.depth =
    {
        .compare = SG_COMPAREFUNC_LESS_EQUAL,
        .write_enabled = true,
    };
    fillPipeDesc.cull_mode = SG_CULLMODE_BACK;

    sg_pipeline_desc wirePipeDesc = fillPipeDesc;
    wirePipeDesc.depth.bias = -1.0f;
    wirePipeDesc.depth.bias_slope_scale = -1.0f;
    wirePipeDesc.primitive_type = SG_PRIMITIVETYPE_LINES;

    g_State.m_FillPipe = sg_make_pipeline(fillPipeDesc);
    g_State.m_WirePipe = sg_make_pipeline(wirePipeDesc);

    sg_pass_action passAction = {};
    passAction.colors[0] =
    {
        .load_action = SG_LOADACTION_CLEAR,
        .clear_value = { 0.58f, 0.7f, 0.85f, 1.0f } // Cauliflower blue
    };

    g_State.m_PassAction = passAction;
}

static void FrameCB()
{
    if (g_ShowImgui)
    {
        // Start Imgui frame
        simgui_new_frame(
        {
            .width = sapp_width(),
            .height = sapp_height(),
            .delta_time = sapp_frame_duration(),
            .dpi_scale = sapp_dpi_scale(),
        });

        bool demoWindowOpen = true;

        ImGui::SetNextWindowPos(ImVec2(sapp_widthf() / 2.0f, sapp_heightf() / 2.0f), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&demoWindowOpen);
    }

    sg_begin_pass(sg_pass{ .action = g_State.m_PassAction, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(g_State.m_FillPipe);
    sg_apply_bindings(g_State.m_Bind);

    const auto now = std::chrono::high_resolution_clock::now();
    g_PixelUniforms.m_Time = std::chrono::duration<float>(now - START_TIMESTAMP).count();

    const float ratio = sapp_widthf() / sapp_heightf();

    const glm::mat4 m = glm::rotate(
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.5f)),
        g_PixelUniforms.m_Time,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );

    const glm::mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);

    VertexUniforms vertexUniforms = {};
    vertexUniforms.m_MVP = p * m;

    // 1. Draw filled cube first
    sg_apply_uniforms(0, sg_range{ .ptr = &vertexUniforms, .size = sizeof(VertexUniforms) });
    sg_apply_uniforms(1, sg_range{ .ptr = &g_PixelUniforms, .size = sizeof(PixelUniforms) });
    sg_draw(0, 36, 1);

    // 2. Draw wireframe cube on top
    vertexUniforms.m_IsWireframe = true;
    sg_apply_pipeline(g_State.m_WirePipe);
    sg_apply_bindings(g_State.m_Bind);
    sg_apply_uniforms(0, sg_range{ .ptr = &vertexUniforms, .size = sizeof(VertexUniforms) });
    sg_apply_uniforms(1, sg_range{ .ptr = &g_PixelUniforms, .size = sizeof(PixelUniforms) });
    sg_draw(0, 36, 1);

    if (g_ShowImgui)
    {
        simgui_render(); // Render Imgui
    }

    sg_end_pass();
    sg_commit();
}

static void CleanupCB()
{
    simgui_shutdown(); // Shutdown Imgui
    sg_shutdown();
}

static void EventCB(const sapp_event* ev)
{
    if (g_ShowImgui)
    {
        simgui_handle_event(ev); // Handle Imgui input
    }

    if (ev->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        // Escape to exit the application
        if (ev->key_code == SAPP_KEYCODE_ESCAPE)
        {
            sapp_request_quit();
        }
        // Shift+F10 to toggle Imgui
        if (ev->modifiers == SAPP_MODIFIER_SHIFT)
        {
            if (ev->key_code == SAPP_KEYCODE_F10)
            {
                g_ShowImgui = !g_ShowImgui;
            }
        }
    }
    if (ev->type == SAPP_EVENTTYPE_RESIZED)
    {
        g_PixelUniforms.m_Resolution = glm::vec2(sapp_widthf(), sapp_heightf());
    }
    if (ev->type == SAPP_EVENTTYPE_MOUSE_MOVE)
    {
        const glm::vec2 normalizedMouse = glm::vec2(ev->mouse_x, ev->mouse_y) / g_PixelUniforms.m_Resolution;

        g_PixelUniforms.m_Mouse = normalizedMouse;
    }
}

sapp_desc sokol_main(int argc, char* argv[])
{
    sapp_desc result = {};

    result.width = 1280;
    result.height = 720;
    result.window_title = "Learn Sokol";
    result.high_dpi = true;
    result.icon.sokol_default = true;
    result.logger.func = slog_func;
    result.win32_console_attach = true;
    result.init_cb = InitCB;
    result.frame_cb = FrameCB;
    result.cleanup_cb = CleanupCB;
    result.event_cb = EventCB;

    return result;
}
