/**
* Author: Lucy Zheng
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "StartScene.h"
#include "Utility.h"

StartScene::~StartScene()
{   
    Mix_FreeChunk(m_game_state.jump_sfx);
}

void StartScene::initialise()
{
    // ----- Background ----- //
    m_scene_type = START;

    constexpr float bg_mult = 11.0f;
    GLuint background_texture_id = Utility::load_texture("assets/background.png");

    m_game_state.background = new Entity(background_texture_id, BACKGROUND);

    m_game_state.background->set_position(glm::vec3(0.0f, 1.25f, 0.0f));
    m_game_state.background->set_scale(glm::vec3(1.1702396f * bg_mult, 1.0f * bg_mult, 1.0f));

    // ----- Text ----- //
    m_font_texture_id = Utility::load_texture("assets/font2.png");
}

void StartScene::update(float delta_time) { 
    m_game_state.background->update(delta_time, NULL, NULL, NULL, NULL);
}

void StartScene::render(ShaderProgram* g_shader_program)
{
    // ----- Background ----- //
    m_game_state.background->render(g_shader_program);

    // ----- Text ----- //
    Utility::draw_text(g_shader_program, m_font_texture_id, "Platformer", 0.6f, 0.0f,
        glm::vec3(-2.7f, 1.75f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "PRESS ENTER TO START GAME", 0.2f, 0.0f,
        glm::vec3(-2.5f, -2.25f, 0.0f));
}
