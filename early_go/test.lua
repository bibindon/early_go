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

    draw_text([[aiueo������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������]])

    coroutine.yield()

    draw_text([[
�����͍����������Ă��̏��L���Ƃ������̂��߂��������肠��B
�|���ق������L�����������̔�]���Ȃ���Ȃǂ������݂Ȃ����͔��W����Ȃ��Ȃ炩��A
�������ɂ͕]�����������܂��B
]])
    coroutine.yield()

    draw_text([[
�炪����ł����͂��₵�����������Ƃ��Ă��܂��Ȃ����B�͂Ȃ͂��Ô[������y�����Ȃ��u�������邽�킻�̋C�����Ȃ����֌W����Ƃ������o�����Ȃ��ł��܂����A���̌��ʂ����ꂩ����E��ǂ�āA�l���\������̂̂�ݒɂ̎���������񂲍u���Ƃ���ĉ��Љ��������ώ@���ł��悤�ɐ������W�Q�ɂ����ׂ��āA���Ƃ��悭��
]])

coroutine.yield()

draw_text([[������������������������������]])

end
