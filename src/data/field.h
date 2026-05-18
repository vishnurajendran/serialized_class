//
// field.h
// Generic serializable field. Supports int, float, double, bool, std::string.
//

#ifndef FIELD_H
#define FIELD_H
#include <functional>

#include "field_base.h"
#include <type_traits>

template<typename T>
struct Field : public FieldBase
{
private:
    std::function<void(const T&)> onChangeCallback;

public:
    T rawValue;
    Field(std::vector<FieldBase*>& registry, const std::string& n, const T& def)
        : FieldBase(registry, n), rawValue(def)
    {}

    // Accessors
    const T& get() const { return rawValue; }
    T&       get()       { return rawValue; }
    void     set(const T& v) { rawValue = v; if (onChangeCallback) onChangeCallback(rawValue); }
    void     setOnChangeCallback(std::function<void(const T&)> v) { onChangeCallback = v; }

    // Assignment from T so DECLARE_FIELD members feel like plain fields to callers
    Field& operator=(const T& v) { rawValue = v; return *this; }

    // Serialization
    void write(pugi::xml_node& parent) const override
    {
        parent.append_child(name.c_str()).text().set(rawValue);
    }

    void load(const pugi::xml_node& parent) override
    {
        auto node = parent.child(name.c_str());
        if (!node) return;

        if constexpr (std::is_same_v<T, int>)
            rawValue = node.text().as_int();
        else if constexpr (std::is_same_v<T, float>)
            rawValue = node.text().as_float();
        else if constexpr (std::is_same_v<T, double>)
            rawValue = node.text().as_double();
        else if constexpr (std::is_same_v<T, bool>)
            rawValue = node.text().as_bool();
        else if constexpr (std::is_same_v<T, std::string>)
            rawValue = node.text().as_string();
    }
};

#endif // FIELD_H