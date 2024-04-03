function step()
    draw_background("image/back_ground.png")
    fade_in()

    draw_portrait("image/early/0.png", "center")

    draw_message_window()
    
    coroutine.yield()

    draw_text([[aiueoあああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああ]])

    coroutine.yield()

    draw_portrait("image/early/0.png", "right")

    draw_text([[いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい]])

    coroutine.yield()

    draw_portrait("image/early/0.png", "left")

    coroutine.yield()

    draw_portrait_flip("image/early/0.png", "left")

    coroutine.yield()

    draw_portrait_flip("image/early/0.png", "right")

    coroutine.yield()

    draw_portrait_flip("image/early/0.png", "center")

    coroutine.yield()

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
        でも……
        あきらめるの
        ょくなぃって……
        ]])
    coroutine.yield()
        
    draw_text([[
        メロスゎ……
        ぉもって……
        がんばった……
        ]])
    coroutine.yield()
        
    draw_text([[
        でも……
        ネイル…われて……
        ]])
    coroutine.yield()
    
    draw_text([[
        イタイょ……ゴメン……
        まにあわなかった……
        ]])
    coroutine.yield()
        
    draw_text([[
        でも……メロスとセリヌンゎ……ズッ友だょ……！！ 
]])
    coroutine.yield()

    draw_text([[あいうえおあいうえおあいうえお]])

end
