#include "constants.hpp"
#include "stdafx.hpp"

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
const std::string constants::FAILED_TO_READ_X_FILE_MESSAGE =
    "Xファイルの読み込みに失敗しました";
const std::string constants::FAILED_TO_CREATE_HARDWARE_MODE_MESSAGE =
    "正常に起動できませんでした。動作が重くなる可能性があります。";
const std::string constants::FAILED_TO_CREATE_WINDOW_MESSAGE =
    "Failed to create a window.";
const std::string constants::MESH_FILE_NAME = "tiger.x";
const std::string constants::MESH_FILE_NAME2 = "Dwarf.x";
const std::string constants::ANIMATION_MESH_FILE_NAME = "RobotArm.x";
const std::string constants::SKINNED_ANIMATION_MESH_FILE_NAME = "wolf.x";
const std::string constants::SKINNED_ANIMATION_MESH_FILE_NAME2 = "tiny.x";

const std::string constants::EARLY_BODY = "early_tentative_body.x";
const std::string constants::EARLY_ARMOR = "early_tentative_armor.x";
const std::string constants::EARLY_LANCE = "early_tentative_lance.x";
const std::string constants::EARLY_SABER = "early_tentative_saber.x";
}
