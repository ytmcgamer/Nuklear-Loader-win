#include <iostream>
#include <future>
#include "nuklear/nuklear_d3d11.h"
#pragma comment(lib, "Winmm.lib")
#include <Windows.h>
#include <winternl.h>
#include <assert.h>
#include <tchar.h>
#include <ShlObj_core.h>
#include <AccCtrl.h>
#include <AclAPI.h>
#include <shellapi.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include "stdafx.h"
#include "xor.h"
#pragma comment(lib, "version")

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")
HWND FortniteHWND = NULL;
#define WINDOW_WIDTH 540
#define WINDOW_HEIGHT 340

namespace loader {

    uint64_t window_width = 540;
    uint64_t window_height = 340;

    bool login = false;

    bool login2 = false;

    bool window_update = false;

    char key[256];

    static IDXGISwapChain* swap_chain;
    static ID3D11Device* device;
    static ID3D11DeviceContext* context;
    static ID3D11RenderTargetView* rt_view;


}



static void set_swap_chain_size(int width, int height)
{
    ID3D11Texture2D* back_buffer;
    D3D11_RENDER_TARGET_VIEW_DESC desc;
    HRESULT hr;

    if (loader::rt_view)
        loader::rt_view->Release();

    loader::context->OMSetRenderTargets(0, NULL, NULL);

    hr = loader::swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR)
    {
        MessageBoxW(NULL, L"DXGI device is removed or reset!", L"Error", 0);
        exit(0);
    }
    assert(SUCCEEDED(hr));

    memset(&desc, 0, sizeof(desc));
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = loader::swap_chain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&back_buffer);
    assert(SUCCEEDED(hr));

    hr = loader::device->CreateRenderTargetView((ID3D11Resource*)back_buffer, &desc, &loader::rt_view);

    assert(SUCCEEDED(hr));

    back_buffer->Release();
}

static LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        if (loader::swap_chain)
        {
            int width = LOWORD(lparam);
            int height = HIWORD(lparam);
            set_swap_chain_size(width, height);
            nk_d3d11_resize(loader::context, width, height);
        }
        break;
    }

    if (nk_d3d11_handle_event(wnd, msg, wparam, lparam))
        return 0;

    return DefWindowProcW(wnd, msg, wparam, lparam);
}



int main()
{
    struct nk_context* ctx;
    struct nk_colorf bg;

    WNDCLASSW wc;
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;
    HWND wnd;
    int running = 1;
    HRESULT hr;
    D3D_FEATURE_LEVEL feature_level;
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"NuklearWindowClass";
    RegisterClassW(&wc);

    AdjustWindowRectEx(&rect, style, FALSE, exstyle);

    wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"NuklearWindowClass",style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,rect.right - rect.left, rect.bottom - rect.top,NULL, NULL, wc.hInstance, NULL);

    memset(&swap_chain_desc, 0, sizeof(swap_chain_desc));
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.OutputWindow = wnd;
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.Flags = 0;

    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &swap_chain_desc, &loader::swap_chain, &loader::device, &feature_level, &loader::context))) { hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &swap_chain_desc, &loader::swap_chain, &loader::device, &feature_level, &loader::context); assert(SUCCEEDED(hr)); } set_swap_chain_size(loader::window_width, loader::window_height); ctx = nk_d3d11_init(loader::device, loader::window_width, loader::window_height, MAX_VERTEX_BUFFER, MAX_INDEX_BUFFER); {struct nk_font_atlas* atlas; nk_d3d11_font_stash_begin(&atlas);  nk_d3d11_font_stash_end(); }

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while (running)
    {
        MSG msg;nk_input_begin(ctx);while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)){if (msg.message == WM_QUIT)running = 0;TranslateMessage(&msg);DispatchMessageW(&msg);}nk_input_end(ctx);

        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);
        int horizontal = desktop.right;
        int vertical = desktop.bottom;

        if (nk_begin(ctx, xorstr_("login plz"), nk_rect(0, 0, 540, 340), NK_WINDOW_BORDER | NK_WINDOW_TITLE | !NK_WINDOW_SCALABLE)) set_style(ctx, THEME_RED);
        {

            if (!loader::login)
            {
                nk_layout_row_dynamic(ctx, 25, 1);


                nk_label_colored(ctx, xorstr_(""), NK_TEXT_CENTERED, nk_color{ 209, 209, 209, 255 });
                nk_label(ctx, xorstr_("key"), NK_TEXT_CENTERED);
                nk_flags eventPassword = nk_edit_string_zero_terminated(ctx, NK_EDIT_BOX, loader::key, sizeof(loader::key), nk_filter_ascii); std::string key = loader::key;

                if (nk_button_label(ctx, xorstr_("Login")))
                {
                    loader::login = true;
                }
            }
        }
        nk_end(ctx);
        if (loader::login)
        {
            if (nk_begin(ctx, xorstr_("logged in"), nk_rect(0, 0, 540, 340), NK_WINDOW_BORDER | NK_WINDOW_TITLE | !NK_WINDOW_SCALABLE)) set_style(ctx, THEME_RED); 
            {
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label_colored(ctx, xorstr_("ver  1.0"), NK_TEXT_LEFT, nk_color{ 255, 255, 255, 255 });
                if (nk_button_label(ctx, xorstr_("Load")))
                {
                   // inject code
                }
            }
            nk_end(ctx);
        }




        loader::context->ClearRenderTargetView(loader::rt_view, &bg.r);
        loader::context->OMSetRenderTargets(1, &loader::rt_view, NULL);
        nk_d3d11_render(loader::context, NK_ANTI_ALIASING_ON);
        hr = loader::swap_chain->Present(1, 0);
        if (hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_REMOVED) {
            MessageBoxW(NULL, L"D3D11 device is lost or removed!", L"Error", 0);
            break;
        }
        else if (hr == DXGI_STATUS_OCCLUDED) {
            Sleep(10);
        }
        assert(SUCCEEDED(hr));
    }
    loader::context->ClearState();
    nk_d3d11_shutdown();
    loader::rt_view->Release();
    loader::context->Release();
    loader::device->Release();
    loader::swap_chain->Release();
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}