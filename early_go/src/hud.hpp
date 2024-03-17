#ifndef HUD_HPP
#define HUD_HPP
#include "stdafx.hpp"
#include "main_window.hpp"

#include <thread>
#include <mutex>

namespace early_go
{

    struct message_writer;
    struct message_writer_for_thread;

    class hud final
    {
    public:
        hud(const std::shared_ptr<IDirect3DDevice9> &);
        void add_image(const std::string &, const std::string &, const cv::Point &);
        void delete_image(const std::string &);
        void add_message(const std::string &, const std::string &, const cv::Rect &);
        void delete_message(const std::string &);
        void add_frame(const std::string &, const cv::Rect &, const cv::Scalar &);
        void delete_frame(const std::string &);
        void add_message_in_frame(const std::string &,
                                  const std::string &, const std::string &);
        void show_HP_info();
        void remove_HP_info();
        void operator()(main_window &);

    private:
        std::shared_ptr<IDirect3DDevice9> d3d_device_;
        std::shared_ptr<ID3DXSprite> sprite_;
        // std::shared_ptr<message_writer>      message_writer_;

        void create_round_rect(LPDIRECT3DTEXTURE9 &,
                               const cv::Point &, const cv::Scalar &);

        struct texture
        {
            const std::string id_;
            std::shared_ptr<IDirect3DTexture9> value_;
            cv::Rect rect_;
        };
        std::list<texture> textures_;

        struct message_info
        {
            decltype(hud::textures_)::iterator texture_;
            std::shared_ptr<message_writer> message_writer_;
        };
        std::list<message_info> message_list_;

        struct frame_animator;
        struct message_frame
        {
            hud &outer_;
            decltype(hud::textures_)::iterator texture_;
            const cv::Point size_dest_;
            const cv::Scalar color_;
            std::shared_ptr<frame_animator> frame_animator_;
        };
        std::list<message_frame> frame_list_;

        struct frame_animator
        {
            frame_animator();
            virtual bool operator()(message_frame &);
            virtual ~frame_animator(){};
            const static float LENGTH;
            int count_;
        };
        struct frame_animator_delete : frame_animator
        {
            frame_animator_delete();
            virtual bool operator()(message_frame &);
        };
        const static int EDGE_CIRCLE_RADIUS;

        struct HP_info_animator;
        struct HP_info
        {
            hud &outer_;
            decltype(hud::textures_)::iterator texture_;
            std::shared_ptr<HP_info_animator> HP_info_animator_;
            const static cv::Size TEXTURE_SIZE;
        };
        struct HP_info_animator2;
        struct HP_info2
        {
            hud &outer_;
            decltype(hud::textures_)::iterator texture_;
            std::shared_ptr<HP_info_animator2> HP_info_animator_;
            const static cv::Size TEXTURE_SIZE;
        };

        class HP_info_drawer;
        struct HP_info_animator
        {
            HP_info_animator();
            void operator()(HP_info &, main_window &);
            //    void rotate(const type&, HP_info&);
            std::shared_ptr<HP_info_drawer> HP_info_drawer_;
        };
        class HP_info_drawer
        {
        public:
            ~HP_info_drawer();
            const static int THREAD_NUM = 4;
            HP_info_drawer();
            void operator()();
            cv::Mat &get_image() { return image_; }
            void set_charge_func_index(const int val) { charge_func_index_ = val; }
            std::mutex &get_charge_func_index_mutex() { return charge_func_index_mtx_; }
            std::mutex &get_idle_mutex() { return idle_mtx_; }
            std::condition_variable &get_idle_condition_variable() { return cond_; }
            bool *get_idles() { return idle_; };
            int get_count() { return count_; }
            void set_count(const int val) { count_ = val; }
            enum class anim_type
            {
                APPEARING,
                APPEARED,
                ROTATING_RIGHT,
                ROTATED_RIGHT,
                ROTATING_LEFT,
                ROTATED_LEFT,
            };
            void set_type(const anim_type val) { type_ = val; }
            int health_;
            int max_health_;

            std::shared_ptr<character> main_chara_;
            std::shared_ptr<character> enemy_;

            std::vector<std::string> normal_move_name_;
            std::pair<int, int> power_;

        private:
            const cv::Scalar BASIC_COLOR{255, 240, 200, 60};
            const DWORD LETTER_COLOR{
                D3DCOLOR_ARGB(static_cast<int>(BASIC_COLOR[3]),
                              static_cast<int>(BASIC_COLOR[2]),
                              static_cast<int>(BASIC_COLOR[1]),
                              static_cast<int>(BASIC_COLOR[0]))};
            const cv::Point CENTER{HP_info::TEXTURE_SIZE.width / 2,
                                   HP_info::TEXTURE_SIZE.height / 2};
            const int LINE_THICKNESS{2};
            const int BEZIER_LENGTH{20};
            const std::vector<cv::Point> BASE_POINT{
                {CENTER},
                {CENTER + cv::Point{80, 80}},
                {CENTER + cv::Point{80, 80} + cv::Point{100, 0}},
                {CENTER + cv::Point{80, 80} + cv::Point{100, 0} + cv::Point{60, -60}},
                {CENTER + cv::Point{80, 80} + cv::Point{100, 0} + cv::Point{60, -60} + cv::Point{150, 0}},
                {CENTER + cv::Point{80, 80} + cv::Point{100, 0} + cv::Point{80, 80}}};

            const int CENTER_CIRCLE_RADIUS_IN{5};
            const int CENTER_CIRCLE_RADIUS_OUT{12};
            const int CENTER_CIRCLE_ANIMATION_TIME{20};
            void draw_center_circle();

            const int LINE_1_ANIMATION_TIME{10};
            const cv::Point LINE_1_DEST{
                BASE_POINT.at(1) - cv::Point{BEZIER_LENGTH, BEZIER_LENGTH}};
            void draw_line_1();

            const std::array<cv::Point, 3> BEZIER_COOD_1{
                BASE_POINT.at(1) - cv::Point{BEZIER_LENGTH, BEZIER_LENGTH},
                BASE_POINT.at(1),
                BASE_POINT.at(1) + cv::Point{BEZIER_LENGTH, 0}};
            void draw_curve_1();

            const cv::Point LINE_2_START{BEZIER_COOD_1.at(2)};
            const cv::Point LINE_2_DEST{BASE_POINT.at(2)};
            const int LINE_2_ANIMATION_START{11};
            const int LINE_2_ANIMATION_END{20};
            const int LINE_2_ANIMATION_LENGTH{
                LINE_2_ANIMATION_END - LINE_2_ANIMATION_START + 1};
            void draw_line_2();

            const cv::Point LINE_3_START{BASE_POINT.at(2)};
            const cv::Point LINE_3_DEST{
                BASE_POINT.at(3) + cv::Point(-BEZIER_LENGTH, BEZIER_LENGTH)};
            const int LINE_3_ANIMATION_START{21};
            const int LINE_3_ANIMATION_END{30};
            const int LINE_3_ANIMATION_LENGTH{
                LINE_3_ANIMATION_END - LINE_3_ANIMATION_START + 1};
            void draw_line_3();

            const std::array<cv::Point, 3> BEZIER_COOD_2{
                BASE_POINT.at(3) + cv::Point{-BEZIER_LENGTH, BEZIER_LENGTH},
                BASE_POINT.at(3),
                BASE_POINT.at(3) + cv::Point{BEZIER_LENGTH, 0}};
            void draw_curve_2();

            const cv::Point LINE_4_START{BEZIER_COOD_2.at(2)};
            const cv::Point LINE_4_DEST{BASE_POINT.at(4)};
            const int LINE_4_ANIMATION_START{31};
            const int LINE_4_ANIMATION_END{40};
            const int LINE_4_ANIMATION_LENGTH{
                LINE_4_ANIMATION_END - LINE_4_ANIMATION_START + 1};
            void draw_line_4();

            const int LINE_5_ANIMATION_START{21};
            const cv::Point LINE_5_START{BASE_POINT.at(2)};
            const cv::Point LINE_5_DEST{BASE_POINT.at(5)};
            const int LINE_5_ANIMATION_END{30};
            const int LINE_5_ANIMATION_LENGTH{
                LINE_5_ANIMATION_END - LINE_5_ANIMATION_START + 1};
            void draw_line_5();

            const int CIRCLE_2_RADIUS_OUT{15};
            const int CIRCLE_2_OUT_ANIMATION_START{11};
            const int CIRCLE_2_OUT_ANIMATION_LENGTH{20};
            const int CIRCLE_2_OUT_ANIMATION_END{
                CIRCLE_2_OUT_ANIMATION_START + CIRCLE_2_OUT_ANIMATION_LENGTH - 1};

            const int CIRCLE_2_RADIUS_IN{CIRCLE_2_RADIUS_OUT - (LINE_THICKNESS * 2 - 1)};
            const int CIRCLE_2_IN_ANIMATION_START{21};
            const int CIRCLE_2_IN_ANIMATION_LENGTH{20};
            const int CIRCLE_2_IN_ANIMATION_END{
                CIRCLE_2_IN_ANIMATION_START + CIRCLE_2_IN_ANIMATION_LENGTH - 1};
            void draw_circle_2();

            const int HP_BAR_RADIUS_IN{50};
            const int HP_BAR_RADIUS_OUT{60};
            const cv::Point HP_BAR_CENTER{BASE_POINT.at(4)};

            const int HP_BAR_BACK_ANIMATION_START{11};
            const int HP_BAR_BACK_ANIMATION_END{40};
            const int HP_BAR_BACK_ANIMATION_LENGTH{
                HP_BAR_BACK_ANIMATION_END - HP_BAR_BACK_ANIMATION_START + 1};
            const cv::Scalar HP_BAR_BACK{BASIC_COLOR[0], BASIC_COLOR[1],
                                         BASIC_COLOR[2], BASIC_COLOR[3] / 2};
            const int HP_BAR_FORE_ANIMATION_START{31};
            const int HP_BAR_FORE_ANIMATION_END{60};
            const int HP_BAR_FORE_ANIMATION_LENGTH{
                HP_BAR_FORE_ANIMATION_END - HP_BAR_FORE_ANIMATION_START + 1};
            const cv::Scalar HP_BAR_FORE{BASIC_COLOR};
            void draw_HP_bar();

            const int TIME_BAR_RADIUS_IN{30};
            const int TIME_BAR_RADIUS_OUT{40};
            const cv::Point TIME_BAR_CENTER{BASE_POINT.at(5)};

            const int TIME_BAR_BACK_ANIMATION_START{LINE_5_ANIMATION_START};
            const int TIME_BAR_BACK_ANIMATION_LENGTH{30};
            const int TIME_BAR_BACK_ANIMATION_END{
                TIME_BAR_BACK_ANIMATION_START + TIME_BAR_BACK_ANIMATION_LENGTH - 1};

            const int TIME_BAR_FORE_ANIMATION_START{TIME_BAR_BACK_ANIMATION_START + 20};
            const int TIME_BAR_FORE_ANIMATION_LENGTH{30};
            const int TIME_BAR_FORE_ANIMATION_END{
                TIME_BAR_FORE_ANIMATION_START + TIME_BAR_FORE_ANIMATION_LENGTH - 1};
            void draw_Time_bar();

            const int LETTER_FADE_DURATION{10};

            void draw_text_stage_number_1(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_stage_number_2(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_time_1(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_time_2(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_strength_1(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_strength_2(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_strength_3(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_strength_4(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_HP_1(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_HP_2(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_HP_3(
                const std::shared_ptr<message_writer_for_thread> &);

            void copy_text(const std::shared_ptr<message_writer_for_thread> &);

            std::shared_ptr<std::thread> drawer_[THREAD_NUM];
            cv::Mat image_{cv::Mat::zeros(
                HP_info::TEXTURE_SIZE.width, HP_info::TEXTURE_SIZE.height, CV_8UC4)};
            std::vector<std::function<void(void)>> funcs_;
            int charge_func_index_{0};
            std::mutex charge_func_index_mtx_;
            std::mutex idle_mtx_;
            std::condition_variable cond_;
            bool idle_[THREAD_NUM]{false};
            bool finish_request_{false};
            int count_;
            anim_type type_{anim_type::APPEARING};
        };
        class HP_info_drawer2;
        struct HP_info_animator2
        {
            HP_info_animator2();
            void operator()(HP_info2 &, main_window &);
            //    void rotate(const type&, HP_info&);
            std::shared_ptr<HP_info_drawer2> HP_info_drawer2_;
        };
        class HP_info_drawer2
        {
        public:
            ~HP_info_drawer2();
            const static int THREAD_NUM = 4;
            HP_info_drawer2();
            void operator()();
            cv::Mat &get_image() { return image_; }
            void set_charge_func_index(const int val) { charge_func_index_ = val; }
            std::mutex &get_charge_func_index_mutex() { return charge_func_index_mtx_; }
            std::mutex &get_idle_mutex() { return idle_mtx_; }
            std::condition_variable &get_idle_condition_variable() { return cond_; }
            bool *get_idles() { return idle_; };
            int get_count() { return count_; }
            void set_count(const int val) { count_ = val; }
            enum class anim_type
            {
                APPEARING,
                APPEARED,
                ROTATING_RIGHT,
                ROTATED_RIGHT,
                ROTATING_LEFT,
                ROTATED_LEFT,
            };
            void set_type(const anim_type val) { type_ = val; }
            int health_;
            int max_health_;

            std::shared_ptr<character> main_chara_;
            std::shared_ptr<character> enemy_;

            std::vector<std::string> normal_move_name_;
            std::pair<int, int> power_;

        private:
            const cv::Scalar BASIC_COLOR{255, 240, 200, 60};
            const DWORD LETTER_COLOR{
                D3DCOLOR_ARGB(static_cast<int>(BASIC_COLOR[3]),
                              static_cast<int>(BASIC_COLOR[2]),
                              static_cast<int>(BASIC_COLOR[1]),
                              static_cast<int>(BASIC_COLOR[0]))};
            const cv::Point CENTER{HP_info::TEXTURE_SIZE.width / 2,
                                   HP_info::TEXTURE_SIZE.height / 2};
            const int LINE_THICKNESS{2};
            const int BEZIER_LENGTH{20};
            const std::vector<cv::Point> BASE_POINT{
                {CENTER},
                {CENTER + cv::Point{80, 80}},
                {CENTER + cv::Point{80, 80} + cv::Point{100, 0}},
                {CENTER + cv::Point{80, 80} + cv::Point{100, 0} + cv::Point{60, -60}},
                {CENTER + cv::Point{80, 80} + cv::Point{100, 0} + cv::Point{60, -60} + cv::Point{150, 0}},
                {CENTER + cv::Point{80, 80} + cv::Point{100, 0} + cv::Point{80, 80}}};

            const int CENTER_CIRCLE_RADIUS_IN{5};
            const int CENTER_CIRCLE_RADIUS_OUT{12};
            const int CENTER_CIRCLE_ANIMATION_TIME{20};
            void draw_center_circle();

            const int LINE_1_ANIMATION_TIME{10};
            const cv::Point LINE_1_DEST{
                BASE_POINT.at(1) - cv::Point{BEZIER_LENGTH, BEZIER_LENGTH}};
            void draw_line_1();

            const std::array<cv::Point, 3> BEZIER_COOD_1{
                BASE_POINT.at(1) - cv::Point{BEZIER_LENGTH, BEZIER_LENGTH},
                BASE_POINT.at(1),
                BASE_POINT.at(1) + cv::Point{BEZIER_LENGTH, 0}};
            void draw_curve_1();

            const cv::Point LINE_2_START{BEZIER_COOD_1.at(2)};
            const cv::Point LINE_2_DEST{BASE_POINT.at(2)};
            const int LINE_2_ANIMATION_START{11};
            const int LINE_2_ANIMATION_END{20};
            const int LINE_2_ANIMATION_LENGTH{
                LINE_2_ANIMATION_END - LINE_2_ANIMATION_START + 1};
            void draw_line_2();

            const cv::Point LINE_3_START{BASE_POINT.at(2)};
            const cv::Point LINE_3_DEST{
                BASE_POINT.at(3) + cv::Point(-BEZIER_LENGTH, BEZIER_LENGTH)};
            const int LINE_3_ANIMATION_START{21};
            const int LINE_3_ANIMATION_END{30};
            const int LINE_3_ANIMATION_LENGTH{
                LINE_3_ANIMATION_END - LINE_3_ANIMATION_START + 1};
            void draw_line_3();

            const std::array<cv::Point, 3> BEZIER_COOD_2{
                BASE_POINT.at(3) + cv::Point{-BEZIER_LENGTH, BEZIER_LENGTH},
                BASE_POINT.at(3),
                BASE_POINT.at(3) + cv::Point{BEZIER_LENGTH, 0}};
            void draw_curve_2();

            const cv::Point LINE_4_START{BEZIER_COOD_2.at(2)};
            const cv::Point LINE_4_DEST{BASE_POINT.at(4)};
            const int LINE_4_ANIMATION_START{31};
            const int LINE_4_ANIMATION_END{40};
            const int LINE_4_ANIMATION_LENGTH{
                LINE_4_ANIMATION_END - LINE_4_ANIMATION_START + 1};
            void draw_line_4();

            const int LINE_5_ANIMATION_START{21};
            const cv::Point LINE_5_START{BASE_POINT.at(2)};
            const cv::Point LINE_5_DEST{BASE_POINT.at(5)};
            const int LINE_5_ANIMATION_END{30};
            const int LINE_5_ANIMATION_LENGTH{
                LINE_5_ANIMATION_END - LINE_5_ANIMATION_START + 1};
            void draw_line_5();

            const int CIRCLE_2_RADIUS_OUT{15};
            const int CIRCLE_2_OUT_ANIMATION_START{11};
            const int CIRCLE_2_OUT_ANIMATION_LENGTH{20};
            const int CIRCLE_2_OUT_ANIMATION_END{
                CIRCLE_2_OUT_ANIMATION_START + CIRCLE_2_OUT_ANIMATION_LENGTH - 1};

            const int CIRCLE_2_RADIUS_IN{CIRCLE_2_RADIUS_OUT - (LINE_THICKNESS * 2 - 1)};
            const int CIRCLE_2_IN_ANIMATION_START{21};
            const int CIRCLE_2_IN_ANIMATION_LENGTH{20};
            const int CIRCLE_2_IN_ANIMATION_END{
                CIRCLE_2_IN_ANIMATION_START + CIRCLE_2_IN_ANIMATION_LENGTH - 1};
            void draw_circle_2();

            const int HP_BAR_RADIUS_IN{50};
            const int HP_BAR_RADIUS_OUT{60};
            const cv::Point HP_BAR_CENTER{BASE_POINT.at(4)};

            const int HP_BAR_BACK_ANIMATION_START{11};
            const int HP_BAR_BACK_ANIMATION_END{40};
            const int HP_BAR_BACK_ANIMATION_LENGTH{
                HP_BAR_BACK_ANIMATION_END - HP_BAR_BACK_ANIMATION_START + 1};
            const cv::Scalar HP_BAR_BACK{BASIC_COLOR[0], BASIC_COLOR[1],
                                         BASIC_COLOR[2], BASIC_COLOR[3] / 2};
            const int HP_BAR_FORE_ANIMATION_START{31};
            const int HP_BAR_FORE_ANIMATION_END{60};
            const int HP_BAR_FORE_ANIMATION_LENGTH{
                HP_BAR_FORE_ANIMATION_END - HP_BAR_FORE_ANIMATION_START + 1};
            const cv::Scalar HP_BAR_FORE{BASIC_COLOR};
            void draw_HP_bar();

            const int TIME_BAR_RADIUS_IN{30};
            const int TIME_BAR_RADIUS_OUT{40};
            const cv::Point TIME_BAR_CENTER{BASE_POINT.at(5)};

            const int TIME_BAR_BACK_ANIMATION_START{LINE_5_ANIMATION_START};
            const int TIME_BAR_BACK_ANIMATION_LENGTH{30};
            const int TIME_BAR_BACK_ANIMATION_END{
                TIME_BAR_BACK_ANIMATION_START + TIME_BAR_BACK_ANIMATION_LENGTH - 1};

            const int TIME_BAR_FORE_ANIMATION_START{TIME_BAR_BACK_ANIMATION_START + 20};
            const int TIME_BAR_FORE_ANIMATION_LENGTH{30};
            const int TIME_BAR_FORE_ANIMATION_END{
                TIME_BAR_FORE_ANIMATION_START + TIME_BAR_FORE_ANIMATION_LENGTH - 1};
            void draw_Time_bar();

            const int LETTER_FADE_DURATION{10};

            void draw_text_stage_number_1(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_stage_number_2(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_time_1(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_time_2(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_strength_1(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_strength_2(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_strength_3(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_strength_4(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_HP_1(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_HP_2(
                const std::shared_ptr<message_writer_for_thread> &);
            void draw_text_HP_3(
                const std::shared_ptr<message_writer_for_thread> &);

            void copy_text(const std::shared_ptr<message_writer_for_thread> &);

            std::shared_ptr<std::thread> drawer_[THREAD_NUM];
            cv::Mat image_{cv::Mat::zeros(
                HP_info2::TEXTURE_SIZE.width, HP_info2::TEXTURE_SIZE.height, CV_8UC4)};
            std::vector<std::function<void(void)>> funcs_;
            int charge_func_index_{0};
            std::mutex charge_func_index_mtx_;
            std::mutex idle_mtx_;
            std::condition_variable cond_;
            bool idle_[THREAD_NUM]{false};
            bool finish_request_{false};
            int count_;
            anim_type type_{anim_type::APPEARING};
        };
        std::shared_ptr<HP_info> HP_info_;
        std::shared_ptr<HP_info2> HP_info2_;
    };
} /* namespace early_go */
#endif
