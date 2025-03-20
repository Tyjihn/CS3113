enum EntityType { PLATFORM, PLAYER, BACKGROUND };
enum Animation { OFF, ON };

#include <vector>

class Entity
{
private:
    bool m_is_active = true;

    // ––––– PHYSICS (GRAVITY) ––––– //
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    
    float m_width  = 1;
    float m_height = 1;
    
    // ––––– SETUP AND RENDERING ––––– //
    GLuint m_texture_id;
    glm::mat4 m_model_matrix;
    EntityType m_type;
    
    // ––––– TRANSLATIONS ––––– //
    float m_speed;
    glm::vec3 m_movement;

    // ----- ROTATION ----- //
    float m_rotation_angle,
          m_rotated_angle;
    glm::vec3 m_rotation_plane;

    // ----- SCALING ----- //
    glm::vec3 m_scale;
    
    // ––––– ANIMATIONS ––––– //
    std::vector<GLuint> m_texture_ids;
    std::vector<std::vector<int>> m_animations;
    Animation m_current_animation;

    int *m_animation_indices = nullptr;
    float m_animation_time   = 0.0f;
    int m_animation_frames   = 0;
    int m_animation_index    = 0;
    int m_animation_cols     = 0;
    int m_animation_rows     = 0;
    
    // ––––– PHYSICS (ENGINE) ––––– //
    bool m_is_engine_on = false;
    float m_engine_power = 0;
    int m_fuel = 0;
    
    // ––––– PHYSICS (COLLISIONS) ––––– //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;

public:
    // ––––– STATIC ATTRIBUTES ––––– //
    static const int SECONDS_PER_FRAME = 6;

    // ––––– METHODS ––––– //
    Entity();
    Entity(std::vector<GLuint> texture_ids, std::vector<std::vector<int>> animations, 
        float speed, float animation_time, int animation_frames, int animation_index, 
        int animation_cols, int animation_rows, Animation animation);
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program);
    void update(float delta_time, Entity *collidable_entities, int collidable_entity_count);
    void render(ShaderProgram *program);

    // ----- ANIMATION ----- //
    void set_animation_state(Animation new_animation);
    
    void const check_collision_y(Entity *collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity *collidable_entities, int collidable_entity_count);
    bool const check_collision(Entity *other) const;
    bool const check_screen_collision(glm::mat4 &projection_matrix) const;
    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };
    
    // ––––– GETTERS ––––– //
    glm::vec3 const get_position()        const { return m_position;        };
    glm::vec3 const get_movement()        const { return m_movement;        };
    glm::vec3 const get_velocity()        const { return m_velocity;        };
    glm::vec3 const get_acceleration()    const { return m_acceleration;    };
    glm::vec3 const get_scale()           const { return m_scale;           };
    glm::vec3 const get_rotation_plane()  const { return m_rotation_plane;  };
    float     const get_rotation_angle()  const { return m_rotation_angle;  };
    float     const get_rotated_angle()   const { return m_rotated_angle;   };
    int       const get_width()           const { return m_width;           };
    int       const get_height()          const { return m_height;          };
    int       const get_fuel()            const { return m_fuel;            };
    bool      const get_collided_top()    const { return m_collided_top;    };
    bool      const get_collided_bottom() const { return m_collided_bottom; };
    bool      const get_collided_left()   const { return m_collided_left;   };
    bool      const get_collided_right()  const { return m_collided_right;  };
    
    // ––––– SETTERS ––––– //
    void const set_position(glm::vec3 new_position)           { m_position = new_position;             };
    void const set_movement(glm::vec3 new_movement)           { m_movement = new_movement;             };
    void const set_velocity(glm::vec3 new_velocity)           { m_velocity = new_velocity;             };
    void const set_acceleration(glm::vec3 new_acceleration)   { m_acceleration = new_acceleration;     };
    void const set_rotation_axis(glm::vec3 new_plane)         { m_scale = new_plane;                   };
    void const set_rotation_angle(float new_angle)            { m_rotation_angle = new_angle;          };
    void const set_rotated_angle(float new_angle)             { m_rotated_angle = new_angle;           };
    void const set_scale(glm::vec3 new_scale)                 { m_scale = new_scale;                   };
    void const set_width(float new_width)                     { m_width = new_width;                   };
    void const set_height(float new_height)                   { m_height = new_height;                 };
    void const set_fuel(float new_fuel)                       { m_fuel = new_fuel;                     };
    void const set_engine_power(int new_engine)               { m_engine_power = new_engine;           };
    void const set_engine_state(bool new_engine_state)        { m_is_engine_on = new_engine_state;     };
    void const set_m_type(EntityType new_type)                { m_type = new_type;                     };
    void const set_texture_id(GLuint new_id)                  { m_texture_id = new_id;                 };
};