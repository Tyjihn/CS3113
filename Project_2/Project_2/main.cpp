/**
* Author: Lucy Zheng
* Assignment: Pong Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"               // We'll talk about these later in the course
#include "stb_image.h"
#include <vector>
#include <format>

enum AppStatus { RUNNING, TERMINATED };

// Our window dimensions
constexpr float WINDOW_SIZE_MULT = 1.75f;
constexpr int WINDOW_WIDTH =  640 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

// Background color components
constexpr float BG_RED = 1.0f,
                BG_BLUE = 1.0f,
                BG_GREEN = 1.0f,
                BG_OPACITY = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1,
                LEVEL_OF_DETAIL = 0,
                TEXTURE_BORDER = 0;

// source: https://pngmart.com/ , https://among-us.fandom.com/
constexpr char PADDLE1_SPRITE_FILEPATH[] = "assets/Blue_Pico.png",
               PADDLE2_SPRITE_FILEPATH[] = "assets/Red_Pico.png",
               BALL1_SPRITE_FILEPATH[] =   "assets/Ghost.png",
               BALL2_SPRITE_FILEPATH[] =   "assets/Slime.png",
               BALL3_SPRITE_FILEPATH[] =   "assets/Button.png",
               FONTSHEET_FILEPATH[] =      "assets/font1.png";

// Text Function
void draw_text(ShaderProgram* shader_program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position);

// Initial position and scaling
constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_POS_PADDLE1 =      glm::vec3(-4.3f, 0.0f, 0.0f),
                    INIT_SCALE_PADDLE1 =    glm::vec3(0.8f, 0.8f * 1.195f, 0.0f),
                    INIT_POS_PADDLE2 =      glm::vec3(4.3f, 0.0f, 0.0f),
                    INIT_SCALE_PADDLE2 =    glm::vec3(0.8f, 0.8f * 1.195f, 0.0f),
                    INIT_POS_BALL1 =        glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_BALL1 =      glm::vec3(0.3f, 0.3f * 1.1494f, 1.0f),
                    INIT_POS_BALL2 =        glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_BALL2 =      glm::vec3(0.3f, 0.3f * 0.53125f, 1.0f),
                    INIT_POS_BALL3 =        glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_BALL3 =      glm::vec3(0.3f, 0.3f * 0.49123f, 1.0f);

// Our object's fill colour
constexpr float TRIANGLE_RED = 1.0,
                TRIANGLE_BLUE = 0.4,
                TRIANGLE_GREEN = 0.4,
                TRIANGLE_OPACITY = 1.0;

float g_previous_ticks = 0;

AppStatus g_app_status = RUNNING;
SDL_Window* g_display_window;

ShaderProgram g_shader_program;

glm::mat4 g_view_matrix,        // Defines the position (location and orientation) of the camera
          g_projection_matrix,  // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.
          g_paddle1_matrix,               
          g_paddle2_matrix,
          g_ball1_matrix,
          g_ball2_matrix,
          g_ball3_matrix;

glm::vec3 g_paddle1_position = glm::vec3(0.0f),
          g_paddle1_movement = glm::vec3(0.0f);

glm::vec3 g_paddle2_position = glm::vec3(0.0f),
          g_paddle2_movement = glm::vec3(0.0f);

glm::vec3 g_ball1_position = glm::vec3(0.0f),
          g_ball1_movement = glm::vec3(-1.0f, -0.5f, 0.0f);

glm::vec3 g_ball2_position = glm::vec3(0.0f),
          g_ball2_movement = glm::vec3(1.0f, 0.25f, 0.0f);

glm::vec3 g_ball3_position = glm::vec3(0.0f),
          g_ball3_movement = glm::vec3(-1.0f, 0.75f, 0.0f);

constexpr float PADDLE_SPEED = 0.55f,
                BALL_SPEED = 0.4f,
                AMPLITUDE_INCREMENT = 2.5f;

bool single_player = false,
     ball2 = false,
     ball3 = false,
     game_over = false;

constexpr int FONTBANK_SIZE = 16;

GLuint g_paddle1_texture_id,
       g_paddle2_texture_id,
       g_ball1_texture_id,
       g_ball2_texture_id,
       g_ball3_texture_id,
       g_font_texture_id;


void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for
    // each character. Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their
        //    position relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
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

    // 4. And render all of them using the pairs
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

GLuint load_texture(const char* filepath)
{
    // load image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // Generate and bind texture ID to image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Set texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Release file from memory and return texture ID
    stbi_image_free(image);
    return textureID;
}

void initialise()
{
    // HARD INITIALISE ———————————————————————————————————————————————————————————————————
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hello, Pong!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    if (g_display_window == nullptr)
    {
        std::cerr << "ERROR: SDL Window could not be created.\n";
        g_app_status = TERMINATED;

        SDL_Quit();
        exit(1);
    }

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ———————————————————————————————————————————————————————————————————————————————————

    // SOFT INITIALISE ———————————————————————————————————————————————————————————————————
    // Initialise our camera
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    // Load up our shaders
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    // Initialise our view, model, and projection matrices
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    // Object Matrices
    g_paddle1_matrix = glm::mat4(1.0f);
    g_paddle2_matrix = glm::mat4(1.0f);
    g_ball1_matrix = glm::mat4(1.0f);
    g_ball2_matrix = glm::mat4(1.0f);
    g_ball3_matrix = glm::mat4(1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_paddle1_texture_id = load_texture(PADDLE1_SPRITE_FILEPATH);
    g_paddle2_texture_id = load_texture(PADDLE2_SPRITE_FILEPATH);
    g_ball1_texture_id =   load_texture(BALL1_SPRITE_FILEPATH);
    g_ball2_texture_id =   load_texture(BALL2_SPRITE_FILEPATH);
    g_ball3_texture_id =   load_texture(BALL3_SPRITE_FILEPATH);
    g_font_texture_id =    load_texture(FONTSHEET_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_paddle1_movement.y = 0.0f;
    g_paddle2_movement.y = 0.0f;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
            break;
        }
        switch (event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_t:
                        single_player = true;
                        g_paddle2_position.y = 0.0f;
                        break;
                    case SDLK_1:
                        ball2 = false;
                        ball3 = false;
                        break;
                    case SDLK_2:
                        ball2 = true;
                        ball3 = false;
                        g_ball2_position = glm::vec3(0.0f);
                        break;
                    case SDLK_3:
                        ball2 = true;
                        ball3 = true;
                        g_ball3_position = glm::vec3(0.0f);
                        break;
                }
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    // Paddle Constraints
    if (key_state[SDL_SCANCODE_W]) 
    {
        if (g_paddle1_position.y < (3.75f - (INIT_SCALE_PADDLE1.y / 2.0f)))
            g_paddle1_movement.y += 1.0f;
    }
    if (key_state[SDL_SCANCODE_S])
    {
        if (g_paddle1_position.y > (-3.75f + (INIT_SCALE_PADDLE1.y / 2.0f)))
            g_paddle1_movement.y -= 1.0f;
    }
    if (!single_player)
    {
        if (key_state[SDL_SCANCODE_UP])
        {
            if (g_paddle2_position.y < (3.75f - (INIT_SCALE_PADDLE2.y / 2.0f)))
                g_paddle2_movement.y += 1.0f;
        }
        else if (key_state[SDL_SCANCODE_DOWN])
        {
            if (g_paddle2_position.y > (-3.75f + (INIT_SCALE_PADDLE2.y / 2.0f)))
                g_paddle2_movement.y -= 1.0f;
        }
    }
}

// Update Ball Trnsformations
void update_ball(glm::vec3& ball_pos, glm::vec3 ball_init_pos, glm::vec3 ball_scale,
    glm::vec3& ball_movement, glm::mat4& ball_matrix, float delta_time)
{
    ball_pos += ball_movement * BALL_SPEED * delta_time;
    ball_matrix = glm::mat4(1.0f);
    ball_matrix = glm::translate(ball_matrix, ball_init_pos);
    ball_matrix = glm::translate(ball_matrix, ball_pos);

    ball_matrix = glm::scale(ball_matrix, ball_scale);
}

// Check Collision between Ball and Paddle
void paddle_collision_check(glm::vec3& paddle_pos, glm::vec3 paddle_init_pos, glm::vec3 paddle_scale,
    glm::vec3& ball_pos, glm::vec3 ball_init_pos, glm::vec3 ball_scale, glm::vec3& ball_movement)
{
    float x_distance = fabs(paddle_pos.x + paddle_init_pos.x - ball_init_pos.x - ball_pos.x) -
        ((paddle_scale.x + ball_scale.x) / 2.0f);

    float y_distance = fabs(paddle_pos.y + paddle_init_pos.y - ball_init_pos.y - ball_pos.y) -
        ((paddle_scale.y + ball_scale.y) / 2.0f);

    if (x_distance < 0.0f && y_distance < 0.0f)
    {
        if (fabs(x_distance) < fabs(y_distance))
        {
            if ((ball_init_pos.x + ball_pos.x) > (paddle_pos.x + paddle_init_pos.x))
                ball_pos.x -= x_distance;
            else
                ball_pos.x += x_distance;

            ball_movement.x *= -1.0f;
        }
        else
        {
            if ((ball_init_pos.y + ball_pos.y) > (paddle_pos.y + paddle_init_pos.y))
                ball_pos.y -= y_distance;
            else
                ball_pos.y += y_distance;

            ball_movement.y *= -1.0f;
        }
    }
}

// Check Collision between Ball and Wall
void wall_collision_check(glm::vec3& ball_pos, glm::vec3 ball_scale, glm::vec3& ball_movement)
{
    // Wall Collision
    if (fabs(ball_pos.y) > (3.75f - (ball_scale.y / 2.0f)))
    {
        float y_distance = fabs(ball_pos.y) - (3.75f - (ball_scale.y / 2.0f));
        if (ball_pos.y < 0)
            ball_pos.y += y_distance;
        else if (ball_pos.y > 0)
            ball_pos.y -= y_distance;

        ball_movement.y *= -1.0f;
    }

    // Win Condition
    if (fabs(ball_pos.x) > (5.0f - (ball_scale.y / 2.0f)))
        game_over = true;
}

void update()
{
    /* Delta time calculations */
    float ticks = (float)SDL_GetTicks() / 100.0f;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    /* Accumulators */
    g_paddle1_position += g_paddle1_movement * PADDLE_SPEED * delta_time;
    if (!single_player)   g_paddle2_position.y += g_paddle2_movement.y * PADDLE_SPEED * delta_time;
    else                  g_paddle2_position.y += PADDLE_SPEED * delta_time;

    /* Transformations */
    g_paddle1_matrix = glm::mat4(1.0f);
    g_paddle1_matrix = glm::translate(g_paddle1_matrix, INIT_POS_PADDLE1);
    g_paddle1_matrix = glm::translate(g_paddle1_matrix, g_paddle1_position);
    g_paddle1_matrix = glm::scale(g_paddle1_matrix, INIT_SCALE_PADDLE1);

    g_paddle2_matrix = glm::mat4(1.0f);
    g_paddle2_matrix = glm::translate(g_paddle2_matrix, INIT_POS_PADDLE2);
    if (!single_player)
    {
        g_paddle2_matrix = glm::translate(g_paddle2_matrix, g_paddle2_position);
    }
    else
    {
        g_paddle2_matrix = glm::translate(g_paddle2_matrix, 
            glm::vec3(0.0f, sin(g_paddle2_position.y) * AMPLITUDE_INCREMENT, 0.0f));
    }
    g_paddle2_matrix = glm::scale(g_paddle2_matrix, INIT_SCALE_PADDLE2);

    /* Ball 1 */
    update_ball(g_ball1_position, INIT_POS_BALL1, INIT_SCALE_BALL1, g_ball1_movement, g_ball1_matrix, delta_time);
    if (g_ball1_position.x < WINDOW_WIDTH * (3 / 4))
    {
        paddle_collision_check(g_paddle1_position, INIT_POS_PADDLE1, INIT_SCALE_PADDLE1,
            g_ball1_position, INIT_POS_BALL1, INIT_SCALE_BALL1, g_ball1_movement);
    }
    else if (g_ball1_position.x > WINDOW_WIDTH * (3 / 4))
    {
        paddle_collision_check(g_paddle2_position, INIT_POS_PADDLE2, INIT_SCALE_PADDLE2,
            g_ball1_position, INIT_POS_BALL1, INIT_SCALE_BALL1, g_ball1_movement);
    }
    wall_collision_check(g_ball1_position, INIT_SCALE_BALL1, g_ball1_movement);

    /* Ball 2 */
    if (ball2)
    {
        update_ball(g_ball2_position, INIT_POS_BALL2, INIT_SCALE_BALL2, g_ball2_movement, g_ball2_matrix, delta_time);
        if (g_ball2_position.x < WINDOW_WIDTH * (3 / 4))
        {
            paddle_collision_check(g_paddle1_position, INIT_POS_PADDLE1, INIT_SCALE_PADDLE1,
                g_ball2_position, INIT_POS_BALL2, INIT_SCALE_BALL2, g_ball2_movement);
        }
        else if (g_ball2_position.x > WINDOW_WIDTH * (3 / 4))
        {
            paddle_collision_check(g_paddle2_position, INIT_POS_PADDLE2, INIT_SCALE_PADDLE2,
                g_ball2_position, INIT_POS_BALL2, INIT_SCALE_BALL2, g_ball2_movement);
        }
        wall_collision_check(g_ball2_position, INIT_SCALE_BALL2, g_ball2_movement);
    }

    /* Ball 3*/
    if (ball3)
    {
        update_ball(g_ball3_position, INIT_POS_BALL3, INIT_SCALE_BALL3, g_ball3_movement, g_ball3_matrix, delta_time);
        if (g_ball3_position.x < WINDOW_WIDTH * (3 / 4))
        {
            paddle_collision_check(g_paddle1_position, INIT_POS_PADDLE1, INIT_SCALE_PADDLE1,
                g_ball3_position, INIT_POS_BALL3, INIT_SCALE_BALL3, g_ball3_movement);
        }
        else if (g_ball3_position.x > WINDOW_WIDTH * (3 / 4))
        {
            paddle_collision_check(g_paddle2_position, INIT_POS_PADDLE2, INIT_SCALE_PADDLE2,
                g_ball3_position, INIT_POS_BALL3, INIT_SCALE_BALL3, g_ball3_movement);
        }
        wall_collision_check(g_ball3_position, INIT_SCALE_BALL3, g_ball3_movement);
    }
}

void draw_object(glm::mat4& object_g_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT,
        false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
        false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_paddle1_matrix, g_paddle1_texture_id);
    draw_object(g_paddle2_matrix, g_paddle2_texture_id);
    draw_object(g_ball1_matrix, g_ball1_texture_id);
    if (ball2) draw_object(g_ball2_matrix, g_ball2_texture_id);
    if (ball3) draw_object(g_ball3_matrix, g_ball3_texture_id);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    if (game_over)
    {
        std::string winner = (g_ball1_position.x < 0) ? "Player 2" : "Player 1";
        draw_text(&g_shader_program, g_font_texture_id, winner + " Wins", 0.5f, 0.05f,
            glm::vec3(-3.5f, 0.0f, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    // Initialise our program—whatever that means
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();  // If the player did anything—press a button, move the joystick—process it
        if (!game_over)
        {
            update();     // Using the game's previous state, and whatever new input we have, update the game's state
            render();         // Once updated, render those changes onto the screen
        }
    }

    shutdown();  // The game is over, so let's perform any shutdown protocols
    return 0;
}