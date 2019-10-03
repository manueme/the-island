#include "Material.h"

Material::Material(const shared_ptr<GameObject>& parent)
    : Component("material", parent) {}

void Material::setWireframe(bool w)
{
    wireframe = w;
}

string Material::getLocalPath(const string& path)
{
    const int endPath = path.find_last_of('/');
    return path.substr(0, endPath + 1);
}

Material::~Material() = default;

ComponentKey Material::getComponentKey()
{
    return MaterialComponent;
}
