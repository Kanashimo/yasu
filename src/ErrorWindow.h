#pragma once

#include <string>
#include <cstdlib>

class ErrorWindow
{
    public:

        ErrorWindow(std::string error)
        {
            if (std::system("command -v kdialog > /dev/null 2>&1") == 0) {
                if (error.length() < 30)
                {
                    int iterations = 30 - error.length();
                    for (int i = 0; i < iterations; i++) {
                        error += " ";
                    }
                }
                std::string cmd = "kdialog --title 'An error has occured' --error '" + error + "'";
                std::system(cmd.c_str());
            } else if (std::system("command -v zenity > /dev/null 2>&1") == 0) {
                std::string cmd = "zenity --title 'An error has occured' --error --text '" + error + "'";
                std::system(cmd.c_str());
            }
        }

    private:

        std::string format_error(std::string error)
        {
            std::string formated_error;

            size_t i = 0;
            for (auto &c : error)
            {
                if (i > 30 && c == ' ')
                {
                    formated_error += "<br>";
                    i = 0;
                }

                formated_error += c;
                i++;
            }

            return formated_error;
        }
};
