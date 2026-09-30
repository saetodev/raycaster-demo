#include <cassert>
#include <vector>

#include <raylib.h>
#include <raymath.h>

enum TileKind : uint32_t {
    TILE_EMPTY,
    TILE_WALL,
};

struct World {
    uint32_t              width  = 0;
    uint32_t              height = 0;
    std::vector<uint32_t> tiles  = {};

    void init(uint32_t width, uint32_t height) {
        this->width  = width;
        this->height = height;
        this->tiles.resize(width * height, TILE_EMPTY);
    }

    void set_tile(uint32_t x, uint32_t y, uint32_t tile) {
        this->tiles.at(_index(x, y)) = tile;
    }

    uint32_t get_tile(uint32_t x, uint32_t y) {
        return this->tiles.at(_index(x, y));
    }

    size_t _index(uint32_t x, uint32_t y) {
        return x + y * this->width;
    }
};

int main() {
    InitWindow(950, 540, "window");

    World world = {};
    world.init(25, 25);

    for (uint32_t x = 0; x < world.width; x++) {
        world.set_tile(x, 0, TILE_WALL);
        world.set_tile(x, world.height - 1, TILE_WALL);
    }

    for (uint32_t y = 0; y < world.height; y++) {
        world.set_tile(0, y, TILE_WALL);
        world.set_tile(world.width - 1, y, TILE_WALL);
    }

    Vector2 pos      = {5, 5};
    Vector2 dir      = {-1, 0};
    float plane_dist = 0.66f;
    Vector2 plane    = {0, plane_dist};

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        float forward_input = IsKeyDown(KEY_W) - IsKeyDown(KEY_S);
        float turn_input    = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);

        float rotation = std::atan2(dir.y, dir.x);
        rotation      += turn_input * delta;
        
        dir.x = std::cos(rotation);
        dir.y = std::sin(rotation);

        plane.x = -dir.y;
        plane.y = dir.x;
        plane  *= plane_dist;

        pos += forward_input * dir * 5.0f * delta;

        BeginDrawing();
        ClearBackground(BLACK);
        
        for (int x = 0; x < GetScreenWidth(); x++) {
            float camera_x  = 2 * x / (float)(GetScreenWidth()) - 1;
            Vector2 ray_dir = dir + plane * camera_x;

            int map_x = (int)pos.x;
            int map_y = (int)pos.y;

            Vector2 side_dist      = {};
            Vector2 delta_dist     = {};
            float   perp_wall_dist = 0.0f;

            delta_dist.x = (ray_dir.x == 0.0f) ? std::numeric_limits<float>::max() : std::abs(1.0f / ray_dir.x);
            delta_dist.y = (ray_dir.y == 0.0f) ? std::numeric_limits<float>::max() : std::abs(1.0f / ray_dir.y);

            int step_x = 0;
            int step_y = 0;

            bool hit = false;
            int side = 0;

            if (ray_dir.x < 0) {
                step_x      = -1;
                side_dist.x = (pos.x - map_x) * delta_dist.x;
            }
            else {
                step_x      = 1;
                side_dist.x = (map_x + 1.0f - pos.x) * delta_dist.x;
            }
            
            if (ray_dir.y < 0) {
                step_y      = -1;
                side_dist.y = (pos.y - map_y) * delta_dist.y;
            }
            else {
                step_y      = 1;
                side_dist.y = (map_y + 1.0f - pos.y) * delta_dist.y;
            }

            while (!hit) {
                if (side_dist.x < side_dist.y) {
                    side_dist.x += delta_dist.x;
                    map_x += step_x;
                    side = 0;
                }
                else {
                    side_dist.y += delta_dist.y;
                    map_y += step_y;
                    side = 1;
                }

                if (world.get_tile(map_x, map_y) != TILE_EMPTY) {
                    hit = true;
                }
            }

            if (side == 0) {
                perp_wall_dist = (side_dist.x - delta_dist.x);
            }
            else {
                perp_wall_dist = (side_dist.y - delta_dist.y);
            }

            int line_height = (int)(GetScreenHeight() / perp_wall_dist);
            int draw_start  = std::max(-line_height / 2 + GetScreenHeight() / 2, 0);
            int draw_end    = std::min(line_height / 2 + GetScreenHeight() / 2, GetScreenHeight() - 1);

            Vector4 color = {0.55f, 0.55f, 0.55f, 1.0f};

            if (side == 1) {
                color.x *= 0.5f;
                color.y *= 0.5f;
                color.z *= 0.5f;
            }

            DrawLine(x, draw_start, x, draw_end, ColorFromNormalized(color));
        }

        DrawFPS(32, 32);
        EndDrawing();
    }

    CloseWindow();
}
