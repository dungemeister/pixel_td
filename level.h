#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

class Level{
public:
    Level(std::string map_file){
        std::fstream f(map_file);
        std::cout << "Map " << map_file << "\n";
        std::string token;
        std::string line;
        std::string value;
        std::getline(f, token);
        auto pos = token.find('x');
        if(pos == std::string::npos){
            SDL_Log("ERROR: fail to parse .map file");
            return;
        }
        rows = std::atoi(token.substr(0, pos).c_str());
        columns = std::atoi(token.substr(pos + 1, token.size() - 1).c_str());

        std::cout << "Size " << rows << "x" << columns << "\n";
        while(std::getline(f, line)){
            std::stringstream ss(line);
            while (std::getline(ss, token, ' ')) {
                if (!token.empty()) { // Avoid adding empty strings if there are multiple spaces
                    m_tokens.push_back(token);
                }
            }
        }
    }

    std::pair<size_t, size_t> get_resolution() const { return {rows, columns}; }
    const std::vector<std::string>& get_tokens() const { return m_tokens; }
private:
    std::vector<std::string> m_tokens;
    size_t columns;   
    size_t rows;   
};