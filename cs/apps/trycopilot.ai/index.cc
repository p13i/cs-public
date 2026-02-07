// cs/apps/trycopilot.ai/index.cc
#include <stdint.h>
#include <stdio.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <SDL/SDL.h>
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/html5.h>
#include <emscripten/key_codes.h>
#endif  // __EMSCRIPTEN__

#include "cs/apps/trycopilot.ai/emscripten/emscripten.hh"
#include "cs/apps/trycopilot.ai/protos/auth.proto.hh"
#include "cs/apps/trycopilot.ai/protos/game.proto.hh"
#include "cs/apps/trycopilot.ai/protos/user.proto.hh"
#include "cs/apps/trycopilot.ai/protos/uuid.proto.hh"
#include "cs/apps/trycopilot.ai/scene1.hh"
#include "cs/apps/trycopilot.ai/scene_animator.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "cs/renderer/film.hh"
#include "cs/renderer/pixel.hh"
#include "cs/result.hh"
#include "cs/util/timeit.hh"

namespace {  // use_usings
using ::cs::apps::trycopilotai::SceneAnimator;
using ::cs::apps::trycopilotai::protos::GetAnimationRequest;
using ::cs::apps::trycopilotai::protos::
    GetAnimationResponse;
using ::cs::apps::trycopilotai::protos::GetUuidRequest;
using ::cs::apps::trycopilotai::protos::GetUuidResponse;
using ::cs::apps::trycopilotai::protos::LoginRequest;
using ::cs::apps::trycopilotai::protos::LoginResponse;
using ::cs::apps::trycopilotai::text::fonts::
    SampleCharacterPixel;
using ::cs::renderer::Film;
using ::cs::renderer::Pixel;
using ::cs::renderer::linalg::Transform;
using ::cs::renderer::linalg::transforms::Rotate;
using ::cs::renderer::linalg::transforms::RotateY;
}  // namespace

static bool scene_changed = true;
static const float focal_length = 50;
static p3 pos(0, 0, -10);
static p3 look(0, 0, 0);
static p3 up(0, 1, 0);
static Scene scene({
    // Unit sphere at the origin.
    new Sphere(p3(0, 0, 0), 1),
    // Walls on left and right.
    new Plane(p3(+1, 0, 0).unit(), 10),
    new Plane(p3(-1, 0, 0).unit(), 10),
    // Walls on ceiling and floor.
    new Plane(p3(0, +1, 0).unit(), 5),
    new Plane(p3(0, -1, 0).unit(), 5),
    // Walls in front.
    new Plane(p3(0, 0, 1).unit(), -30),
    // new Plane(p3(+1, 0, 0).unit(), +5),
});
static std::string message = "GAME START";
#if __EMSCRIPTEN__
static SDL_Surface* screen = nullptr;
#endif  // __EMSCRIPTEN__

template <typename T>
void FillWindow(T** src, T** dst, size_t sw, size_t sh,
                size_t dw, size_t dh, size_t dx,
                size_t dy) {
  const size_t cw = std::min(sw, dw - dx);
  const size_t ch = std::min(sh, dh - dy);
  for (size_t y = 0; y < ch; ++y) {
    // keep x inner for better cache behavior on [x][y]
    // layout
    for (size_t x = 0; x < cw; ++x) {
      dst[dx + x][dy + y] = src[x][y];
    }
  }
}

void FillFilm(std::unique_ptr<Film> source,
              Film* destination, size_t destinationXStart,
              size_t destinationYStart) {
  FillWindow(source->pixels, destination->pixels,
             source->width, source->height,
             destination->width, destination->height,
             destinationXStart, destinationYStart);
}

void GetFrame();

// === Movement Helpers ===
void MoveLeft() {
  v3 cross = cs::renderer::geo::cross(pos - look, up);
  p3 delta = -1 * cross.unit().point();
  pos += delta;
  look += delta;
  scene_changed = true;
  message = "MOVED LEFT";
}

void MoveRight() {
  v3 cross = cs::renderer::geo::cross(pos - look, up);
  p3 delta = +1 * cross.unit().point();
  pos += delta;
  look += delta;
  scene_changed = true;
  message = "MOVED RIGHT";
}

void MoveUp() {
  p3 delta = +1 * up.unit();
  pos += delta;
  look += delta;
  scene_changed = true;
  message = "MOVED UP";
}

void MoveDown() {
  p3 delta = -1 * up.unit();
  pos += delta;
  look += delta;
  scene_changed = true;
  message = "MOVED DOWN";
}

void MoveForward() {
  p3 delta = -1 * (pos - look).unit();
  pos += delta;
  look += delta;
  scene_changed = true;
  message = "MOVED FORWARD";
}

void MoveBack() {
  p3 delta = +1 * (pos - look).unit();
  pos += delta;
  look += delta;
  scene_changed = true;
  message = "MOVED BACK";
}

// === Rotation Helpers ===
void RotateLeft() {
  auto d = v3(look - pos).normalized().point();
  auto d_prime = RotateY(-1 * 5.f * PIf / 180.f)(d);
  if (d_prime.ok()) {
    look =
        pos + v3(look - pos).magnitude() * d_prime.value();
    scene_changed = true;
    message = "ROTATE LEFT";
  }
}

void RotateRight() {
  auto d = v3(look - pos).normalized().point();
  auto d_prime = RotateY(+1 * 5.f * PIf / 180.f)(d);
  if (d_prime.ok()) {
    look =
        pos + v3(look - pos).magnitude() * d_prime.value();
    scene_changed = true;
    message = "ROTATE RIGHT";
  }
}

// === API Helpers ===
void FetchUuid() {
  FETCH_API(GET, "/api/uuid/", GetUuidRequest,
            GetUuidResponse, GetUuidRequest(),
            [&](ResultOr<GetUuidResponse> result) {
              if (!result.ok()) {
                LOG(ERROR) << result << ENDL;
                return;
              }
              LOG(INFO)
                  << "Got UUID: " << result.value().uuid
                  << ENDL;
            });

  scene_changed = false;
  message = "FETCH API CALLED";
}

void LoginApi() {
  LoginRequest request;
  request.email = "pramod@p13i.io";
  request.password = "p13i";
  FETCH_API(
      POST, "/api/login/", LoginRequest, LoginResponse,
      request, [&](ResultOr<LoginResponse> result) {
        if (!result.ok()) {
          LOG(ERROR) << result << ENDL;
          return;
        }
        LOG(INFO) << "Got login response: "
                  << result.value().Serialize() << ENDL;
      });

  scene_changed = false;
  message = "LOGIN API CALLED";
}

void RenderFrame() {
  GetFrame();
  scene_changed = false;
  message = "RENDER API CALLED";
}

#ifdef __EMSCRIPTEN__
// === Key Callback ===
EM_BOOL key_callback(int eventType,
                     const EmscriptenKeyboardEvent* e,
                     void* userData) {
  switch (e->keyCode) {
    case DOM_VK_LEFT:
      MoveLeft();
      break;
    case DOM_VK_RIGHT:
      MoveRight();
      break;
    case DOM_VK_UP:
      MoveUp();
      break;
    case DOM_VK_DOWN:
      MoveDown();
      break;
    case DOM_VK_EQUALS:
    case 187:
      MoveForward();
      break;
    case DOM_VK_HYPHEN_MINUS:
    case 189:
      MoveBack();
      break;
    case DOM_VK_OPEN_BRACKET:
      RotateLeft();
      break;
    case DOM_VK_CLOSE_BRACKET:
      RotateRight();
      break;
    case DOM_VK_U:
      FetchUuid();
      break;
    case DOM_VK_L:
      LoginApi();
      break;
    case DOM_VK_R:
      RenderFrame();
      break;
    default:
      break;
  }

  // Prevent default browser behavior if the scene changed.
  return scene_changed ? EM_TRUE : EM_FALSE;
}
#endif  // __EMSCRIPTEN__

std::mutex queued_frame_mutex;
static std::unique_ptr<Film> queued_frame;

static std::atomic<size_t> tile_index{0};

void QueueFrame(GetAnimationResponse response) {
  LOG(DEBUG) << "Queuing frame." << ENDL;
  std::lock_guard<std::mutex> lock(queued_frame_mutex);
  TIME_IT_START;
  queued_frame = std::make_unique<Film>(
      response.request.width, response.request.height);
  TIME_IT_END("Allocating " +
              std::to_string(response.request.width) + "x" +
              std::to_string(response.request.height) +
              " took ");
  for (unsigned int x = 0; x < queued_frame->width; x++) {
    for (unsigned int y = 0; y < queued_frame->height;
         y++) {
      // LOG(DEBUG) << "Setting pixel" << ENDL;
      auto pixel = response.data[0][x][y];
      queued_frame->pixels[x][y] =
          Pixel(pixel[0], pixel[1], pixel[2], pixel[3]);
    }
  }
  LOG(DEBUG) << "Set all queued_frame->pixels" << ENDL;
  scene_changed = true;
  {
    tile_index =
        (tile_index.load(std::memory_order_relaxed) + 1) %
        (APP_SCREEN_WIDTH / response.request.width);
    LOG(DEBUG) << "Tile index is: " << tile_index << ENDL;
  }

  // GetFrame();
}

void GetFrame() {
  // Get a render frame:
  // /api/game/animation/?width=4&height=4&num_frames=1
  GetAnimationRequest request;
  request.width = 16;
  request.height = 16;
  request.num_frames = 1;
  FETCH_API(
      GET, "/api/game/animation/", GetAnimationRequest,
      GetAnimationResponse, request,
      [&](ResultOr<GetAnimationResponse> result) {
        if (!result.ok()) {
          LOG(ERROR) << result << ENDL;
          return;
        }
        LOG(INFO) << "Got animation response." << ENDL;
        QueueFrame(result.value());
      });
}

void DrawString(Film* film, int* xStart, const int yStart,
                std::string str, int margin = 1) {
  for (char ch : str) {
    *xStart += margin;
    for (unsigned int x = 0; x < 8; x++) {
      for (unsigned int y = 0; y < 8; y++) {
        bool value = SampleCharacterPixel(ch, x, y);
        char rgba = value ? 255 : 0;
        unsigned int film_x = *xStart + x;
        unsigned int film_y = yStart + margin + y;
        if (film_x >= film->width ||
            film_y >= film->height || rgba == 0) {
          continue;
        }
        film->pixels[film_x][film_y] =
            Pixel(rgba, rgba, rgba, rgba);
      }
    }
    // Move the x start position
    *xStart += margin + 8;
  }
}

static Film film(APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT);

void loop() {
  // Setup camera
#if 0
  unsigned int pixels_per_unit =
      std::min(APP_SCREEN_WIDTH,
               APP_SCREEN_HEIGHT) /
      2;
#else
  unsigned int pixels_per_unit = 10;
#endif

  // AFTER:
  if (!scene_changed && queued_frame == nullptr) {
#ifdef __EMSCRIPTEN__
    // yield to browser and skip heavy work this tick
    emscripten_sleep(0);
#endif
    return;
  }
  scene_changed = false;

#ifdef __EMSCRIPTEN__
  if (SDL_MUSTLOCK(screen)) {
    SDL_LockSurface(screen);
  }
#endif  // __EMSCRIPTEN__

  LOG(DEBUG) << "Setting up ray tracer scene..." << ENDL;
  // Copy each pixel from the current animation frame
  Transform w2c = LookAt(pos, look, up);
  Camera camera(w2c, pixels_per_unit, focal_length, film);

  // Setup renderer
  SceneRenderer renderer(camera, scene);

  TIME_IT_START;
  renderer.render();
  TIME_IT_END("Rendering: ");

  // Draw message.
  int xStart = 10;
  int yStart = 10;
  DrawString(&film, &xStart, yStart, message);

  // Draw alphabet near bottom
  xStart = 10;
  yStart = APP_SCREEN_HEIGHT - 10 * 2;
  DrawString(&film, &xStart, yStart,
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

  TIME_IT_START;
  if (queued_frame != nullptr) {
    // Add queued frames to top left of film, consdering
    // tile_index.
    LOG(DEBUG) << "Drawing queued_frame..." << ENDL;

    unsigned int filmX =
        (tile_index.load(std::memory_order_relaxed) *
         queued_frame->width) %
        film.width;
    unsigned int filmY =
        APP_SCREEN_HEIGHT - queued_frame->height;
    LOG(DEBUG) << "Filling film..." << ENDL;
    TIME_IT_START;
    FillFilm(std::move(queued_frame), &film, filmX, filmY);
    TIME_IT_END("Film is filled, taking ");
  } else {
    LOG(DEBUG) << "Not drawing queued_frame." << ENDL;
  }
  TIME_IT_END("Drew queued frame: ")

// Draw pixels.
#ifdef __EMSCRIPTEN__
  Uint32* const dst = static_cast<Uint32*>(screen->pixels);
  SDL_PixelFormat* const fmt = screen->format;
  const uint32_t w = film.width;
  const uint32_t h = film.height;
  for (uint32_t j = 0; j < h; ++j) {
    Uint32* row = dst + j * w;
    for (uint32_t i = 0; i < w; ++i) {
      const Pixel& p = film.pixels[i][j];
      row[i] = SDL_MapRGBA(fmt, p.r, p.g, p.b, p.a);
    }
  }
#endif

#ifdef __EMSCRIPTEN__
  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }

  // Display the screen
  SDL_Flip(screen);
#endif  // __EMSCRIPTEN__

#ifdef __EMSCRIPTEN__
#if 1
  emscripten_sleep(1000 / APP_FRAME_RATE_FPS);
#else
  emscripten_sleep(0);
#endif
#endif  // __EMSCRIPTEN__
}
// emscripten_set_main_loop
int main(int argc, char** argv) {
#ifdef __EMSCRIPTEN__
  // Set up the keydown event listener
  emscripten_set_keydown_callback(
      EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE,
      key_callback);

  SDL_Init(SDL_INIT_VIDEO);
  screen =
      SDL_SetVideoMode(APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT,
                       32, SDL_SWSURFACE);
#endif  // __EMSCRIPTEN__

#ifdef TEST_SDL_LOCK_OPTS
  EM_ASM(
      "SDL.defaults.copyOnLock = false; "
      "SDL.defaults.discardOnLock = true; "
      "SDL.defaults.opaqueFrontBuffer = false;");
#endif

  LOG(INFO) << R"(Controls:
Arrow keys:
- up: move up
- down: move down
- left: move left
- right: move right
Open bracket (`[`): rotate left
Close bracket (`]`): rotate right
Minus (`-`/`_`): move backwards
Plus (`=`/`+`): move forwards
u: call uuid API
l: login
r: call render API
)";

#if __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#endif

#ifdef __EMSCRIPTEN__
  SDL_Quit();
#endif  // __EMSCRIPTEN__

#ifdef __EMSCRIPTEN__
  // Keep the program running
  emscripten_exit_with_live_runtime();
#endif  // __EMSCRIPTEN__

  return 0;
}
