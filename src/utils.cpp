#include "utils.h"

using namespace std;

Utils::Utils() {
}

std::string Utils::getExePathAndName() {
	return Platform::getExePath();
}

std::string Utils::getExeName() {
	std::string fullPath = getExePathAndName();
	std::string::size_type pos = std::string(fullPath).find_last_of("\\/");
	return std::string(fullPath).substr(pos + 1);
}

std::string Utils::getExePath() {
	std::string fullPath = getExePathAndName();
	std::string::size_type pos = std::string(fullPath).find_last_of("\\/");
	return Platform::addTrailingSlash(std::string(fullPath).substr(0, pos));
}

std::vector<std::string> Utils::mergeVectors(std::vector<std::string> vector1, std::vector<std::string> vector2) {
	BOOST_LOG_TRIVIAL(debug) << "Starting merge.";
	BOOST_LOG_TRIVIAL(debug) << "Vector 1 size: " << vector1.size() << ".";
	BOOST_LOG_TRIVIAL(debug) << "Vector 2 size: " << vector2.size() << ".";
	std::vector <std::string> result(vector1.size() + vector2.size());
	BOOST_LOG_TRIVIAL(debug) << "New size: " << result.size() << ".";
	int index = 0;
	BOOST_LOG_TRIVIAL(debug) << "Adding first vector.";
	for (unsigned int i = 0; i < vector1.size(); i++) {
		result[index] = vector1[i];
		index++;
	}
	BOOST_LOG_TRIVIAL(debug) << "Adding second vector.";
	for (unsigned int i = 0; i < vector2.size(); i++) {
		result[index] = vector2[i];
		index++;
	}
	BOOST_LOG_TRIVIAL(debug) << "Finished merge.";
	return result;
}

std::vector<std::string> Utils::arrayToVector(int size, char** array) {
	std::vector <std::string> result(size);
	for (int i = 0; i < size; i++) {
		result[i] = array[i];
	}
	return result;
}

std::string Utils::vectorToString(std::vector<std::string> strings) {
	std::string result = "";
	for (int i = 0; i < strings.size(); i++) {
		result += strings.at(i);
		if (i != strings.size()) {
			result += " ";
		}
	}
	return result;
}

std::vector<std::string> Utils::splitString(std::string input, std::string delimiter) {
	size_t start = 0;
  size_t end;
	std::vector<std::string> result;

	while ((end = input.find(delimiter, start)) != std::string::npos)
	{
		result.push_back(input.substr(start, end - start));
		start = end + delimiter.length();
	}
	result.push_back(input.substr(start));
	return result;
}

std::string Utils::ws2s(std::wstring s) {
	return string(s.begin(), s.end());
}

void Utils::addMatchingFilesToExistingVector(std::vector<std::string>& files, std::string path, std::regex regex) {
	files = Platform::listDirectory(path, regex);
}

std::vector<std::string> Utils::addMatchingFilesToVector(std::string path, std::regex regex) {
	return Platform::listDirectory(path, regex);
}

std::string Utils::launchAppReturnOutput(std::string executable, char** argv) {
	return Platform::launchApplicationCapturingOutput(executable, argv);
}
