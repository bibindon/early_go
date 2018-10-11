#include "stdafx.hpp"
#include "constants.hpp"

namespace early_go {
const int constants::WINDOW_WIDTH = 1600;
const int constants::WINDOW_HEIGHT = 900;
#if defined(_WIN64) && defined(_WIN32)
const float constants::ANIMATION_SPEED = 0.02f/120;
#elif !defined(_WIN64) && defined(_WIN32)
const float constants::ANIMATION_SPEED = 0.02f;
#endif
const std::string constants::APP_NAME = "early_go";
const std::string constants::DATABASE_NAME = ".res";
const std::string constants::MESH_FILE_NAME = "model/tiger/tiger.x";
const std::string constants::MESH_FILE_NAME2 = "model/Dwarf/Dwarf.x";
const std::string constants::ANIMATION_MESH_FILE_NAME =
    "model/RobotArm/RobotArm.x";
const std::string constants::SKINNED_ANIMATION_MESH_FILE_NAME =
    "model/wolf/wolf.x";
const std::string constants::SKINNED_ANIMATION_MESH_FILE_NAME2 =
    "model/tiny/tiny.x";

const std::string constants::EARLY_BODY = "model/early_tentative/body.x";
const std::string constants::EARLY_ARMOR = "model/early_tentative/armor.x";
const std::string constants::EARLY_LANCE = "model/early_tentative/lance.x";
const std::string constants::EARLY_SABER = "model/early_tentative/saber.x";
}
