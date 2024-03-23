function init()
    cube_pos_y = -1.5
    cube_scale = 1.5
    place_mesh("model/cube/cube.x", {-2.0, cube_pos_y, 0.0}, {0.0, 0.0, 0.0}, cube_scale)
    place_mesh("model/cube/cube.x", {-1.0, cube_pos_y, 0.0}, {0.0, 0.0, 0.0}, cube_scale)
    place_mesh("model/cube/cube.x", {0.0, cube_pos_y, 0.0}, {0.0, 0.0, 0.0}, cube_scale)
    place_mesh("model/cube/cube.x", {1.0, cube_pos_y, 0.0}, {0.0, 0.0, 0.0}, cube_scale)
    place_mesh("model/cube/cube.x", {2.0, cube_pos_y, 0.0}, {0.0, 0.0, 0.0}, cube_scale)

    place_mesh("model/tiger/tiger.x", {3.5, 2.0, 1.0}, {0.0, 0.0, 0.0}, 1.2)
    place_mesh("model/tiger/tiger.x", {5.5, 2.0, 1.0}, {0.0, 0.0, 0.0}, 1.0)
    place_mesh("model/Dwarf/Dwarf.x", {4.5, 2.0, 1.0}, {0.0, 0.0, 0.0}, 1.0)

    place_anim_mesh("model/RobotArm/RobotArm.x", {0.0, 0.0, 10.0}, {0.0, 0.0, 0.0}, 1.0)
    place_skin_mesh("model/wolf/wolf.x", {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 1.0)
    place_skin_mesh("model/tiny/tiny.x", {-1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0.003)
end
