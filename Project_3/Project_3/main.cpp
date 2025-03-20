/**
* Author: Lucy Zheng
* Assignment: Lunar Lander
* Date due: 2025-3-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define PLATFORM_COUNT 2

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include "Entity.h"

// ––––– CONSTANTS ––––– //
const float WINDOW_MULT = 1.5f;
const float WINDOW_WIDTH  = 640 * WINDOW_MULT,
            WINDOW_HEIGHT = 480 * WINDOW_MULT;

const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

const char BACKGROUND_FILEPATH[] = "assets/background/background.png",
           CLOUD1_FILEPATH[]     = "assets/background/cloud1.png",
           CLOUD2_FILEPATH[]     = "assets/background/cloud2.png",
           MOON_FILEPATH[]       = "assets/background/moon.png";
const char PLATFORM_FILEPATH[]   = "assets/platform.png";
const char FONT_FILEPATH[]       = "assets/font1.png";

GLuint g_font_texture_id;
GLuint g_background_texture_id,
       g_cloud1_texture_id,
       g_cloud2_texture_id,
       g_moon_texture_id;

const GLint NUMBER_OF_TEXTURES = 1,
            LEVEL_OF_DETAIL    = 0,
            TEXTURE_BORDER     = 0;

// ––––– STRUCTS AND ENUMS ––––– //
enum FilterType { NEAREST, LINEAR };
struct GameState
{
    Entity* player;
    Entity* platforms;
    Entity* background;
    Entity* cloud1;
    Entity* cloud2;
    Entity* moon;
};

// ––––– GLOBAL VARIABLES ––––– //
GameState g_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_win =  false,
     g_game_lose = false,
     g_game_end =  false;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

constexpr int FONTBANK_SIZE = 16;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

const float GRAVITY_CONST = 0.16f;

// ----- FUNCTION PROTOTYPES ----- //
GLuint load_texture(const char* filepath);
GLuint load_texture(const char* filepath, FilterType filterType);
void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
               float font_size, float spacing, glm::vec3 position);

// ––––– GENERAL FUNCTIONS ––––– //
void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Lunar Lander",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_program.get_program_id());
    
    // ----- BACKGROUND ----- //
    g_state.background = new Entity();
    g_state.background->set_m_type(BACKGROUND);
    g_background_texture_id = load_texture(BACKGROUND_FILEPATH);
    g_state.background->set_texture_id(g_background_texture_id);
    g_state.background->set_position(glm::vec3(0.0f));
    g_state.background->set_scale(glm::vec3(10.0f, 7.5f, 1.0f));
    g_state.background->update(0.0f, NULL, 0.0f);

    g_state.cloud1 = new Entity();
    g_state.background->set_m_type(BACKGROUND);
    g_cloud1_texture_id = load_texture(CLOUD1_FILEPATH);
    g_state.cloud1->set_texture_id(g_cloud1_texture_id);
    g_state.cloud1->set_position(glm::vec3(-2.2f, 2.6f, 1.0f));
    g_state.cloud1->set_scale(glm::vec3(4.0f, 2.0f, 1.0f));
    g_state.cloud1->update(0.0f, NULL, 0.0f);

    g_state.cloud2 = new Entity();
    g_state.background->set_m_type(BACKGROUND);
    g_cloud2_texture_id = load_texture(CLOUD2_FILEPATH);
    g_state.cloud2->set_texture_id(g_cloud2_texture_id);
    g_state.cloud2->set_position(glm::vec3(1.4f, 1.5f, 1.0f));
    g_state.cloud2->set_scale(glm::vec3(3.0f, 1.5f, 1.0f));
    g_state.cloud2->update(0.0f, NULL, 0.0f);

    g_state.moon = new Entity();
    g_state.background->set_m_type(BACKGROUND);
    g_moon_texture_id = load_texture(MOON_FILEPATH);
    g_state.moon->set_texture_id(g_moon_texture_id);
    g_state.moon->set_position(glm::vec3(3.8f, 2.3f, 1.0f));
    g_state.moon->set_scale(glm::vec3(-1.8f, 1.8f, 1.0f));
    g_state.moon->update(0.0f, NULL, 0.0f);

    // ----- TEXT ----- //
    g_font_texture_id = load_texture(FONT_FILEPATH);
    
    // ––––– PLATFORMS ––––– //
    GLuint platform_texture_id = load_texture(PLATFORM_FILEPATH);
    
    g_state.platforms = new Entity[PLATFORM_COUNT];

    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        g_state.platforms[i].set_m_type(PLATFORM);
    }

    g_state.platforms[PLATFORM_COUNT - 1].set_texture_id(platform_texture_id);
    g_state.platforms[PLATFORM_COUNT - 1].set_position(glm::vec3(-1.5f, -2.85f, 0.0f));
    g_state.platforms[PLATFORM_COUNT - 1].set_width(3.159f * 0.25f);
    g_state.platforms[PLATFORM_COUNT - 1].set_height(1.0f * 0.25f);
    g_state.platforms[PLATFORM_COUNT - 1].set_scale(glm::vec3(3.159f * 0.25f, 1.0f * 0.25f, 1.0f));
    g_state.platforms[PLATFORM_COUNT - 1].update(0.0f, NULL, 0);

    g_state.platforms[PLATFORM_COUNT - 2].set_texture_id(platform_texture_id);
    g_state.platforms[PLATFORM_COUNT - 2].set_position(glm::vec3(2.5f, -2.5f, 0.0f));
    g_state.platforms[PLATFORM_COUNT - 2].set_width(3.159f * 0.25f);
    g_state.platforms[PLATFORM_COUNT - 2].set_height(1.0f * 0.25f);
    g_state.platforms[PLATFORM_COUNT - 2].set_scale(glm::vec3(3.159f * 0.25f, 1.0f * 0.25f, 1.0f));
    g_state.platforms[PLATFORM_COUNT - 2].update(0.0f, NULL, 0);
    
    // ––––– PLAYER ––––– //
    std::vector<GLuint> player_textures_ids = {
        load_texture("assets/off.png", NEAREST),   // OFF spritesheet
        load_texture("assets/on.png", NEAREST)     // ON spritesheet
    };

    std::vector<std::vector<int>> player_animations = {
        {0},    // OFF animation frames
        {0, 1}  // ON animation frames
    };

    // Existing
    //g_state.player = new Entity(
    //    player_textures_ids, // a list of texture IDs
    //    player_animations,   // list of animation frames for each type of animation
    //    1.0f,                // translation speed
    //    0.0f,                // animation time
    //    1,                   // number of frames for animation
    //    0,                   // current frame index
    //    1,                   // current animation col amount
    //    1,                   // current animation row amount
    //    OFF                  // current animation
    //);

    g_state.player = new Entity();
    g_state.player->set_m_type(PLAYER);
    g_state.player->set_texture_id(load_texture("assets/off.png"));
    g_state.player->set_position(glm::vec3(-4.0f, 2.5f, 0.0f));
    g_state.player->set_movement(glm::vec3(10.0f, 0.0f, 0.0f));
    g_state.player->set_acceleration(glm::vec3(0.0f, GRAVITY_CONST, 0.0f));
    g_state.player->set_rotated_angle(90.0f);
    g_state.player->set_scale(glm::vec3(1.0f * 0.5f, 1.1f * 0.5f, 0.0f));
    g_state.player->set_fuel(150);

    
    // Moving
    //g_state.player->set_animation_state(OFF);
    //g_state.player->m_animation_frames = 4;
    //g_state.player->m_animation_index  = 0;
    //g_state.player->m_animation_time   = 0.0f;
    //g_state.player->m_animation_cols   = 4;
    //g_state.player->m_animation_rows   = 4;
    g_state.player->set_height(0.9f);
    g_state.player->set_width(0.9f);

    // Engine
    g_state.player->set_engine_power(1.0f);
    
    // ––––– GENERAL ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_state.player->set_rotation_angle(0.0f);
    g_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_game_is_running = false;
                        break;
                        
                    default:
                        break;
                }

            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        // Rotate CCW, minimum angle of -90
        if (g_state.player->get_rotated_angle() > -90)
            g_state.player->set_rotation_angle(-4.0f);
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        // Rotate CW, maximum angle of 90
        if (g_state.player->get_rotated_angle() < 90)
            g_state.player->set_rotation_angle(4.0f);
    }

    if (key_state[SDL_SCANCODE_UP])
    {
        // Set Animation State
        //g_state.player->set_animation_state(Animation::ON);

        // Engine off if player has no fuel
        if (g_state.player->get_fuel() > 0)
            g_state.player->set_engine_state(true);
    }
    else 
    {
        //g_state.player->set_animation_state(OFF);
        g_state.player->set_engine_state(false);
    }
    
    if (glm::length(g_state.player->get_movement()) > 1.0f)
    {
        g_state.player->set_movement(glm::normalize(g_state.player->get_movement()));
    }
}

void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    // Add gravity in y-axis
    g_state.player->set_acceleration(g_state.player->get_acceleration() - glm::vec3(0.0f, GRAVITY_CONST, 0.0f));
    
    while (delta_time >= FIXED_TIMESTEP)
    {
        g_state.player->update(FIXED_TIMESTEP, g_state.platforms, PLATFORM_COUNT);
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;

    // ----- MISSION FAIL/SUCCESS ----- //
    if (g_state.player->get_collided_bottom() || g_state.player->get_collided_left() ||
        g_state.player->get_collided_right() || g_state.player->get_collided_top())
    {
        g_game_win = true;
    }
    else if (g_state.player->check_screen_collision(g_projection_matrix))
    {
        g_game_lose = true;
    }

    if (g_game_win || g_game_lose)
        g_game_end = true;
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // ----- BACKGROUND ----- //
    g_state.background->render(&g_program);
    g_state.cloud1->render(&g_program);
    g_state.cloud2->render(&g_program);
    g_state.moon->render(&g_program);

    // ----- PLAYER ----- //
    g_state.player->render(&g_program);
    
    // ----- PLATFORMS ----- //
    for (int i = 0; i < PLATFORM_COUNT; i++) g_state.platforms[i].render(&g_program);

    // ----- GAME END MESSAGE ----- //
    if (g_game_win)
        draw_text(&g_program, g_font_texture_id, "Mission Accomplished", 0.4f, 0.0f,
            glm::vec3(-3.5f, 2.0f, 0.0f));
    else if (g_game_lose)
        draw_text(&g_program, g_font_texture_id, "Mission Failed", 0.4f, 0.0f,
            glm::vec3(-2.8f, 2.0f, 0.0f));
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete [] g_state.platforms;
    delete g_state.player;
    delete g_state.background;
    delete g_state.cloud1;
    delete g_state.cloud2;
    delete g_state.moon;
}

int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        if (!g_game_end)
        {
            update();
            render();
        }
    }
    
    shutdown();
    return 0;
}


GLuint load_texture(const char* filepath, FilterType filterType)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components,
        STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
        GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        filterType == NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        filterType == NEAREST ? GL_NEAREST : GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    for (int i = 0; i < text.size(); i++) {
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}