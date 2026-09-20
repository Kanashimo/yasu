#include <iostream>
#include <stdexcept>

#include "src/Application.h"
#include "src/UI/BackgroundTexture.h"
#include "src/UI/Debug.h"
#include "src/UI/Crop.h"
#include "src/UI/Exit.h"

int main()
{
    try {
        Application app;
        app.attach<UI::Exit>();
        app.attach<UI::Debug>();
        app.attach<UI::BackgroundTexture>();
        app.attach<UI::Crop>();
        app.run();
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
