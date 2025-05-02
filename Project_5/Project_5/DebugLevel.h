#include "Scene.h"

class DebugLevel : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_font_texture_id;
    int ENEMY_COUNT = 0;

    // ————— DESTRUCTOR ————— //
    ~DebugLevel();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program, ShaderProgram* clone_program) override;
    void player_death();
};
