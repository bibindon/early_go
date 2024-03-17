#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include "stdafx.hpp"

// TODO: if constants become large, make namespace more nest.
namespace early_go
{
struct constants
{
    const static int WINDOW_WIDTH;
    const static int WINDOW_HEIGHT;
    const static int MAX_STAGE_NUMBER;
    const static float ANIMATION_SPEED;
    const static LONG TEXTURE_PIXEL_SIZE;
    const static LONG EMPTY_TEXTURE_SIZE;
    const static int GRID_NUM_HEIGHT;
    const static int GRID_NUM_WIDTH;
    const static float GRID_LENGTH;
    const static float ACTION_INTERVAL;
    const static int ACTION_INTERVAL_FRAME;
    const static std::string APP_NAME;
    const static std::string DATABASE_NAME;
    const static std::string MESH_FILE_NAME;
    const static std::string MESH_FILE_NAME2;
    const static std::string ANIMATION_MESH_FILE_NAME;
    const static std::string SKINNED_ANIMATION_MESH_FILE_NAME;
    const static std::string SKINNED_ANIMATION_MESH_FILE_NAME2;

    const static std::string EARLY_BODY;
    const static std::string EARLY_ARMOR;
    const static std::string EARLY_LANCE;
    const static std::string EARLY_SABER;

    const static std::string EARLY_HAIR;
    const static std::string EARLY_SKIRT;

    const static std::string SUO_BODY;
    const static std::string SUO_ARMOR;
    const static std::string SUO_SABER;
};

enum class direction
{
    FRONT,
    LEFT,
    BACK,
    RIGHT,
    NONE,
};

float get_sine_curve(const float &, const float &);
int get_next_pow_2(const int &);

} /* namespace early_go */
#endif
