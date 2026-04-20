//
// Created by ssj5v on 20-04-2026.
//

#ifndef TESTDATA_H
#define TESTDATA_H
#include "../data/serialized_class_base.h"

class test_data : public SerializedClassBase
{
    DECLARE_FIELD(screenWidth, int, 1920)
    DECLARE_FIELD(screenHeight, int, 1080)
    DECLARE_FIELD(fullscreen, bool, false)
    DECLARE_FIELD(volume, float, 1.0f)
    DECLARE_FIELD(title, std::string, "Test title")
};

#endif //TESTDATA_H
