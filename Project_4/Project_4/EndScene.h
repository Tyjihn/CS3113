#include "Scene.h"

class EndScene : public Scene {
public:
    // ����� STATIC ATTRIBUTES ����� //
    GLuint m_font_texture_id;
    bool m_has_played_sound = false;

    // ����� DESTRUCTOR ����� //
    ~EndScene();

    // ����� METHODS ����� //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
