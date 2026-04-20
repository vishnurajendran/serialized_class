//
// Created by ssj5v on 20-04-2026.
//

#ifndef FIELD_H
#define FIELD_H
#include "field_base.h"
#include <type_traits>
template<typename T>
struct Field : public FieldBase
{
    T value;

    Field(std::vector<FieldBase*>& registry, const std::string& n, const T& def)
        : FieldBase(registry, n), value(def)
    {}

    const T& get() const { return value; }
    void set(const T& v) { value = v; }

    void write(pugi::xml_node& parent) const override
    {
        parent.append_child(name.c_str()).text().set(value);
    }

    void load(const pugi::xml_node& parent) override
    {
        auto node = parent.child(name.c_str());
        if (!node) return;

        if constexpr (std::is_same_v<T, int>)
            value = node.text().as_int();
        else if constexpr (std::is_same_v<T, float>)
            value = node.text().as_float();
        else if constexpr (std::is_same_v<T, double>)
            value = node.text().as_double();
        else if constexpr (std::is_same_v<T, bool>)
            value = node.text().as_bool();
        else if constexpr (std::is_same_v<T, std::string>)
            value = node.text().as_string();
    }
};
#endif //FIELD_H
