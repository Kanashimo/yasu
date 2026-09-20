#pragma once

struct Instance;

class Module
{
    public:
        virtual ~Module() = default;

        virtual void on_init() {};
        virtual void on_instance_init(Instance &instance) {};
        virtual void on_update(Instance &instance) {};
        virtual void on_after_render(Instance &instance) {};
        virtual void on_close() {};
};
