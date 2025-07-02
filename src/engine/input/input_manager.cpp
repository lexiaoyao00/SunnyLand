#include "input_manager.h"
#include "../core/config.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

namespace engine::input {
InputManager::InputManager(SDL_Renderer *sdl_renderer, const engine::core::Config *config)
{
    if (!sdl_renderer) {
        spdlog::error("InputManager: SDL_Renderer is null");
        throw std::runtime_error("InputManager: SDL_Renderer is null");
    }
    initializeMappings(config);

    float x,y;
    SDL_GetMouseState(&x, &y);
    mouse_position_ = {x,y};
    spdlog::trace("InputManager: Initialized mouse position: ({},{})", mouse_position_.x, mouse_position_.y);
}

void InputManager::update()
{
    for (auto &[action, state] : action_states_) {
        if (state == ActionState::PRESSED_THIS_FRAME) {
            state = ActionState::HELD_DOWN;
        } else if (state == ActionState::RELEASED_THIS_FRAME) {
            state = ActionState::INACTIVE;
        }
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        processEvent(event);
    }
}

bool InputManager::isActionDown(const std::string &action_name) const
{
    if (auto it = action_states_.find(action_name); it != action_states_.end()) {
        return it->second == ActionState::PRESSED_THIS_FRAME || it->second == ActionState::HELD_DOWN;
    }
    return false;
}

bool InputManager::isActionPressed(const std::string &action_name) const
{
    if (auto it = action_states_.find(action_name); it != action_states_.end()) {
        return it->second == ActionState::PRESSED_THIS_FRAME;
    }
    return false;
}

bool InputManager::isActionReleased(const std::string &action_name) const
{
    if (auto it = action_states_.find(action_name); it != action_states_.end()) {
        return it->second == ActionState::RELEASED_THIS_FRAME;
    }
    return false;
}

bool InputManager::shouldQuit() const
{
    return should_quit_;
}

void InputManager::setShouldQuit(bool should_quit)
{
    should_quit_ = should_quit;
}

glm::vec2 InputManager::getMousePosition() const
{
    return mouse_position_;
}

glm::vec2 InputManager::getLogicalMousePosition() const
{
    glm::vec2 logicl_pos;
    // 通过窗口坐标获取渲染坐标(逻辑坐标)
    SDL_RenderCoordinatesFromWindow(sdl_renderer_,mouse_position_.x, mouse_position_.y, &logicl_pos.x, &logicl_pos.y);
    return logicl_pos;
}

void InputManager::processEvent(const SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: {
        SDL_Scancode scancode = event.key.scancode;
        bool is_down = event.key.down;
        bool is_repeat = event.key.repeat;

        auto it = input_to_actions_map_.find(scancode);
        if (it != input_to_actions_map_.end()) {
            const std::vector<std::string>& associated_actions = it->second;
            for (const auto &action : associated_actions) {
                updateActionStates(action, is_down, is_repeat);
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        Uint32 mouse_button = event.button.button;
        bool is_mouse_down = event.button.down;
        auto it = input_to_actions_map_.find(mouse_button);
        if (it != input_to_actions_map_.end()) {
            const std::vector<std::string>& associated_actions = it->second;
            for (const auto &action : associated_actions) {
                updateActionStates(action, is_mouse_down, false);
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_MOTION:
        mouse_position_ = {event.motion.x, event.motion.y};
        break;
    case SDL_EVENT_QUIT:
        should_quit_ = true;
        break;

    default:
        break;
    }
}

void InputManager::initializeMappings(const engine::core::Config *config)
{
    spdlog::trace("InputManager: Initializing mappings...");
    if (!config) {
        spdlog::error("InputManager: Config is null");
        throw std::runtime_error("InputManager: Config is null");
    }
    actions_to_keyname_map_ = config->input_mappings_;
    input_to_actions_map_.clear();
    action_states_.clear();

    // 如果配置中没有定义鼠标按钮动作，则添加默认映射， 用于 UI 事件
    if (actions_to_keyname_map_.find("MouseLeftClick") == actions_to_keyname_map_.end()) {
        spdlog::debug("InputManager: MouseLeftClick not found in config, adding default mapping to 'MouseLeftClick'.");
        actions_to_keyname_map_["MouseLeftClick"] = {"MouseLeft"};
    }
    if (actions_to_keyname_map_.find("MouseRightClick") == actions_to_keyname_map_.end()) {
        spdlog::debug("InputManager: MouseRightClick not found in config, adding default mapping to 'MouseRightClick'.");
        actions_to_keyname_map_["MouseRightClick"] = {"MouseRight"};
    }

    for (const auto &[action, keynames] : actions_to_keyname_map_) {
        action_states_[action] = ActionState::INACTIVE;
        spdlog::trace("InputManager: Added action: {}", action);
        for (const auto &keyname : keynames) {
            SDL_Scancode scancode = scancodeFromString(keyname);
            Uint32 mouse_button = mouseButtonUint32FromString(keyname);
            // more ...

            if (scancode != SDL_SCANCODE_UNKNOWN) {
                input_to_actions_map_[scancode].push_back(action);
                spdlog::trace("InputManager: Added scancode mapping: {} -> {}", keyname, static_cast<int>(scancode));
            } else if (mouse_button != 0) {
                input_to_actions_map_[mouse_button].push_back(action);
                spdlog::trace("InputManager: Added mouse button mapping: {} -> {}", keyname, static_cast<int>(mouse_button));
            } else {
                spdlog::warn("InputManager: Unknown keyname: {} used for action: {}", keyname, action);
            }
        }
    }
}

void InputManager::updateActionStates(const std::string action_name, bool is_input_active, bool is_repeat_event)
{
    auto it = action_states_.find(action_name);
    if (it == action_states_.end()) {
        spdlog::warn("InputManager: Try to update Action {} which is not registered", action_name);
        return;
    }

    if (is_input_active) {
        if (is_repeat_event) {
            it->second = ActionState::HELD_DOWN;
        } else {
            it->second = ActionState::PRESSED_THIS_FRAME;
        }
    } else {
        it->second = ActionState::RELEASED_THIS_FRAME;
    }
}

SDL_Scancode InputManager::scancodeFromString(const std::string &key_name) const
{
    return SDL_GetScancodeFromName(key_name.c_str());
}

Uint32 InputManager::mouseButtonUint32FromString(const std::string &button_name) const
{
    if (button_name == "MouseLeft") return SDL_BUTTON_LEFT;
    if (button_name == "MouseMiddle") return SDL_BUTTON_MIDDLE;
    if (button_name == "MouseRight") return SDL_BUTTON_RIGHT;
    if (button_name == "MouseX1") return SDL_BUTTON_X1;
    if (button_name == "MouseX2") return SDL_BUTTON_X2;
    return 0;

}

} // namespace engine::input