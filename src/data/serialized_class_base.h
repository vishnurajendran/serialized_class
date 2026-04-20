//
// Created by ssj5v on 20-04-2026.
//

#ifndef SERIALISEDCLASSBASE_H
#define SERIALISEDCLASSBASE_H
#include <iostream>
#include <pugixml.hpp>
#include "field.h"
#include <vector>
// ----------------------------------------------
// Macros
// ----------------------------------------------

#define DECLARE_FIELD(name, type, defaultValue) \
public: \
Field<type> name{fields, #name, defaultValue};

class SerializedClassBase
{
protected:
    std::vector<FieldBase*> fields;
    pugi::xml_document doc;
    pugi::xml_node root;

public:
    virtual ~SerializedClassBase() = default;

    void Serialise(std::string filePath)
    {
        doc.reset();
        root = doc.append_child("root");

        for (auto f : fields)
            f->write(root);

        if (doc.save_file(filePath.c_str()))
        {
            std::cout<<"File saved successfully!"<<std::endl;
        }
    }

    void Deserialize(const pugi::xml_node& inRoot)
    {
        for (auto f : fields)
            f->load(inRoot);
    }
};
#endif //SERIALISEDCLASSBASE_H
