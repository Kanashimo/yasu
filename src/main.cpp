#include <iostream>
#include <stdexcept>

#include "src/Application.h"
#include "src/ErrorWindow.h"
#include "src/UI/BackgroundTexture.h"
#include "src/UI/Debug.h"
#include "src/UI/Crop.h"
#include "src/UI/Exit.h"
#include "src/UI/Toolbar.h"
#include "src/UI/Fonts.h"
#include "src/UI/Pipette.h"

int main()
{
    try {
        Application app;
        app.attach<UI::Fonts>();
        app.attach<UI::BackgroundTexture>();
        app.attach<UI::Toolbar>();
        app.attach<UI::Exit>();
        app.attach<UI::Debug>();
        app.attach<UI::Crop>();
        app.attach<UI::Pipette>();
        app.run();
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        ErrorWindow error(e.what());
        return 1;
    }

    return 0;
}
