#include "utils.h"

Utils::Utils() {
}

std::vector<std::string> Utils::mergeVectors(std::vector<std::string> vector1, std::vector<std::string> vector2) {
    std::vector <std::string> result (vector1.size() + vector2.size());
    int index = 0;
    for(int i = 0; i < vector1.size(); i++) {
        result[index] = vector1[i];
        index++;
    }
    for(int i = 0; i < vector2.size(); i++) {
        result[index] = vector2[i];
        index++;
    }
    return result;
}

std::vector<std::string> Utils::arrayToVector(int size, char** array) {
    std::vector <std::string> result (size);
    for (int i = 0; i < size; i++) {
        result[i] = array[i];
    }
    return result;
}

std::vector<std::string> Utils::splitString(std::string input, std::string delimiter) {
      unsigned start = 0;
      unsigned end;
      std::vector<std::string> result;

      while( (end = input.find(delimiter, start)) != std::string::npos )
      {
            result.push_back(input.substr(start, end-start));
            start = end + delimiter.length();
      }
      result.push_back(input.substr(start));
      return result;
}
