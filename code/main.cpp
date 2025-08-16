#include <cstdio>
#include <cstdlib>
#include <chrono>

#include <HandmadeMath/HandmadeMath.h>

#define SOKOL_IMPL
#define SOKOL_D3D11
#include <sokol/sokol_app.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_log.h>

#include "base/base_arena.h"
#include "base/base_file.h"
#include "base/base_types.h"
#include "base/base_utils.h"

struct State
{
    sg_pipeline m_Pipe;
    sg_bindings m_Bind;
    sg_pass_action m_PassAction;
};

static State g_State = {};

struct CBParams
{
    float m_Time; // Time elapsed since load
    HMM_Vec2 m_Resolution; // Window size (width, height)
    HMM_Vec2 m_Mouse; // Mouse coordinates
};

static CBParams g_CBParams = {};

static const auto START_TIMESTAMP = std::chrono::high_resolution_clock::now();

static void InitCB()
{
    Arena* arena = ArenaCreate(GB(1));

    g_CBParams.m_Resolution.Width = sapp_widthf();
    g_CBParams.m_Resolution.Height = sapp_heightf();

    sg_desc setupDesc = {};
    setupDesc.environment = sglue_environment();
    setupDesc.logger.func = slog_func;
    sg_setup(setupDesc);

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
        .stage = SG_SHADERSTAGE_FRAGMENT,
        .size = sizeof(CBParams),
        .hlsl_register_b_n = 0,
    };

    sg_pipeline_desc pipeDesc = {};
    pipeDesc.shader = sg_make_shader(shaderDesc);
    pipeDesc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;

    g_State.m_Pipe = sg_make_pipeline(pipeDesc);

    sg_pass_action passAction = {};
    passAction.colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.58f, 0.7f, 0.85f, 1.0f } };

    g_State.m_PassAction = passAction;
}

static void FrameCB()
{
    sg_begin_pass((sg_pass{ .action = g_State.m_PassAction, .swapchain = sglue_swapchain() }));
    sg_apply_pipeline(g_State.m_Pipe);
    //sg_apply_bindings(g_State.m_Bind);

    const auto now = std::chrono::high_resolution_clock::now();
    g_CBParams.m_Time = std::chrono::duration<float>(now - START_TIMESTAMP).count();

    sg_apply_uniforms(0, sg_range{ .ptr = &g_CBParams, .size = sizeof(g_CBParams) });
    sg_draw(0, 4, 1);
    sg_end_pass();
    sg_commit();
}

static void CleanupCB()
{
    sg_shutdown();
}

static void EventCB(const sapp_event* ev)
{
    if (ev->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
        if (ev->key_code == SAPP_KEYCODE_ESCAPE)
        {
            sapp_request_quit();
        }
    }
    if (ev->type == SAPP_EVENTTYPE_RESIZED)
    {
        g_CBParams.m_Resolution.Width = sapp_widthf();
        g_CBParams.m_Resolution.Height = sapp_heightf();
    }
    if (ev->type == SAPP_EVENTTYPE_MOUSE_MOVE)
    {
        HMM_Vec2 normalizedMouse = HMM_DivV2(
            HMM_Vec2{ ev->mouse_x, ev->mouse_y },
            g_CBParams.m_Resolution);

        g_CBParams.m_Mouse = normalizedMouse;
    }
}

sapp_desc sokol_main(int argc, char* argv[])
{
    sapp_desc result = {};

    result.width = 640;
    result.height = 480;
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
