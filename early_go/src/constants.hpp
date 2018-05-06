#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include "stdafx.hpp"
// TODO: if constants become large, make namespace more nest.
namespace early_go {
struct constants {
  const static int         WINDOW_WIDTH;
  const static int         WINDOW_HEIGHT;
  const static float       ANIMATION_SPEED;
  const static std::string APP_NAME;
  const static std::string DATABASE_NAME;
  const static std::string FAILED_TO_READ_X_FILE_MESSAGE;
  const static std::string FAILED_TO_CREATE_HARDWARE_MODE_MESSAGE;
  const static std::string FAILED_TO_CREATE_WINDOW_MESSAGE;
  const static std::string MESH_FILE_NAME;
  const static std::string MESH_FILE_NAME2;
  const static std::string ANIMATION_MESH_FILE_NAME;
  const static std::string SKINNED_ANIMATION_MESH_FILE_NAME;
  const static std::string SKINNED_ANIMATION_MESH_FILE_NAME2;
};
} /* namespace early_go */
#endif
