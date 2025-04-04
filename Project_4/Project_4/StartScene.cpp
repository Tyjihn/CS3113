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
    //delete[] m_game_state.enemies;
    //delete   m_game_state.player;
    //delete   m_game_state.map;
    
    Mix_FreeChunk(m_game_state.jump_sfx);
    //Mix_FreeMusic(m_game_state.bgm);
}

void StartScene::initialise()
{
    // ----- Background ----- //
    m_scene_type = START;

    constexpr float scale_mult = 11.0f;
    m_background_texture_id = Utility::load_texture("assets/background.png");

    m_background_position = glm::vec3(0.0f, 1.25f, 0.0f);
    m_background_scale = glm::vec3(1.1702396f * scale_mult, 1.0f * scale_mult, 1.0f);

    // ----- Text ----- //
    m_font_texture_id = Utility::load_texture("assets/font2.png");

    // ----- BGM and SFX ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/music/jump.wav");
}

void StartScene::update(float delta_time) { }

void StartScene::render(ShaderProgram* g_shader_program)
{
    // ----- Background ----- //
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, m_background_position);
    model_matrix = glm::scale(model_matrix, m_background_scale);

    g_shader_program->set_model_matrix(model_matrix);
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_background_texture_id);

    glVertexAttribPointer(g_shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program->get_position_attribute());
    glVertexAttribPointer(g_shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(g_shader_program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_shader_program->get_position_attribute());
    glDisableVertexAttribArray(g_shader_program->get_tex_coordinate_attribute());


    // ----- Text ----- //
    Utility::draw_text(g_shader_program, m_font_texture_id, "Platformer", 0.6f, 0.0f,
        glm::vec3(-2.7f, 1.75f, 0.0f));
    Utility::draw_text(g_shader_program, m_font_texture_id, "PRESS ENTER TO START GAME", 0.2f, 0.0f,
        glm::vec3(-2.5f, -2.25f, 0.0f));
}
