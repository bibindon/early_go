#include "stdafx.hpp"
#include "constants.hpp"

#include <boost/static_assert.hpp>

namespace early_go {
const int constants::WINDOW_WIDTH = 1600;
const int constants::WINDOW_HEIGHT = 900;
const int constants::STAGE_NUMBER  = 20;
const float constants::ANIMATION_SPEED = 1.0f/60;
const int constants::GRID_NUM_HEIGHT = 9;
const int constants::GRID_NUM_WIDTH  = 9;
BOOST_STATIC_ASSERT(constants::GRID_NUM_WIDTH%2 == 1);
const float constants::GRID_LENGTH = 1.5f;
const float constants::ACTION_INTERVAL = 1.0f/3;
const int constants::ACTION_INTERVAL_FRAME =
    static_cast<int>(constants::ACTION_INTERVAL/constants::ANIMATION_SPEED);
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

const std::string constants::EARLY_BODY  = "model/early_tentative/body.x";
const std::string constants::EARLY_ARMOR = "model/early_tentative/armor.x";
const std::string constants::EARLY_LANCE = "model/early_tentative/lance.x";
const std::string constants::EARLY_SABER = "model/early_tentative/saber.x";
const std::string constants::EARLY_HAIR  = "model/early_tentative/hair.x";
const std::string constants::EARLY_SKIRT = "model/early_tentative/skirt.x";

const std::string constants::SUO_BODY  = "model/suo_tentative/body.x";
const std::string constants::SUO_SABER = "model/suo_tentative/saber.x";
const std::string constants::SUO_ARMOR = "model/suo_tentative/armor.x";

float get_sine_curve(
    const float& animation_count, const float& duration)
{
  float sine_curve{
      std::sin(animation_count/duration*D3DX_PI-D3DX_PI/2)};
  sine_curve += 1.0f;
  sine_curve /= 2.0f;
  return sine_curve;
}
}
