#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include "stdafx.hpp"
// TODO: if constants become large, make namespace more nest.
namespace early_go {
namespace constants {
  const static int WINDOW_WIDTH = 800;
  const static int WINDOW_HEIGHT = 600;
  const static float ANIMATION_SPEED = 0.03f;
  const static std::string APP_NAME = "early_go";
  const static std::string FAILED_TO_READ_X_FILE_MESSAGE =
      "Xファイルの読み込みに失敗しました";
  const static std::string FAILED_TO_CREATE_HARDWARE_MODE_MESSAGE =
      "正常に起動できませんでした。動作が重くなる可能性があります。";
  const static std::string FAILED_TO_CREATE_WINDOW_MESSAGE =
      "Failed to create a window.";
  const static std::string MESH_FILE_PATH = "../early_go/res/Tiger.x";
  const static std::string MESH_FILE_PATH2 = "../early_go/res/Dwarf.x";
  const static std::string ANIMATION_MESH_FILE_PATH =
      "../early_go/res/RobotArm.x";
} /* namespace constants */
} /* namespace early_go */
#endif
