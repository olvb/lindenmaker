#pragma once
#include "geometry.hpp"
#include "shader_program.hpp"
#include "transform.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace lindenmaker {

/** Abstract 3D object class */
class Object
{
public:
    Transform transform;

    virtual ~Object() = 0;
    virtual void draw(const ShaderProgram& program, const glm::mat4& parent_matrix) const = 0;
};

/** Object with a geometry */
class GeometryObject : public Object
{
public:
    GeometryObject(std::shared_ptr<Geometry> geometry);
    void draw(const ShaderProgram& program, const glm::mat4& parent_matrix) const;

private:
    std::shared_ptr<Geometry> geometry_;
};

/** Object made of other objects */
template <typename T>
class CompositeObject : public Object
{
public:
    CompositeObject() = default;
    CompositeObject(const std::vector<T>& objects);
    void draw(const ShaderProgram& program, const glm::mat4& parent_matrix) const;
    void add_object(T object);

private:
    std::vector<T> objects_;
};

template <typename T>
CompositeObject<T>::CompositeObject(const std::vector<T>& objects) : objects_(objects)
{
}

template <typename T>
void CompositeObject<T>::add_object(T object)
{
    objects_.push_back(object);
}

template <typename T>
void CompositeObject<T>::draw(const ShaderProgram& program, const glm::mat4& parent_matrix) const
{
    const auto model_view_matrix = parent_matrix * transform.get_matrix();

    for (const auto& object : objects_) {
        object.draw(program, model_view_matrix);
    }
}
}
