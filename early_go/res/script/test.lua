function step()
    draw_background("image/back_ground.png")
    fade_in()

    draw_portrait("image/early/0.png", "center")

    draw_message_window()
    
    coroutine.yield()

    draw_text([[aiueoあああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああ]])

    coroutine.yield()

    draw_portrait("image/early/0.png", "right")

    coroutine.yield()

    draw_portrait("image/early/0.png", "left")

    coroutine.yield()

    draw_portrait("image/early/0r.png", "left")

    draw_portrait("image/early/0r.png", "right")

    remove_portrait("center")

    coroutine.yield()

    draw_text([[
        メロスゎ走った……
        セリヌンティウスが
        まってる……
        ]])
    coroutine.yield()
        
    draw_text([[
        でも……
        もぅつかれちゃった…
        ]])
    coroutine.yield()
        
    draw_text([[
        でも……メロスとセリヌンゎ……ズッ友だょ……！！ 
    ]])
    coroutine.yield()

end
