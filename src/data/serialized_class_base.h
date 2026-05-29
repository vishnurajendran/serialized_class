//
// serialized_class_base.h
// Base class for any object that wants XML field serialization.
//
// Usage:
//   class MyClass : public SerializedClassBase {
//       DECLARE_FIELD(speed,     float,      1.0f)
//       DECLARE_FIELD(label,     std::string,"default")
//       DECLARE_LIST_FIELD(tags, SString,    {})
//   };
//
// Subclasses can also override onSerialise / onDeserialise to manually
// write/read data that doesn't fit the DECLARE_FIELD / DECLARE_LIST_FIELD
// macros (e.g. raw transform members, fixed-size arrays, maps).
//

#ifndef SERIALISEDCLASSBASE_H
#define SERIALISEDCLASSBASE_H

#include <iostream>
#include <string>
#include <vector>
#include <pugixml.hpp>
#include "field.h"
#include "list_field.h"   // brings in ListField<T> and DECLARE_LIST_FIELD

// ─── DECLARE_FIELD ───────────────────────────────────────────────────────────
// Declares a Field<type> member that self-registers with the fields vector.
#define DECLARE_FIELD(fieldName, type, defaultValue) \
public: \
    Field<type> fieldName { fields, #fieldName, defaultValue };

// DECLARE_LIST_FIELD is defined in list_field.h (included above).
// It mirrors DECLARE_FIELD exactly:
//   DECLARE_LIST_FIELD(fieldName, type, defaultValue)
// where defaultValue is a valid std::vector<type> initialiser, e.g. {}.

class SerializedClassBase
{
protected:
    std::vector<FieldBase*> fields;

    // Override hooks — called by serialiseToNode / deserialiseFromNode after
    // the fields vector is processed.
    virtual void onSerialise(pugi::xml_node& /*node*/)        {}
    virtual void onDeserialise(const pugi::xml_node& /*node*/) {}

public:
    virtual ~SerializedClassBase() = default;

    // Expose the field list for external systems (editor, inspector, etc.).
    const std::vector<FieldBase*>& getFields() const { return fields; }

    // ---- Node-based API ----------------------------------------------------

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

    // ---- File-based API ----------------------------------------------------

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