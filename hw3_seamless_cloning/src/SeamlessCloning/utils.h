#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <QtCore/QPoint>
#include <QtGui/QImage>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <iostream>
/* some utility functions
 */
class Utils {
    public:
        static std::string getCurrentTimeString(){
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);

            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch() % std::chrono::seconds(1)).count();
            std::stringstream ss;
            // std::localtime may cause data races
            ss << std::put_time(std::localtime(&now_c), "%T") << "."
                << std::setw(3) << std::setfill('0') << ms;
            return ss.str();
        }
};

#endif
