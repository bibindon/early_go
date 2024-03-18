function step()
    fade_out()

    coroutine.yield()

    fade_in()
    draw_background("image/back_ground.png")
    draw_portrait("image/early/0.png", "center")

    coroutine.yield()

    draw_portrait("image/early/0.png", "right")

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

    draw_message_window()

    coroutine.yield()

    draw_text([[aiueoあああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああ]])

    coroutine.yield()

    draw_text([[
それらは今日けっしてその所有方という事のためを書いありある。
毫もほかが享有屋ももうこの批評たなけれなどをしがみないがは発展しよないならから、
たったには評したいたうまし。
]])
    coroutine.yield()

    draw_text([[
顔がするです事はいやしくも毎日をとうていますないた。はなはだ嘉納さんを楽自分なぜ講演をあるた弟その気分あなたか関係からというご出立たないですますが、その結果もおれか教場右を読んて、ネルソンさんののを鈍痛の私をもちろんご講演とあるて何片仮名がお観察をできように勢いお妨害にありんべきて、たといよく勉
]])

coroutine.yield()

draw_text([[あいうえおあいうえおあいうえお]])

end
