function init()
    cube_pos_y = -0.75
    cube_scale = 0.75
    stage_grid = 1.5
    stage_distance = 10

    -- Stage
    --[[
    for k = 0, 10 do
        for j = 0+(stage_distance*k), 8+(stage_distance*k) do
            for i = -4, 4 do
                 place_shadow_mesh("model/cube2/cube2.x", {stage_grid * i, cube_pos_y, stage_grid * j}, {0.0, 0.0, 0.0}, cube_scale)
            end
        end
    end

    ]]
    
    --place_shadow_mesh("model/grass/grass.x", {0, -20, 0}, {0.0, 0.0, 0.0}, 15.0)

    place_shadow_mesh("model/cube2/cube2.x", {-20, -5, 0}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {-10, -5, 0}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {0, -5, 0}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {10, -5, 0}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {20, -5, 0}, {0.0, 0.0, 0.0}, 5.0)

    place_shadow_mesh("model/cube2/cube2.x", {-20, -5, 10}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {-10, -5, 10}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {0, -5, 10}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {10, -5, 10}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {20, -5, 10}, {0.0, 0.0, 0.0}, 5.0)

    place_shadow_mesh("model/cube2/cube2.x", {-20, -5, 20}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {-10, -5, 20}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {0, -5, 20}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {10, -5, 20}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {20, -5, 20}, {0.0, 0.0, 0.0}, 5.0)

    place_shadow_mesh("model/cube2/cube2.x", {-20, -5, 30}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {-10, -5, 30}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {0, -5, 30}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {10, -5, 30}, {0.0, 0.0, 0.0}, 5.0)
    place_shadow_mesh("model/cube2/cube2.x", {20, -5, 30}, {0.0, 0.0, 0.0}, 5.0)


    place_shadow_mesh("model/tiger/tiger.x", {-2.0, 0.0, 10.0}, {0.0, 0.0, 0.0}, 1.2)
    place_shadow_mesh("model/tiger/tiger.x", {-2.5, 1.0, 3.0}, {0.0, 0.0, 0.0}, 1.2)
    place_shadow_mesh("model/Dwarf/Dwarf.x", {6.5, 2.0, 3.0}, {0.0, 0.0, 0.0}, 1.0)
    place_shadow_mesh("model/Dwarf/Dwarf.x", {10, 0.0, 10.0}, {0.0, 0.0, 0.0}, 2.0)
    place_shadow_mesh("model/cube2/cube2.x", {8, 0, 8}, {0.0, 0.0, 0.0}, 3.0)
 --   place_shadow_skin_mesh("model/wolf/wolf.x", {1.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, 1.0)
 --   place_shadow_skin_mesh("model/tiny/tiny.x", {1.0, 0.0, 3.0}, {0.0, 0.0, 0.0}, 0.003)
    --[[
    place_shadow_mesh("model/tiger/tiger.x", {3.5, 2.0, -10.0}, {0.0, 0.0, 0.0}, 1.2)
    place_shadow_mesh("model/tiger/tiger.x", {5.5, 2.0, -10.0}, {0.0, 0.0, 0.0}, 1.0)
    place_shadow_mesh("model/Dwarf/Dwarf.x", {4.5, 2.0, -10.0}, {0.0, 0.0, 0.0}, 1.0)
    
    place_anim_mesh("model/RobotArm/RobotArm.x", {0.0, 0.0, -10.0}, {0.0, 0.0, 0.0}, 1.0)
    place_skin_mesh("model/wolf/wolf.x", {1.0, 0.0, -10.0}, {0.0, 0.0, 0.0}, 1.0)
    place_skin_mesh("model/tiny/tiny.x", {-1.0, 0.0, -10.0}, {0.0, 0.0, 0.0}, 0.003)
    ]]--
end
