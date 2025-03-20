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

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include <vector>

Entity::Entity()
{
    m_animation_indices = nullptr;

    // ––––– PHYSICS ––––– //
    m_position     = glm::vec3(0.0f);
    m_velocity     = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);
    
    // ––––– TRANSLATION ––––– //
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);

    // ----- ROTATION ----- //
    m_rotation_angle = 0.0f;
    m_rotation_angle = 0.0f;
    m_rotation_plane = glm::vec3(0.0f, 0.0f, 1.0f);

    // ----- SCALING ----- //
    m_scale = glm::vec3(1.0f);
}

Entity::Entity(std::vector<GLuint> texture_ids, std::vector<std::vector<int>> animations,
    float speed, float animation_time, int animation_frames, int animation_index,
    int animation_cols, int animation_rows, Animation animation)
    : m_position(glm::vec3(0.0f)), m_movement(glm::vec3(0.0f)), m_scale(glm::vec3(1.0f)),
    m_speed(speed), m_texture_ids(texture_ids), m_animations(animations),
    m_animation_cols(animation_cols), m_animation_frames(animation_frames),
    m_animation_index(animation_index), m_animation_rows(animation_rows),
    m_animation_time(animation_time), m_current_animation(animation)
{
    set_animation_state(m_current_animation);  // Initialize animation state
}

Entity::~Entity() { }

void Entity::set_animation_state(Animation new_animation)
{
    m_current_animation = new_animation;
    m_animation_cols = m_animations[m_current_animation].size();
    m_animation_frames = m_animations[m_current_animation].size();
    m_animation_indices = m_animations[m_current_animation].data();
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program)
{
    if (m_animation_indices != nullptr)
    {
        GLuint current_texture = m_texture_ids[m_current_animation];  // Get the right texture

        float u_coord = (float)(m_animation_index % m_animation_cols) / (float)m_animation_cols;
        float v_coord = (float)(m_animation_index / m_animation_cols) / (float)m_animation_rows;

        float width = 1.0f / (float)m_animation_cols;
        float height = 1.0f / (float)m_animation_rows;

        float tex_coords[] =
        {
            u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width,
            v_coord, u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
        };

        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

        glBindTexture(GL_TEXTURE_2D, current_texture);

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    }
    else
    {
        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    }

    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::update(float delta_time, Entity *collidable_entities, int collidable_entity_count)
{
    if (!m_is_active) return;

    // ----- COLLISION ----- //
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    // ––––– ANIMATION ––––– //
    if (m_animation_indices != nullptr)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float) 1 / SECONDS_PER_FRAME;
            
            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }
        
    // ----- ENGINE ----- //
    if (m_is_engine_on)
    {
        // Increase acceleration @ total rotated angle
        m_acceleration.x += m_engine_power * sin(glm::radians(m_rotated_angle - 180));
        m_acceleration.y += m_engine_power * cos(glm::radians(m_rotated_angle));

        // Decrement fuel
        m_fuel -= 1;
    }
    else
    {
        m_acceleration.x *= 0.9999f;

        if (glm::abs(m_acceleration.x) < 0.0001f)
        {
            m_acceleration.x = 0.0f; // Stop drifting if the x-value of acceleration is too small
        }
    }
    
    // ––––– GRAVITY ––––– //
    m_velocity.x = m_movement.x * m_speed;
    m_velocity.y = m_movement.y * m_speed;
    m_velocity  += m_acceleration * delta_time;
    
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count);
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    
    // ––––– TRANSFORMATIONS ––––– //
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);

    // ----- ROTATION ----- //
    if (m_type == PLAYER)
    {
        m_rotated_angle += m_rotation_angle;
        m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotated_angle), m_rotation_plane);
    }

    // ----- SCALING ----- //
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

bool const Entity::check_collision(Entity* other) const
{
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

bool const Entity::check_screen_collision(glm::mat4& projection) const
{
    float top    = (2.0f / projection[1][1] - projection[1][3]) / 2.0f;
    float bottom = top - 2.0f / projection[1][1];
    float right  = (2.0f / projection[0][0] - projection[0][3]) / 2.0f;
    float left   = right - 2.0f / projection[0][0];

    // Check collision with the screen edges
    if (m_position.x - (m_scale.x / 2.0f) < left ||     // Left
        m_position.x + (m_scale.x / 2.0f) > right ||    // Right
        m_position.y - (m_scale.y / 2.0f) < bottom ||   // Bottom
        m_position.y + (m_scale.y / 2.0f) > top)        // Top
    {
        return true;
    }

    return false;
}

void const Entity::check_collision_y(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            
            if (m_velocity.y > 0) {
                m_position.y   -= y_overlap;
                m_velocity.y    = 0;
                m_collided_top  = true;
            } else if (m_velocity.y < 0) {
                m_position.y      += y_overlap;
                m_velocity.y       = 0;
                m_collided_bottom  = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x     -= x_overlap;
                m_velocity.x      = 0;
                m_collided_right  = true;
            } else if (m_velocity.x < 0) {
                m_position.x    += x_overlap;
                m_velocity.x     = 0;
                m_collided_left  = true;
            }
        }
    }
}

void Entity::render(ShaderProgram *program)
{
    if (!m_is_active) return;
    
    program->set_model_matrix(m_model_matrix);
    
    draw_sprite_from_texture_atlas(program);
}