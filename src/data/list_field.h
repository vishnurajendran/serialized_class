//
// list_field.h
// Serialisable std::vector field. Mirrors Field<T> exactly so it slots into
// SerializedClassBase alongside DECLARE_FIELD without any other changes.
//
// Natively supported element types (no specialisation required):
//   int, float, double, bool, std::string, any enum
//
// For engine math types (SVector3, SQuaternion, SString, etc.) add a
// ListFieldElementIO<T> specialisation in field_engine_types.h — see the
// comment on ListFieldElementIO below.
//
// Usage:
//   class MyClass : public SerializedClassBase {
//       DECLARE_LIST_FIELD(tags,      std::string, {})
//       DECLARE_LIST_FIELD(scores,    int,         {})
//       DECLARE_LIST_FIELD(waypoints, SVector3,    {})  // needs specialisation
//   };
//
// XML format produced:
//   <tags>
//     <item>Player</item>
//     <item>Enemy</item>
//   </tags>
//

#ifndef LIST_FIELD_H
#define LIST_FIELD_H

#include <functional>
#include <string>
#include <type_traits>
#include <vector>
#include "field_base.h"

// --- ListFieldElementIO<T> ---------------------------------------------------
//
// Controls how a single element of type T is written to and read from an
// <item> xml_node.
//
// The primary template handles: int, float, double, bool, std::string, enum.
//
// To support additional types, add a full specialisation in field_engine_types.h:
//
//   template<>
//   struct ListFieldElementIO<SVector3> {
//       static void writeElement(pugi::xml_node& node, const SVector3& v) { ... }
//       static SVector3 readElement(const pugi::xml_node& node)           { ... }
//   };

template<typename T>
struct ListFieldElementIO
{
    static void writeElement(pugi::xml_node& node, const T& value)
    {
        if constexpr (std::is_enum_v<T>)
            node.text().set(static_cast<int>(value));
        else if constexpr (std::is_same_v<T, std::string>)
            // pugixml's text().set() has no std::string overload — must pass c_str().
            node.text().set(value.c_str());
        else
            // Covers int, float, double, bool — pugixml has direct overloads for all.
            node.text().set(value);
    }

    static T readElement(const pugi::xml_node& node)
    {
        if constexpr (std::is_enum_v<T>)
            return static_cast<T>(node.text().as_int());
        else if constexpr (std::is_same_v<T, int>)
            return node.text().as_int();
        else if constexpr (std::is_same_v<T, float>)
            return node.text().as_float();
        else if constexpr (std::is_same_v<T, double>)
            return node.text().as_double();
        else if constexpr (std::is_same_v<T, bool>)
            return node.text().as_bool();
        else if constexpr (std::is_same_v<T, std::string>)
            return node.text().as_string();
        else
        {
            // If you hit this, add a ListFieldElementIO<YourType> specialisation
            // in field_engine_types.h.
            static_assert(sizeof(T) == 0,
                "ListField<T>: no element serialiser for T. "
                "Add a ListFieldElementIO<T> specialisation in field_engine_types.h.");
            return T{};
        }
    }
};

// --- ListField<T> -------------------------------------------------------------

template<typename T>
class ListField : public FieldBase
{
public:
    ListField(std::vector<FieldBase*>& registry,
              const std::string&       fieldName,
              std::vector<T>           defaultValue = {})
        : FieldBase(registry, fieldName)
        , rawValue(std::move(defaultValue))
    {}

    // ---- Read-only access --------------------------------------------------

    [[nodiscard]] const std::vector<T>& get()       const { return rawValue; }
    [[nodiscard]] const T& operator[](size_t index) const { return rawValue[index]; }
    [[nodiscard]] size_t   size()                   const { return rawValue.size(); }
    [[nodiscard]] bool     isEmpty()                const { return rawValue.empty(); }

    auto begin() const { return rawValue.begin(); }
    auto end()   const { return rawValue.end(); }

    // ---- Mutation (all fire the onChange callback) -------------------------

    void set(const std::vector<T>& v)  { rawValue = v;            fireCallback(); }
    void set(std::vector<T>&& v)       { rawValue = std::move(v); fireCallback(); }

    void add(const T& item)            { rawValue.push_back(item); fireCallback(); }

    // Inserts item before the element at index. Clamps to [0, size].
    void addAt(size_t index, const T& item)
    {
        rawValue.insert(rawValue.begin() + std::min(index, rawValue.size()), item);
        fireCallback();
    }

    // No-op if index is out of range.
    void removeAt(size_t index)
    {
        if (index >= rawValue.size()) return;
        rawValue.erase(rawValue.begin() + index);
        fireCallback();
    }

    void clear() { rawValue.clear(); fireCallback(); }

    void setOnChangeCallback(std::function<void(const std::vector<T>&)> cb)
    {
        onChangeCallback = std::move(cb);
    }

    // ---- Serialisation -----------------------------------------------------

    void write(pugi::xml_node& parent) const override
    {
        pugi::xml_node listNode = parent.append_child(name.c_str());
        for (const T& item : rawValue)
            ListFieldElementIO<T>::writeElement(listNode.append_child("item"), item);
    }

    void load(const pugi::xml_node& parent) override
    {
        const pugi::xml_node listNode = parent.child(name.c_str());
        if (!listNode) return;

        rawValue.clear();
        for (const pugi::xml_node& itemNode : listNode.children("item"))
            rawValue.push_back(ListFieldElementIO<T>::readElement(itemNode));

        // Fire once after the full vector is rebuilt — not per element — so
        // listeners always see the complete final state.
        fireCallback();
    }

private:
    void fireCallback()
    {
        if (onChangeCallback) onChangeCallback(rawValue);
    }

    // rawValue is private — all mutation goes through the guarded methods above
    // so the onChange callback is never bypassed.
    std::vector<T>                             rawValue;
    std::function<void(const std::vector<T>&)> onChangeCallback;
};

// --- Macro --------------------------------------------------------------------

#define DECLARE_LIST_FIELD(fieldName, type, defaultValue) \
public: \
    ListField<type> fieldName { fields, #fieldName, defaultValue };

#endif // LIST_FIELD_H