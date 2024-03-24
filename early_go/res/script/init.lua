function init()
    cube_pos_y = -0.75
    cube_scale = 0.75
    stage_grid = 1.5

    for j = 0, 8 do
        for i = -4, 4 do
            place_mesh("model/cube/cube.x", {stage_grid * i, cube_pos_y, stage_grid * j}, {0.0, 0.0, 0.0}, cube_scale)
        end
    end




    place_mesh("model/tiger/tiger.x", {3.5, 2.0, -10.0}, {0.0, 0.0, 0.0}, 1.2)
    place_mesh("model/tiger/tiger.x", {5.5, 2.0, -10.0}, {0.0, 0.0, 0.0}, 1.0)
    place_mesh("model/Dwarf/Dwarf.x", {4.5, 2.0, -10.0}, {0.0, 0.0, 0.0}, 1.0)

    place_anim_mesh("model/RobotArm/RobotArm.x", {0.0, 0.0, -10.0}, {0.0, 0.0, 0.0}, 1.0)
    place_skin_mesh("model/wolf/wolf.x", {1.0, 0.0, -10.0}, {0.0, 0.0, 0.0}, 1.0)
    place_skin_mesh("model/tiny/tiny.x", {-1.0, 0.0, -10.0}, {0.0, 0.0, 0.0}, 0.003)
end
