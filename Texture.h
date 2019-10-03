#pragma once
#include <string>
#include <vector>
using namespace std;

class Texture {
public:
    Texture(const string& path, bool repeat = false);
    Texture(vector<string> faces);
    unsigned int getData() const;
    virtual ~Texture();
    int error;
private:
    unsigned int texture;
};
