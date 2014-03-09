#ifndef UTILS_H
#define	UTILS_H

#include <string>
#include <stdlib.h>
#include <vector>
#include "../config/ConfigReader.h"

class Utils {
    public:
        static std::vector<std::string> mergeVectors(std::vector<std::string>, std::vector<std::string>);
        static std::vector<std::string> arrayToVector(int, char**);
        static std::vector<std::string> splitString(std::string, std::string);
    private:
        Utils();
    protected:
};

#endif	/* UTILS_H */
