/**
* Author: Lucy Zheng
* Assignment: Simple 2D Scene
* Date due: 2025-02-15, 11:59pm
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


enum AppStatus { RUNNING, TERMINATED };

// Our window dimensions
constexpr int WINDOW_WIDTH = 640 * 1.5,
              WINDOW_HEIGHT = 480 * 1.5;

// Background color components
constexpr float BG_RED = 0.0f,
                BG_BLUE = 0.0f,
                BG_GREEN = 0.0f,
                BG_OPACITY = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";


/* -------------- Edited Code START-------------- */
constexpr GLint NUMBER_OF_TEXTURES = 1,
                LEVEL_OF_DETAIL = 0,
                TEXTURE_BORDER = 0;

// source: https://pngmart.com/ , https://among-us.fandom.com/
constexpr char CREWMATE_SPRITE_FILEPATH[] = "Among_US_Character.png",
               PET_SPRITE_FILEPATH[] = "UFO.png";

constexpr glm::vec3 INIT_POS_CREWMATE =     glm::vec3(-4.5f, 0.0f, 0.0f),
                    INIT_SCALE_CREWMATE =   glm::vec3(1.1f * 0.2f, 1.14f * 0.2f, 0.0f),
                    INIT_POS_PET =          glm::vec3(4.35f, -3.15f, 0.0f),
                    INIT_SCALE_PET =        glm::vec3(0.6f, 0.4f, 0.0f);

constexpr float ROTATE_INCREMENT = 1.0f;
constexpr float SCALE_INCREMENT = 1.0f;

AppStatus g_app_status = RUNNING;
SDL_Window* g_display_window;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,        // Defines the position (location and orientation) of the camera
          g_crewmate_matrix,
          g_pet_matrix,
          g_projection_matrix;  // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.

float g_previous_ticks = 0.0f;

glm::vec3 g_translation_crewmate = glm::vec3(0.0f, 0.0f, 0.0f),
          g_rotation_crewmate =    glm::vec3(0.0f, 0.0f, 0.0f),
          g_scale_crewmate =       glm::vec3(0.0f, 0.0f, 0.0f),
          g_translation_pet =      glm::vec3(0.0f, 0.0f, 0.0f),
          g_rotation_pet =         glm::vec3(0.0f, 0.0f, 0.0f);

GLuint g_crewmate_texture_id,
       g_pet_texture_id;


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
/* --------------- Edited Code END --------------- */


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Hello, Textures!",
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

    // Initialise our camera
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    // Load up our shaders
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);


    /* -------------- Edited Code START -------------- */
    g_view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    g_crewmate_matrix = glm::mat4(1.0f);
    g_pet_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    // Each object has its own unique ID
    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_crewmate_texture_id = load_texture(CREWMATE_SPRITE_FILEPATH);
    g_pet_texture_id =      load_texture(PET_SPRITE_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* --------------- Edited Code END --------------- */
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}

void update()
{
    /* Delta time calculations */ 
    float ticks = (float) SDL_GetTicks() / 1000.0f;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    /* Game Logic */ 
    // translate, rotate, scale
    g_translation_crewmate.x += 0.5f * delta_time;
    g_rotation_crewmate.z += -1.0f * ROTATE_INCREMENT * delta_time;
    g_scale_crewmate.x += 1.0f * SCALE_INCREMENT * delta_time;
    g_scale_crewmate.y += 1.0f * SCALE_INCREMENT * delta_time;

    g_translation_pet.x += 1.0f * delta_time;
    g_translation_pet.y += 1.0f * delta_time;

    /* Model matrix reset */
    g_crewmate_matrix = glm::mat4(1.0f);
    g_pet_matrix = glm::mat4(1.0f);

    /* Transformations */

    // transform in relation to another - pet rotate in relation to crewmate rotation (DONE)
    // moving in relation to another - crewmate translate in relation to scaling of pet (y-value: should move in sine, x-value: constant) (DONE)
    // 1 object rotating (DONE)
    
    // EXTRA CREDIT
    //      1 object change scale - crewmate scaling in sinusoidal (DONE)
    //      both object translate non-horizontal/vertical - crew: sinusoidal (DONE) / pet: circular (DONE)

    g_crewmate_matrix = glm::translate(g_crewmate_matrix, 
                            glm::vec3(INIT_POS_CREWMATE.x + g_translation_crewmate.x, INIT_POS_CREWMATE.y + sin(g_translation_pet.y), 0.0f));
    g_crewmate_matrix = glm::rotate(g_crewmate_matrix, g_rotation_crewmate.z, glm::vec3(0.0f, 0.0f, 1.0f));
    g_crewmate_matrix = glm::scale(g_crewmate_matrix, 
                            glm::vec3(INIT_SCALE_CREWMATE.x * (cos(g_scale_crewmate.x) + 3.0f), 
                                      INIT_SCALE_CREWMATE.y * (cos(g_scale_crewmate.y) + 3.0f), 0.0f));
    g_pet_matrix =      glm::translate(g_pet_matrix, 
                            glm::vec3(cos(g_translation_pet.x)*0.2f + INIT_POS_PET.x, sin(g_translation_pet.y)*0.2f + INIT_POS_PET.y, 0.0f));
    g_pet_matrix =      glm::rotate(g_pet_matrix, g_rotation_crewmate.z, glm::vec3(0.0f, -1.0f, 0.0f));
    g_pet_matrix =      glm::scale(g_pet_matrix, INIT_SCALE_PET);
}

void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);

    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render()
{
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
    draw_object(g_crewmate_matrix, g_crewmate_texture_id);
    draw_object(g_pet_matrix, g_pet_texture_id);
    
    // Disable two attribute arrays
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
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
        update();         // Using the game's previous state, and whatever new input we have, update the game's state
        render();         // Once updated, render those changes onto the screen
    }

    shutdown();  // The game is over, so let's perform any shutdown protocols
    return 0;
}