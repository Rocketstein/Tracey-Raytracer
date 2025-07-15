#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "color.h"
#include "env.h"
#include "objects/objs.h"
#include "objects/world.h"
#include "objects/sphere.h"
#include "objects/quad.h"
#include "objects/bvh/aabb.h"
#include "objects/bvh/bvh.h"
#include "material/diffuse.h"
#include "material/metal.h"
#include "material/dielectric.h"
#include "material/bulb.h"
#include "texture/texture.h"

// Libraries
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "src/lib/stb_image_write.h"
#include "src/lib/stb_image.h"
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_win32.h"
#include "lib/imgui/imgui_impl_dx11.h"

using std::unordered_map;


inline void flatten_world(world& dest, const world& src) {
    // Merges a world to another world as a collection of objects
    for (const auto& obj : src.objects) {
        dest.insert(obj);
    }
}

inline shared_ptr<world> box(const vec3& a, const vec3& b, shared_ptr<material> mat) {
    // Returns the 3D box containing the two opposite vertices a & b.
    auto sides = make_shared<world>();
    auto min = vec3(std::fmin(a.x(),b.x()), std::fmin(a.y(),b.y()), std::fmin(a.z(),b.z()));
    auto max = vec3(std::fmax(a.x(),b.x()), std::fmax(a.y(),b.y()), std::fmax(a.z(),b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->insert(make_shared<Quad>(vec3(min.x(), min.y(), max.z()),  dx,  dy, mat));       // front
    sides->insert(make_shared<Quad>(vec3(max.x(), min.y(), max.z()), -1 * dz,  dy, mat));   // right
    sides->insert(make_shared<Quad>(vec3(max.x(), min.y(), min.z()), -1 * dx,  dy, mat));   // back
    sides->insert(make_shared<Quad>(vec3(min.x(), min.y(), min.z()),  dz,  dy, mat));       // left
    sides->insert(make_shared<Quad>(vec3(min.x(), max.y(), max.z()),  dx, -1 * dz, mat));   // top
    sides->insert(make_shared<Quad>(vec3(min.x(), min.y(), min.z()),  dx,  dz, mat));       // bottom

    return sides;
}

// Forward declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// D3D11 globals
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

int main(int argc, char** argv) {
    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Rasterizer", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"3D Rasterizer", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Initialize variables
    bool    show_preview       = true;
    int     image_width        = 800;
    int     image_height       = 800;
    int     channels           = 3;
    int     aa_factor          = 150;
    int     max_recursion      = 40;
    int     FOV                = 20;
    float   dof_angle          = 0.0;
    vec3    background_col     = vec3(0, 0, 0);

    // Initialize world
    world world_list;
    unordered_map<std::string, shared_ptr<material>> materials_list;
    unordered_map<std::string, shared_ptr<objs>> objects_list;
    unordered_map<std::string, shared_ptr<world>> complex_objects_list;
    vector<unsigned char> image(image_width * image_height * channels);
    camera cam(image_width, image_height, image, FOV, dof_angle, background_col, aa_factor, max_recursion);

    // Variables for texture display
    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* texture_srv = nullptr;

    // Main loop
    bool done = false;
    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Create main control panel
        ImGui::Begin("Tracey Controls");
        
        // Material
        ImGui::Text("Material:");
        static char material_name[256] = "";
        ImGui::InputText("Material Name", material_name, sizeof(material_name));

        // // Set Material Type
        static const char* material_types[] = { "diffuse", "metal", "dielectric", "bulb", "texture" };
        static int current_material  = 0; // Index of currently selected item
        ImGui::Combo("Material Type", &current_material, material_types, IM_ARRAYSIZE(material_types));

        // // Set Material Properties
        static int albedo[3] = {0, 0, 0};
        ImGui::InputInt3("Albedo", albedo);
        static double fuzz_or_refraction = 0; // For Metal & Dielectric materials only
        ImGui::InputDouble("Fuzziness/Refraction index", &fuzz_or_refraction);

        // // Set Textures
        static char tex_path[256] = "texture_images/";
        if (current_material == 4) {
            ImGui::InputText("Texture File", tex_path, sizeof(tex_path));
        }

        // // Create Material and add it to the list
        if (ImGui::Button("Create Material")) {
            vec3 albedo_vec(albedo[0], albedo[1], albedo[2]);
            std::shared_ptr<material> mat;
            
            switch(current_material) {
                case 0: mat = std::make_shared<diffuse>(albedo_vec); break;
                case 1: mat = fuzz_or_refraction ? std::make_shared<metal>(albedo_vec, fuzz_or_refraction) 
                                                : std::make_shared<metal>(albedo_vec); break;
                case 2: mat = std::make_shared<dielectric>(fuzz_or_refraction); break;
                case 3: mat = std::make_shared<Bulb>(albedo_vec); break;
                case 4: {
                    auto mat_texture = make_shared<ImageTexture>(tex_path);
                    mat = make_shared<diffuse>(mat_texture);
                    break;
                }
            }
            
            materials_list[material_name] = mat;
        }

        // // Remove Material by Name
        static char remove_name[256] = "";
        ImGui::InputText("Remove", remove_name, sizeof(remove_name));
        if (ImGui::Button("Remove Material")) {
            materials_list.erase(remove_name);
        }
        ImGui::Separator();



        // Object
        ImGui::Text("Objects:");

        // // Set Object Name
        static char object_name[256] = "";
        ImGui::InputText("Object Name", object_name, sizeof(object_name));

        // // Set Object Type
        static const char* object_types[] = { "sphere", "quad", "box" };
        static int current_object_type  = 0; // Index of currently selected item
        ImGui::Combo("Object Type", &current_object_type, object_types, IM_ARRAYSIZE(object_types));

        // // Set Position
        static int position[3] = {0, 0, 0};
        ImGui::InputInt3("Position", position);

        // // Set Radius (Sphere only)
        static int radius = 0;
        ImGui::InputInt("Sphere Radius", &radius);

        // // Set u, v index (Quads only)
        static int quad_u[3] = {0, 0, 0};
        ImGui::InputInt3("Quad u", quad_u);
        static int quad_v[3] = {0, 0 , 0};
        ImGui::InputInt3("Quad v", quad_v);

        // // Set Opposite Vertex (Box only)
        static int position2[3] = {0, 0, 0};
        ImGui::InputInt3("Opposite Position", position2);

        // // Set Rotation
        ImGui::Text("Object Transformations:");
        static float x_rotation = 0;
        static float y_rotation = 0;
        ImGui::SliderFloat("X Rotation", &x_rotation, -180.0f, 180.0f, "%.1f°");
        ImGui::SliderFloat("Y Rotation", &y_rotation, -180.0f, 180.0f, "%.1f°");

        // // Set Object Material
        // Create vector of material names for the combo
        std::vector<std::string> material_names;
        for (const auto& pair : materials_list) {
            material_names.push_back(pair.first);
        }
        
        // Convert to const char* array for ImGui
        static int material_index = 0;
        std::vector<const char*> material_c_strs;
        for (const auto& name : material_names) {
            material_c_strs.push_back(name.c_str());
        }
        
        // Clamp selection to valid range
        if (material_index >= material_names.size()) {
            material_index = 0;
        }
        
        ImGui::Combo("Material", &material_index, material_c_strs.data(), material_c_strs.size());

        // // Create Material
        if (ImGui::Button("Create Object")) {
            std::shared_ptr<objs> object;
            auto material_name = material_names[material_index];
            auto mat = materials_list[material_name];
            auto where = vec3(position[0], position[1], position[2]);
            
            switch(current_object_type) {
                case 0: {
                    // Sphere
                    object = make_shared<sphere>(radius, where, mat);
                    object->rotate(x_rotation, 'x');
                    object->rotate(y_rotation, 'y');
                    objects_list[object_name] = object;
                    break;
                }
                case 1: {
                    // Quad
                    object = make_shared<Quad>(where, vec3(quad_u[0], quad_u[1], quad_u[2]), vec3(quad_v[0], quad_v[1], quad_v[2]), mat);
                    object->rotate(x_rotation, 'x');
                    object->rotate(y_rotation, 'y');
                    objects_list[object_name] = object;
                    break;
                }
                case 2: {
                    // Box
                    std::shared_ptr<world> complex_object;
                    complex_object = box(where, vec3(position2[0], position2[1], position2[2]), mat);
                    complex_object->rotate(x_rotation, 'x');
                    complex_object->rotate(y_rotation, 'y');
                    complex_objects_list[object_name] = complex_object;
                    break;
                }
            }
        }
        ImGui::Separator();



        // Camera
        ImGui::Text("World/Camera Settings:");

        // // Image Size
        if (ImGui::SliderInt("Width", &image_width, 100, 2048)) {
            // Resize image vector when dimensions change
            image.resize(image_width * image_height * channels);
        }
        if (ImGui::SliderInt("Height", &image_height, 100, 2048)) {
            image.resize(image_width * image_height * channels);
        }

        // // Camera Position
        static int camera_position_arr[3] = {0, 0, 0};
        ImGui::InputInt3("Camera Position", camera_position_arr);
        auto camera_position = vec3(camera_position_arr[0], camera_position_arr[1], camera_position_arr[2]);

        // // Camera Lookat
        static int lookat_arr[3] = {0, 0, -100};
        ImGui::InputInt3("Look at", lookat_arr);
        auto lookat = vec3(lookat_arr[0], lookat_arr[1], lookat_arr[2]);

        // // Background Color
        static float background_color_arr[3] = {0, 0, 0};
        ImGui::InputFloat3("Background Color", background_color_arr);
        background_col = vec3(background_color_arr[0], background_color_arr[1], background_color_arr[2]);

        // // Field of View
        ImGui::InputInt("FOV", &FOV);

        // // Depth of Field
        ImGui::InputFloat("DOF", &dof_angle);

        // // Anti-Aliasing factor
        ImGui::InputInt("AA-Factor", &aa_factor);

        // // Recursion depth
        ImGui::InputInt("Recursion depth", &max_recursion);

        // // Finalize Settings
        if(ImGui::Button("Save Changes")) {
            image.resize(image_width * image_height * channels);
            cam.~camera();
            new(&cam) camera(image_width, image_height, image, FOV, dof_angle, background_col, aa_factor, max_recursion);
        }
        ImGui::Separator();


        
        // Render controls
        if (ImGui::Button("Render Image")) {
            try {
                // Clear the image
                std::fill(image.begin(), image.end(), 0);
                world_list.wipe();

                // Fill objs to world list
                for (const auto& o : objects_list) {
                    world_list.insert(o.second);
                }

                for (const auto& o : complex_objects_list) {
                    flatten_world(world_list, *o.second);
                }

                // Apply BVH
                // Refrain from applying BVH to empty world
                if (!objects_list.empty() || !complex_objects_list.empty()) {
                    world_list = world(make_shared<BoundingVolumeNode>(world_list));
                }
                
                // Initialize renderer with current settings
                cam.render(world_list, camera_position, lookat);
                
                // Update texture for display
                if (texture) {
                    texture->Release();
                    texture = nullptr;
                }
                
                // Create texture from rendered image (RGB format)
                D3D11_TEXTURE2D_DESC desc   = {};
                desc.Width                  = image_width;
                desc.Height                 = image_height;
                desc.MipLevels              = 1;
                desc.ArraySize              = 1;
                desc.Format                 = DXGI_FORMAT_R8G8B8A8_UNORM;
                desc.SampleDesc.Count       = 1;
                desc.Usage                  = D3D11_USAGE_DEFAULT;
                desc.BindFlags              = D3D11_BIND_SHADER_RESOURCE;
                
                vector<unsigned char> packed_image(image_width * image_height * 4);
                for (int i = 0; i < image_width * image_height; i++) {
                    packed_image[i * 4 + 0] = image[i * 3 + 0]; // R
                    packed_image[i * 4 + 1] = image[i * 3 + 1]; // G
                    packed_image[i * 4 + 2] = image[i * 3 + 2]; // B
                    packed_image[i * 4 + 3] = 255;              // A (fully opaque)
                }
                
                D3D11_SUBRESOURCE_DATA subResource = {};
                subResource.pSysMem = packed_image.data();
                subResource.SysMemPitch = image_width * 4;
                
                HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &texture);
                if (SUCCEEDED(hr)) {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MipLevels = desc.MipLevels;
                    hr = g_pd3dDevice->CreateShaderResourceView(texture, &srvDesc, &texture_srv);

                    if (FAILED(hr)) {
                        texture->Release();
                        texture = nullptr;
                    }

                } else {
                    std::cerr << "Failed to create texture, HRESULT: 0x" << std::hex << hr << std::endl;
                }
            } catch (const std::exception& e) {
                // Handle errors - you might want to display this in the UI
                std::cerr << "Render error: " << e.what() << std::endl;
            }
        }

        // Show preview image after rendering
        ImGui::Checkbox("Show Preview", &show_preview);
        
        // Export button
        if (ImGui::Button("Export PNG")) {
            if (!image.empty()) {
                int stride = image_width * channels;
                cam.export_image(image, image_width, image_height, stride);
            }
        }
        
        ImGui::End();
        
        // Display the rendered image
        if (texture_srv && show_preview) {
            ImGui::Begin("Rendered Image");
            
            // Maintain window aspect ratio
            ImVec2 window_size = ImGui::GetContentRegionAvail();
            float aspect_ratio = (float)image_width / (float)image_height;
            ImVec2 display_size;
            
            if (window_size.x / aspect_ratio <= window_size.y) {
                display_size.x = window_size.x;
                display_size.y = window_size.x / aspect_ratio;
            } else {
                display_size.x = window_size.y * aspect_ratio;
                display_size.y = window_size.y;
            }
            
            ImGui::Image((void*)texture_srv, display_size);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0); // Present with vsync
    }

    // Cleanup
    if (texture) texture->Release();
    if (texture_srv) texture_srv->Release();
    
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions for D3D11 setup
bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount                          = 2;
    sd.BufferDesc.Width                     = 0;
    sd.BufferDesc.Height                    = 0;
    sd.BufferDesc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator     = 60;
    sd.BufferDesc.RefreshRate.Denominator   = 1;
    sd.Flags                                = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage                          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                         = hWnd;
    sd.SampleDesc.Count                     = 1;
    sd.SampleDesc.Quality                   = 0;
    sd.Windowed                             = TRUE;
    sd.SwapEffect                           = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
        &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
            featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
            &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    
    if (res != S_OK)
        return false;

    // Create render target
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();

    return true;
}

void CleanupDeviceD3D() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
            if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            ID3D11Texture2D* pBackBuffer;
            g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
            pBackBuffer->Release();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

// g++ -fopenmp -I src -o raytracer main.cpp src/vec3.cpp src/color.cpp src/env.cpp src/ray.cpp src/util.cpp src/objects/objs.cpp src/objects/sphere.cpp src/objects/quad.cpp src/objects/world.cpp src/material/material.cpp src/material/diffuse.cpp src/material/metal.cpp src/material/dielectric.cpp src/material/bulb.cpp src/texture/texture.cpp src/objects/bvh/aabb.cpp src/objects/bvh/bvh.cpp src/lib/imgui/imgui.cpp src/lib/imgui/imgui_demo.cpp src/lib/imgui/imgui_draw.cpp src/lib/imgui/imgui_tables.cpp src/lib/imgui/imgui_widgets.cpp src/lib/imgui/imgui_impl_win32.cpp src/lib/imgui/imgui_impl_dx11.cpp -ld3d11 -ldxgi -ld3dcompiler -lgdi32 -ldwmapi  

// g++ -I src -o raytracer main.cpp src/vec3.cpp src/color.cpp src/env.cpp src/ray.cpp src/util.cpp src/objects/objs.cpp src/objects/sphere.cpp src/objects/quad.cpp src/objects/world.cpp src/material/material.cpp src/material/diffuse.cpp src/material/metal.cpp src/material/dielectric.cpp src/material/bulb.cpp src/texture/texture.cpp src/objects/bvh/aabb.cpp src/objects/bvh/bvh.cpp src/lib/imgui/imgui.cpp src/lib/imgui/imgui_demo.cpp src/lib/imgui/imgui_draw.cpp src/lib/imgui/imgui_tables.cpp src/lib/imgui/imgui_widgets.cpp src/lib/imgui/imgui_impl_win32.cpp src/lib/imgui/imgui_impl_dx11.cpp -ld3d11 -ldxgi -ld3dcompiler -lgdi32 -ldwmapi
// ./raytracer