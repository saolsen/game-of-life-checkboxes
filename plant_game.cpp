#define SOKOL_IMPL
#if defined(_WIN32)
#define SOKOL_LOG(s) OutputDebugStringA(s)
#endif
/* this is only needed for the debug-inspection headers */
#define SOKOL_TRACE_HOOKS
/* sokol 3D-API defines are provided by build options */
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"
//#include "sokol_audio.h"
//#include "sokol_fetch.h"
#include "imgui.h"
#include "sokol_glue.h"
// #include "imgui_font.h"
#define SOKOL_IMGUI_IMPL
#include <string.h>

#include "sokol_imgui.h"

static bool show_test_window = true;
static bool show_another_window = false;
static bool show_quit_dialog = false;
static bool html5_ask_leave_site = false;

static sg_pass_action pass_action;

#define NUM_CELLS 32

static bool last_frame[NUM_CELLS][NUM_CELLS];
static bool cells[NUM_CELLS][NUM_CELLS];

void init(void) {
  // setup sokol-gfx and sokol-time
  sg_desc desc = {};
  desc.context = sapp_sgcontext();
  sg_setup(&desc);

  // setup sokol-imgui, but provide our own font
  simgui_desc_t simgui_desc = {};
  // simgui_desc.no_default_font = true;
  simgui_desc.dpi_scale = sapp_dpi_scale();
  simgui_setup(&simgui_desc);

  // configure Dear ImGui with our own embedded font
  auto &io = ImGui::GetIO();
  /*ImFontConfig fontCfg;
  fontCfg.FontDataOwnedByAtlas = false;
  fontCfg.OversampleH = 2;
  fontCfg.OversampleV = 2;
  fontCfg.RasterizerMultiply = 1.5f;*/
  // io.Fonts->AddFontFromMemoryTTF(dump_font, sizeof(dump_font), 16.0f,
  // &fontCfg);

  // create font texture for the custom font
  unsigned char *font_pixels;
  int font_width, font_height;
  io.Fonts->GetTexDataAsRGBA32(&font_pixels, &font_width, &font_height);
  sg_image_desc img_desc = {};
  img_desc.width = font_width;
  img_desc.height = font_height;
  img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
  img_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
  img_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
  img_desc.min_filter = SG_FILTER_LINEAR;
  img_desc.mag_filter = SG_FILTER_LINEAR;
  img_desc.data.subimage[0][0].ptr = font_pixels;
  img_desc.data.subimage[0][0].size = font_width * font_height * 4;
  io.Fonts->TexID = (ImTextureID)(uintptr_t)sg_make_image(&img_desc).id;

  // initial clear color
  pass_action.colors[0].action = SG_ACTION_CLEAR;
  pass_action.colors[0].value = {0.3f, 0.7f, 0.0f, 1.0f};

  // set all to 0
  for (int y = 0; y < NUM_CELLS; y++) {
    for (int x = 0; x < NUM_CELLS; x++) {
      cells[y][x] = x > 0 && y > 0 && x / y == 0;
    }
  }
}

int x = 0;

int x_offset = 300;
int y_offset = 50;
int cell_size = 10;

bool running = false;

void frame(void) {
  memcpy(last_frame, cells, sizeof(bool) * NUM_CELLS * NUM_CELLS);

  if (running && x % 10 == 0) {
    // game of life
    for (int y = 0; y < NUM_CELLS; y++) {
      for (int x = 0; x < NUM_CELLS; x++) {
        bool alive = last_frame[y][x];
        int n = 0;
        // loop over neighboring cells,
        for (int i = y - 1; i <= y + 1; i++) {
          for (int j = x - 1; j <= x + 1; j++) {
            // Don't check self
            if (i == y && j == x) continue;
            // Watch out for the border
            if (i < 0 || i >= NUM_CELLS) continue;
            if (j < 0 || j >= NUM_CELLS) continue;
            if (last_frame[i][j]) {
              n++;
            }
          }
        }
        if (alive) {
          if (n == 2 || n == 3) {
            // stay alive
            cells[y][x] = true;
          } else {
            cells[y][x] = false;
          }
        } else {
          if (n == 3) {
            cells[y][x] = true;
          } else {
            cells[y][x] = false;
          }
        }
      }
    }
  }

  x++;
  const int width = sapp_width();
  const int height = sapp_height();
  simgui_new_frame(width, height, 1.0 / 60.0);

  ImVec2 p = ImGui::GetCursorScreenPos();
  ImDrawList *draw = ImGui::GetBackgroundDrawList();

  // draw->AddRectFilled(ImVec2(x_offset, y_offset),ImVec2(x_offset + cell_size * NUM_CELLS, y_offset + cell_size *
  // NUM_CELLS), IM_COL32_WHITE, 0.f, 0);

  /* for (int y = 0; y < NUM_CELLS; y++) {
    for (int x = 0; x < NUM_CELLS; x++) {
      ImVec2 min =
          ImVec2((float)x * cell_size - cell_size / 2 + x_offset, (float)y * cell_size - cell_size / 2 + y_offset);
      ImVec2 max =
          ImVec2((float)x * cell_size + cell_size / 2 + x_offset, (float)y * cell_size + cell_size / 2 + y_offset);
      int color;
      if (cells[y][x]) {
        color = IM_COL32_WHITE;
      } else {
        color = IM_COL32_BLACK;
      }
      draw->AddRectFilled(min, max, color, 0.f, 0);
    }
  } */

  // draw->AddRectFilled(ImVec2(p.x - 10., p.y - 10.), ImVec2(p.x + 10., p.y + 10.), IM_COL32_WHITE, 0.f, 0);

  // ImGui::Begin("Plant Test");
  // ImGui::Text("Plant Testing");
  // ImGui::Checkbox("Running", &running);
  /* ImGui::Text("%i", x);
  ImGui::Text("Cursor At: (%f, %f)", p.x, p.y);
  ImGui::SliderInt("x_offset", &x_offset, 0, 1000);
  ImGui::SliderInt("y_offset", &y_offset, 0, 1000);
  ImGui::SliderInt("cell_size", &cell_size, 0, 1000); */
  // ImGui::End();

  ImGui::Begin("Plants");
  ImGui::Checkbox("Running", &running);
  for (int y = 0; y < NUM_CELLS; y++) {
    for (int x = 0; x < NUM_CELLS; x++) {
      bool *cell = &(cells[y][x]);
      ImGui::PushID(cell);
      ImGui::Checkbox("", cell);
      ImGui::PopID();
      if (x != NUM_CELLS - 1) {
        ImGui::SameLine();
      }
    }
  }
  ImGui::End();

  // 1. Show a simple window
  // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a
  // window automatically called "Debug" static float f = 0.0f;
  // ImGui::Text("Hello, world!");
  // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  // ImGui::ColorEdit3("clear color", &pass_action.colors[0].value.r);
  // ImGui::Text("width: %d, height: %d\n", sapp_width(), sapp_height());
  // if (ImGui::Button("Test Window")) show_test_window ^= 1;
  // if (ImGui::Button("Another Window")) show_another_window ^= 1;
  // ImGui::Text("NOTE: programmatic quit isn't supported on mobile");
  // if (ImGui::Button("Soft Quit")) {
  //    sapp_request_quit();
  //}
  // if (ImGui::Button("Hard Quit")) {
  //    sapp_quit();
  //}
  // if (ImGui::Checkbox("HTML5 Ask Leave Site", &html5_ask_leave_site)) {
  //    sapp_html5_ask_leave_site(html5_ask_leave_site);
  //}
  // if (ImGui::Button(sapp_is_fullscreen() ? "Switch to windowed" : "Switch to
  // fullscreen")) {
  //    sapp_toggle_fullscreen();
  //}
  // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f /
  // ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  // 2. Show another simple window, this time using an explicit Begin/End pair
  /*if (show_another_window) {
      ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
      ImGui::Begin("Another Window", &show_another_window);
      ImGui::Text("Hello");
      ImGui::End();
  }*/

  // 3. Show the ImGui test window. Most of the sample code is in
  // ImGui::ShowDemoWindow() if (show_test_window) {
  //    ImGui::SetNextWindowPos(ImVec2(460, 20), ImGuiCond_FirstUseEver);
  //    ImGui::ShowDemoWindow();
  //}

  // 4. Prepare and conditionally open the "Really Quit?" popup
  if (ImGui::BeginPopupModal("Really Quit?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Do you really want to quit?\n");
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(120, 0))) {
      sapp_quit();
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  if (show_quit_dialog) {
    ImGui::OpenPopup("Really Quit?");
    show_quit_dialog = false;
  }

  // the sokol_gfx draw pass
  sg_begin_default_pass(&pass_action, width, height);
  simgui_render();
  sg_end_pass();
  sg_commit();
}

void cleanup(void) {
  simgui_shutdown();
  sg_shutdown();
}

void input(const sapp_event *event) {
  if (event->type == SAPP_EVENTTYPE_QUIT_REQUESTED) {
    show_quit_dialog = true;
    sapp_cancel_quit();
  } else {
    simgui_handle_event(event);
  }
}

sapp_desc sokol_main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  sapp_desc desc = {};
  desc.init_cb = init;
  desc.frame_cb = frame;
  desc.cleanup_cb = cleanup;
  desc.event_cb = input;
  desc.width = 1024;
  desc.height = 768;
  desc.fullscreen = false;
  desc.high_dpi = true;
  desc.html5_ask_leave_site = html5_ask_leave_site;
  desc.ios_keyboard_resizes_canvas = false;
  desc.gl_force_gles2 = false;
  desc.window_title = "Plant Game";
  // todo
  desc.icon.sokol_default = true;
  return desc;
}