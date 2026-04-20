//
// Created by ssj5v on 20-04-2026.
//

#ifndef FIELD_BASE_H
#define FIELD_BASE_H
#include <string>
#include <vector>
#include <pugixml.hpp>

struct FieldBase
{
    std::string name;

    FieldBase(std::vector<FieldBase*>& registry, const std::string& n)
        : name(n)
    {
        registry.push_back(this);
    }

    virtual ~FieldBase() = default;

    virtual void write(pugi::xml_node& parent) const = 0;
    virtual void load(const pugi::xml_node& parent) = 0;
};
#endif //FIELD_BASE_H
