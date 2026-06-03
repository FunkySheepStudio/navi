#include "../include/cm93_wrapper.h"
#include "../include/ocpn_port_shim.h"
#include "../external/cm93.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <filesystem>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

static cm93compchart *g_cm93_chart = nullptr;
static bool g_cm93_chart_ready = false;

static void debug_log(const std::string &message) {
#ifdef _WIN32
    OutputDebugStringA(message.c_str());
#else
    fprintf(stderr, "%s", message.c_str());
#endif
}

static std::vector<std::filesystem::path> build_init_candidates(const char *cm93_dir) {
    std::vector<std::filesystem::path> candidates;
    if (cm93_dir && *cm93_dir) {
        candidates.emplace_back(cm93_dir);
    }
    try {
        candidates.push_back(std::filesystem::current_path());
        candidates.push_back(std::filesystem::current_path() / "cm93");
    } catch (...) {
    }
#ifdef _WIN32
    char exe_path[MAX_PATH] = {0};
    if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) > 0) {
        std::filesystem::path exe_dir = std::filesystem::path(exe_path).remove_filename();
        candidates.push_back(exe_dir);
        candidates.push_back(exe_dir / "cm93");
        auto parent = exe_dir;
        for (int i = 0; i < 5; ++i) {
            if (parent.has_parent_path()) {
                parent = parent.parent_path();
                candidates.push_back(parent);
                candidates.push_back(parent / "cm93");
            }
        }
    }
#endif
    return candidates;
}

extern "C" CM93_API int cm93_init(const char* cm93_dir) {
    if (g_cm93_chart) {
        delete g_cm93_chart;
        g_cm93_chart = nullptr;
    }
    g_cm93_chart = new cm93compchart();

    const std::vector<std::filesystem::path> candidates =
        build_init_candidates(cm93_dir);
    int last_ret = INIT_FAIL_NOFILE;
    for (const auto &candidate : candidates) {
        if (candidate.empty()) continue;
        const std::string pathStr = candidate.string();
        const wxString wxPath(pathStr.c_str());
        const int ret = static_cast<int>(g_cm93_chart->Init(wxPath, FULL_INIT));
        last_ret = ret;
        debug_log("cm93_init candidate: " + pathStr + " => " + std::to_string(ret) + "\n");
        if (ret == INIT_OK) {
            g_cm93_chart_ready = true;
            return ret;
        }
    }

    g_cm93_chart_ready = false;
    return last_ret;
}

static uint8_t* allocate_image_buffer(int width, int height) {
    size_t size = static_cast<size_t>(width) * static_cast<size_t>(height) * 4u;
    return static_cast<uint8_t*>(std::malloc(size));
}

static void fill_gradient(uint8_t* buf, int width, int height) {
    if (!buf) return;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float t = static_cast<float>(y) / static_cast<float>(height - 1);
            uint8_t r = static_cast<uint8_t>(20 + 30 * (1.0f - t));
            uint8_t g = static_cast<uint8_t>(80 + 80 * (1.0f - t));
            uint8_t b = static_cast<uint8_t>(150 + 80 * (1.0f - t));
            int idx = (y * width + x) * 4;
            buf[idx + 0] = r;
            buf[idx + 1] = g;
            buf[idx + 2] = b;
            float dx = (2.0f * static_cast<float>(x) / static_cast<float>(width - 1) - 1.0f);
            float dy = (2.0f * static_cast<float>(y) / static_cast<float>(height - 1) - 1.0f);
            float dist = std::sqrt(dx * dx + dy * dy);
            buf[idx + 3] = static_cast<uint8_t>(255 * (1.0f - 0.6f * dist));
        }
    }
}

extern "C" CM93_API uint8_t* cm93_render(int width, int height, double lat, double lon, int zoom) {
    if (width <= 0 || height <= 0) return nullptr;

    wxBitmap bitmap(width, height, 32);
    wxMemoryDC dc;
    dc.SelectObject(bitmap);
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    ViewPort vp;
    vp.pix_width = width;
    vp.pix_height = height;
    vp.clat = lat;
    vp.clon = lon;
    vp.view_scale_ppm = (zoom > 0) ? static_cast<double>(zoom) : 1.0;
    vp.chart_scale = 1000000;
    vp.rotation = 0.0;
    vp.m_projection_type = PROJECTION_MERCATOR;
    vp.b_quilt = true;
    vp.b_FullScreenQuilt = true;

    const OCPNRegion region(0, 0, width, height);
    bool rendered = false;
    if (g_cm93_chart_ready && g_cm93_chart) {
        rendered = g_cm93_chart->RenderRegionViewOnDC(dc, vp, region);
    }

    dc.SelectObject(wxNullBitmap);

    uint8_t* buf = allocate_image_buffer(width, height);
    if (!buf) return nullptr;
    const uint8_t* pixels = bitmap.GetPixels();
    if (pixels) {
        std::memcpy(buf, pixels, static_cast<size_t>(width) * static_cast<size_t>(height) * 4u);
    } else {
        std::memset(buf, 0, static_cast<size_t>(width) * static_cast<size_t>(height) * 4u);
    }

    if (!rendered) {
        fill_gradient(buf, width, height);
    }
    return buf;
}

extern "C" CM93_API void cm93_free_buffer(uint8_t* buf) {
    if (buf) {
        std::free(buf);
    }
}

extern "C" CM93_API int cm93_version() {
    return 1;
}
