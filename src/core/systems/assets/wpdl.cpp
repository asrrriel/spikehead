#include <sys/wpdl.h>
#include <iostream>

struct wpdl_pass_result_t {
    bool error = true;
    size_t v = 0;
    bool centered = false;
    char relative = 's';  
};

wpdl_pass_result_t wpdl_pass_pos(const std::string& tmp,bool vertical,wpdl_result_t first_window,wpdl_result_t last_window,wpdl_result_t screen) {
    wpdl_pass_result_t result;
    result.error = true;
    if (tmp.length() < 2) return result; //error

    switch(tmp[0]){
        case 'a':
            result.v = std::stol(tmp.substr(1));
            break;
        case 'f':
            if(first_window.error)
                return result; // this is the first window, dumbass
            switch (tmp[1]) {
                case 'c':
                    result.centered = true;
                    break;
                case 'd':
                case 'l':
                    switch(tmp[2]){
                        case 'p':
                            result.v = float(std::stol(tmp.substr(3)))/100 * (vertical ? first_window.height : first_window.width);
                            break;
                        default:
                            result.v = std::stol(tmp.substr(2));
                    }
                    result.v += vertical ? first_window.y : first_window.x;
                    break;
                case 'u':
                case 'r':
                    result.v = vertical ? first_window.y + first_window.height : first_window.x + first_window.width;
                    switch(tmp[2]){
                        case 'p':
                            result.v -= float(std::stol(tmp.substr(3)))/100 * (vertical ? first_window.height : first_window.width);
                            break;
                        default:
                            result.v -= std::stol(tmp.substr(2));
                    }
                    break;
                default:
                    return result; //error
            }
            break;
        case 'l':
            if(last_window.error)
                return result; // this is the first window, dumbas
            switch (tmp[1]) {
                case 'c':
                    result.centered = true;
                    break;
                case 'd':
                case 'l':
                    switch(tmp[2]){
                        case 'p':
                            result.v = float(std::stol(tmp.substr(3)))/100 * (vertical ? last_window.height : last_window.width);
                            break;
                        default:
                            result.v = std::stol(tmp.substr(2));
                    }
                    result.v += vertical ? last_window.y : last_window.x;
                    break;
                case 'u':
                case 'r':
                    result.v = vertical ? first_window.y + first_window.height : last_window.x + last_window.width;
                    switch(tmp[2]){
                        case 'p':
                            result.v -= float(std::stol(tmp.substr(3)))/100 * (vertical ? last_window.height : last_window.width);
                            break;
                        default:
                            result.v -= std::stol(tmp.substr(2));
                    }
                    break;
                default:
                    return result; //error
            }
            break;
        case 's':
            switch (tmp[1]) {
                case 'c':
                    result.centered = true;
                    break;
                case 'd':
                case 'l':
                    switch(tmp[2]){
                        case 'p':
                            result.v = float(std::stol(tmp.substr(3)))/100 * (vertical ? screen.height : screen.width);
                            break;
                        default:
                            result.v = std::stol(tmp.substr(2));
                    }
                    break;
                case 'u':
                case 'r':
                    result.v = vertical ? screen.height : screen.width;
                    switch(tmp[2]){
                        case 'p':
                            result.v -= float(std::stol(tmp.substr(3)))/100 * (vertical ? screen.height : screen.width);
                            break;
                        default:
                            result.v -= std::stol(tmp.substr(2));
                    }
                    break;
                default:
                    return result; //error
            }
            break;
        default:
            return result; //error
    }

    result.error = false;
    return result;
}

size_t wpdl_pass_size(const std::string& tmp,size_t pos_val,bool vertical,wpdl_result_t first_window,wpdl_result_t last_window,wpdl_result_t screen) {
    size_t result = 0;
    if (tmp.length() < 2) return result; //error

    switch(tmp[0]){
        case 'a':
            result = std::stol(tmp.substr(1));
            break;
        case 'f':
            if(first_window.error)
                return result; // this is the first window, dumbass
            switch (tmp[1]) {
                case 'd':
                case 'l':
                    switch(tmp[2]){
                        case 'p':
                            result = float(std::stol(tmp.substr(3)))/100 * (vertical ? first_window.height : first_window.width);
                            break;
                        default:
                            result = std::stol(tmp.substr(2));
                    }
                    result += vertical ? first_window.y : first_window.x;
                    break;
                case 'u':
                case 'r':
                    result = vertical ? first_window.y + first_window.height : first_window.x + first_window.width;
                    switch(tmp[2]){
                        case 'p':
                            result -= float(std::stol(tmp.substr(3)))/100 * (vertical ? first_window.height : first_window.width);
                            break;
                        default:
                            result -= std::stol(tmp.substr(2));
                    }
                    break;
                default:
                    return result; //error
            }
            break;
        case 'l':
            if(last_window.error)
                return result; // this is the first window, dumbas
            switch (tmp[1]) {
                case 'd':
                case 'l':
                    switch(tmp[2]){
                        case 'p':
                            result = float(std::stol(tmp.substr(3)))/100 * (vertical ? last_window.height : last_window.width);
                            break;
                        default:
                            result = std::stol(tmp.substr(2));
                    }
                    result += vertical ? last_window.y : last_window.x;
                    break;
                case 'u':
                case 'r':
                    result = vertical ? first_window.y + first_window.height : last_window.x + last_window.width;
                    switch(tmp[2]){
                        case 'p':
                            result -= float(std::stol(tmp.substr(3)))/100 * (vertical ? last_window.height : last_window.width);
                            break;
                        default:
                            result -= std::stol(tmp.substr(2));
                    }
                    break;
                default:
                    return result; //error
            }
            break;
        case 's':
            switch (tmp[1]) {
                case 'd':
                case 'l':
                    switch(tmp[2]){
                        case 'p':
                            result = float(std::stol(tmp.substr(3)))/100 * (vertical ? screen.height : screen.width);
                            break;
                        default:
                            result = std::stol(tmp.substr(2));
                    }
                    break;
                case 'u':
                case 'r':
                    switch(tmp[2]){
                        case 'p':
                            result -= float(std::stol(tmp.substr(3)))/100 * (vertical ? screen.height : screen.width);
                            break;
                        default:
                            result -= std::stol(tmp.substr(2));
                    }
                    break;
                default:
                    return result; //error
            }
            break;
        default:
            return result; //error

    }

    return result;
}


wpdl_result_t wpdl_parse(const std::string& wpdl,wpdl_result_t first_window,wpdl_result_t last_window,wpdl_result_t screen) {
    wpdl_result_t result;
    result.error = true;
    bool is_x_centered = false;
    bool is_y_centered = false;
    char relative_x = 's';
    char relative_y = 's';

    std::string tmp = wpdl;

    // x
    auto x_end = tmp.find(";");
    if(x_end == std::string::npos){
        std::cerr << "[FATAL] x's end not found.\n";
        return result;
    }

    auto x_result = wpdl_pass_pos(tmp.substr(0,x_end),false,first_window,last_window,screen);
    if(x_result.error){
        std::cerr << "[FATAL] x not found.\n";
        return result; //error
    }

    is_x_centered = x_result.centered;
    relative_x = x_result.relative;
    result.x = x_result.v;

    tmp = tmp.substr(x_end+1);

    // x
    auto y_end = tmp.find(";");
    if(y_end == std::string::npos) {
        std::cerr << "[FATAL] y's end not found.\n";
        return result;
    }

    auto y_result = wpdl_pass_pos(tmp.substr(0,y_end),true,first_window,last_window,screen);
    if(y_result.error){
        std::cerr << "[FATAL] y not found.\n";
        return result; //error
    }

    is_y_centered = y_result.centered;
    relative_y = y_result.relative;
    result.y = y_result.v;

    tmp = tmp.substr(y_end+1);

    // width
    auto width_end = tmp.find(";");
    if(width_end == std::string::npos){
        std::cerr << "[FATAL] width's end not found.\n";
        return result;
    }

    auto width_result = wpdl_pass_size(tmp.substr(0,width_end),result.x,false,first_window,last_window,screen);
    if(width_result == 0){
        std::cerr << "[FATAL] width not found.\n";
        return result; //error
    } 

    result.width = width_result;

    tmp = tmp.substr(width_end+1);

    // height
    auto height_end = tmp.length();
    if(height_end == std::string::npos){
        std::cerr << "[FATAL] height's end not found.\n";
    }

    auto height_result = wpdl_pass_size(tmp.substr(0,height_end),result.y,true,first_window,last_window,screen);
    if(height_result == 0){
        std::cerr << "[FATAL] height not found.\n";
        return result; //error
    } 

    result.height = height_result;


    if(is_x_centered){
        switch (relative_x) {
            case 's':
                result.x = screen.width/2 - result.width/2;
                break;
            case 'l':
                result.x = last_window.width/2 - result.width/2;
                break;
            default:
                return result; //error
        }
    }
    if(is_y_centered){
        switch (relative_y) {
            case 's':
                result.y = screen.height/2 - result.height/2;
                break;
            case 'l':
                result.y = last_window.height/2 - result.height/2;
                break;
            default:
                return result; //error
        }
    }

    result.error = false;
    return result;
}