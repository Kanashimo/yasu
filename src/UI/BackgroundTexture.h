#pragma once

#include <unordered_map>
#include <glbinding/gl/gl.h>

#include "src/Application.h"
#include "src/Module.h"


namespace UI {

    class BackgroundTexture : public Module
    {
        public:
            void on_init() override;
            void on_instance_init(Instance &instance) override;
            void on_update(Instance &instance) override;

            gl::GLuint *get_texture(Instance &instance);

            ~BackgroundTexture();

        private:
            double begin_time = 0.0;
            float fade = 0.0f;

            std::unordered_map<Instance*, gl::GLuint> textures;
    };

}
