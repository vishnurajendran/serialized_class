//
// serialized_class_base.h
// Base class for any object that wants XML field serialization.
//
// Usage:
//   class MyClass : public SerializedClassBase {
//       DECLARE_FIELD(speed,   float,       1.0f)
//       DECLARE_FIELD(label,   std::string, "default")
//   };
//
// Subclasses can also override onSerialise / onDeserialise to manually
// write/read data that doesn't fit the DECLARE_FIELD macro (e.g. raw
// transform members, arrays, maps).
//

#ifndef SERIALISEDCLASSBASE_H
#define SERIALISEDCLASSBASE_H

#include <iostream>
#include <string>
#include <vector>
#include <pugixml.hpp>
#include "field.h"

// DECLARE_FIELD
// Declares a Field<type> member that self-registers with the fields vector.
// Subclasses just place this in their class body; no further wiring needed.
#define DECLARE_FIELD(fieldName, type, defaultValue) \
public: \
    Field<type> fieldName { fields, #fieldName, defaultValue };

class SerializedClassBase
{
protected:
    std::vector<FieldBase*> fields;

    // Override hooks
    // Called by serialiseToNode / deserialise FromNode after the fields vector
    // is processed. Override in subclasses to handle data that lives outside
    virtual void onSerialise(pugi::xml_node& /*node*/)        {}
    virtual void onDeserialise(const pugi::xml_node& /*node*/) {}

public:
    virtual ~SerializedClassBase() = default;

    // Node-based API (used by scene serializer & entity hierarchy)
    // Write all fields + hook data INTO an already-existing xml_node.
    void serialiseToNode(pugi::xml_node& node)
    {
        for (auto* f : fields)
            f->write(node);
        onSerialise(node);
    }

    // Read all fields + hook data FROM an existing xml_node.
    void deserialiseFromNode(const pugi::xml_node& node)
    {
        for (auto* f : fields)
            f->load(node);
        onDeserialise(node);
    }

    // File-based API (standalone objects, editor save-as, etc.)
    void serialiseToFile(const std::string& filePath)
    {
        pugi::xml_document doc;
        pugi::xml_node root = doc.append_child("root");
        serialiseToNode(root);

        if (doc.save_file(filePath.c_str()))
            std::cout << "[Serialise] Saved to " << filePath << "\n";
        else
            std::cerr << "[Serialise] Failed to save " << filePath << "\n";
    }

    void deserialiseFromFile(const std::string& filePath)
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filePath.c_str());
        if (!result)
        {
            std::cerr << "[Deserialise] Failed to load " << filePath
                      << ": " << result.description() << "\n";
            return;
        }
        deserialiseFromNode(doc.child("root"));
    }
};

#endif // SERIALISEDCLASSBASE_H