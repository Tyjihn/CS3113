/**
* Author: Lucy Zheng
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "EndScene.h"
#include "Utility.h"

EndScene::~EndScene()
{
    Mix_FreeChunk(m_game_state.game_over_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void EndScene::initialise()
{
    m_scene_type = END;
    m_game_state.next_scene_id = -1;

    // ----- Text ----- //
    m_font_texture_id = Utility::load_texture("assets/font2.png");

    // ----- BGM and SFX ----- //
    Mix_HaltMusic(); // Stop BGM
}

void EndScene::update(float delta_time) {}

void EndScene::render(ShaderProgram* g_shader_program)
{
    // ----- Sound Effect ----- //
    if (!m_has_played_sound)
    {
        if (m_player_lives <= 0) {
            m_game_state.game_over_sfx = Mix_LoadWAV("assets/music/game_over.wav");
            Mix_PlayChannel(0, m_game_state.game_over_sfx, 0);
        }
        else {
            m_game_state.game_over_sfx = Mix_LoadWAV("assets/music/game_win.wav");
            Mix_PlayChannel(0, m_game_state.game_over_sfx, 0);
        }
        m_has_played_sound = true;
    }

    // ----- Background ----- //
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // ----- Text ----- //
    Utility::draw_text(g_shader_program, m_font_texture_id, "GAME OVER", 
        0.5f, 0.0f, glm::vec3(-2.15f, 0.75f, 0.0f));
    if (m_player_lives <= 0)
    {
        Utility::draw_text(g_shader_program, m_font_texture_id, "YOU LOSE",
            0.35f, 0.0f, glm::vec3(-1.42f, -0.6f, 0.0f));
    }
    else
    {
        Utility::draw_text(g_shader_program, m_font_texture_id, "YOU WIN",
            0.35f, 0.0f, glm::vec3(-1.25f, -0.6f, 0.0f));
    }
}
