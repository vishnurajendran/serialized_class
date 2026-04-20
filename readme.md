# Serialized Class

A lightweight, macro-assisted serialization system built in **C++** using **pugixml**.
Designed for game engines and tools, this system provides a simple way to declare data fields, automatically register them, and serialize/deserialize them without repetitive boilerplate.

---

## Features

* **Automatic field registration** (no duplicate lists)
* **Minimal macros** for clean declarations
* **XML serialization/deserialization** using pugixml
* **Supports core types**

    * `int`
    * `float`
    * `bool`
    * `std::string`

* **Scales easily** to large data classes (20–100+ fields)
* Designed for **engine/editor workflows**

---

## Core Idea

Each field:

* Registers itself into a central list on construction
* Knows how to **read/write itself**
* Exposes a simple `get()` / `set()` interface

The container (`DataBase`) simply:

* Iterates all fields
* Serializes or deserializes them

---

## Structure

```
DataBase
 ├── std::vector<FieldBase*> fields
 ├── Serialise()
 └── Deserialize()

FieldBase
 ├── write()
 └── load()

Field<T>
 ├── value
 ├── get() / set()
 ├── write()
 └── load()
```

---

## Usage

### 1. Declare your data class

```cpp
class EngineSettings : public DataBase
{
    DECLARE_FIELD(screenWidth, int, 1920)
    DECLARE_FIELD(screenHeight, int, 1080)
    DECLARE_FIELD(fullscreen, bool, false)
    DECLARE_FIELD(volume, float, 1.0f)
    DECLARE_FIELD(title, std::string, "Meteor Engine")
};
```

---

### 2. Use it

```cpp
EngineSettings settings;

// Set values
settings.screenWidth.set(1280);
settings.title.set("My Game");

// Get values
int width = settings.screenWidth.get();

// Serialize
settings.Serialise();
```

---

## Example Output

```xml
<root>
    <screenWidth>1280</screenWidth>
    <screenHeight>1080</screenHeight>
    <fullscreen>false</fullscreen>
    <volume>1</volume>
    <title>My Game</title>
</root>
```

---

## How It Works

* `DECLARE_FIELD(...)` creates a `Field<T>` instance
* The field registers itself into `DataBase::fields`
* Serialization loops over all registered fields:

```cpp
for (auto f : fields)
    f->write(root);
```

---

## Notes

* Field declaration order matters (C++ member initialization order)
* Currently supports basic types (extendable via templates)
* XML backend uses **pugixml**

---

## Use Cases

* Game engine settings
* Editor tools
* Save/load systems
* Debug/config panels
---

## Dependencies

* [pugixml](https://pugixml.org/)
---

## Inspiration

This system is inspired by:

* Unreal Engine’s `UPROPERTY`
* Reflection systems in modern game engines
---

## Author Notes

Built as part of a custom C++ game engine to:

* Reduce boilerplate
* Enable scalable data systems
* Prepare for editor tooling
---
