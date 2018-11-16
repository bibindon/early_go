#include "stdafx.hpp"
#include "constants.hpp"

namespace early_go {
const int constants::WINDOW_WIDTH = 1600;
const int constants::WINDOW_HEIGHT = 900;
const float constants::ANIMATION_SPEED = 1.0f/60;
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
const std::string constants::EARLY_HAIR = "model/early_tentative/hair.x";
const std::string constants::EARLY_SKIRT = "model/early_tentative/skirt.x";
}
