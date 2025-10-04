#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>      // c++17
#include <SDL3/SDL_render.h>
#include <glm/vec2.hpp>

namespace engine::core
{
    class Config;
}
namespace engine::input
{

    enum class ActionState
    {
        INACTIVE,           // 动作未激活
        PRESSED_THIS_FRAME, // 动作在此帧刚刚被按下
        HELD_DOWN,          // 动作被持续按下
        RELEASED_THIS_FRAME // 动作在此帧刚刚被释放
    };

    class InputManager final
    {
    private:
        SDL_Renderer *sdl_renderer_;                                                         // 用于获取逻辑坐标的 SDL_Renderer 指针
        std::unordered_map<std::string, std::vector<std::string>> actions_to_keyname_map_;    // 动作到键名的映射
        std::unordered_map<std::variant<SDL_Scancode,Uint32>, std::vector<std::string>> input_to_actions_map_; // 从输入到动作的映射

        std::unordered_map<std::string, ActionState> action_states_; // 存储每个动作的当前状态

        bool should_quit_ = false; // 退出标志
        glm::vec2 mouse_position_; // 鼠标位置(针对窗口坐标)

    public:
        InputManager(SDL_Renderer *sdl_renderer, const engine::core::Config *config);

        void update(); // 更新输入状态

        bool isActionDown(const std::string &action_name) const;     // 检查动作是否触发(持续按住或者刚刚按下)
        bool isActionPressed(const std::string &action_name) const;  // 检查动作是否在此帧被按下
        bool isActionReleased(const std::string &action_name) const; // 检查动作是否在此帧被释放

        bool shouldQuit() const;              // 查询退出状态
        void setShouldQuit(bool should_quit); // 设置退出状态

        glm::vec2 getMousePosition() const;        // 获取鼠标位置(屏幕坐标)
        glm::vec2 getLogicalMousePosition() const; // 获取鼠标位置(逻辑坐标)

    private:
        void processEvent(const SDL_Event &event);                                                          // 处理 SDL 事件(将按键转换为动作)
        void initializeMappings(const engine::core::Config *config);                                        // 初始化动作到键名的映射
        void updateActionStates(const std::string action_name, bool is_input_active, bool is_repeat_event); // 更新动作状态
        SDL_Scancode scancodeFromString(const std::string &key_name) const;                                 // 将键名转换为 SDL_Scancode
        Uint32 mouseButtonUint32FromString(const std::string &button_name) const;                             // 将鼠标按钮名转换为 SDL_Button
    };

} // namespace engine::input