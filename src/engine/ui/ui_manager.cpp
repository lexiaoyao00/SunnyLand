#include "ui_manager.h"

#include "ui_panel.h"
#include <spdlog/spdlog.h>

namespace engine::ui {
    UIManager::UIManager()
    {
        root_element_ = std::make_unique<UIPanel>(glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
        spdlog::trace("UI 管理器构造完成");
    }

    UIManager::~UIManager() = default;

    bool UIManager::init(const glm::vec2 &window_size)
    {
        root_element_->setSize(window_size);
        spdlog::trace("UI 管理器初始化根面板完成");
        return true;
    }

    void UIManager::addElement(std::unique_ptr<UIElement> element)
    {
        if (root_element_) {
            root_element_->addChild(std::move(element));
        } else {
            spdlog::error("无法添加元素，root_element_ 为空");
        }
    }

    UIPanel *UIManager::getRootElement() const
    {
        return root_element_.get();
    }

    void UIManager::clearElements()
    {
        if (root_element_) {
            root_element_->removeAllChildren();
            spdlog::trace("UI 管理器清空所有元素完成");
        }
    }

    bool UIManager::handleInput(engine::core::Context &context)
    {
        if (root_element_ && root_element_->isVisible()) {
            return root_element_->handleInput(context);
        }

        return false;
    }

    void UIManager::update(float delta_time, engine::core::Context &context)
    {
        if (root_element_ && root_element_->isVisible()) {
            root_element_->update(delta_time,context);
        }
    }

    void UIManager::render(engine::core::Context &context)
    {
        if (root_element_ && root_element_->isVisible()) {
            root_element_->render(context);
        }
    }

}   // namespace engine::ui
