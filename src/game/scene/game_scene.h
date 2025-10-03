#pragma once
#include "../../engine/scene/scene.h"
#include <glm/fwd.hpp>

namespace engine::object {
    class GameObject;
}

namespace game::data {
    class SessionData;
}


namespace game::scene {
    /**
     * @brief 主要的游戏场景，包含玩家、敌人、关卡元素
     *
     */
    class GameScene final : public engine::scene::Scene {
        std::shared_ptr<game::data::SessionData> game_session_data_; // 场景共享数据，用 shared_ptr 管理
        engine::object::GameObject* player_ = nullptr; // 测试对象
    public:
        GameScene(engine::core::Context& context,
                engine::scene::SceneManager& scene_manager,
                std::shared_ptr<game::data::SessionData> data = nullptr);

        void init() override;
        void update(float delta_time) override;
        void render() override;
        void handleInput() override;
        void clean() override;

    private:
        [[nodiscard]] bool initLevel();         // 初始化关卡
        [[nodiscard]] bool initPlayer();        // 初始化玩家
        [[nodiscard]] bool initEnemyAndItem(); // 初始化敌人与道具
        [[nodiscard]] bool initUI();          // 初始化UI


        void handleObjectCollisions(); // 处理对象碰撞
        void handleTileTriggers();   // 处理瓦片触发事件
        void handlePlayerDamage(int damage); // 处理玩家受伤（更新得分、UI 等）
        void PlayerVSEnemyCollision(engine::object::GameObject* player, engine::object::GameObject* enemy);
        void PlayerVSItemCollision(engine::object::GameObject* player, engine::object::GameObject* item);

        void toNextLevel(engine::object::GameObject* trigger); // 进入下一关

        std::string levelNameToPath(const std::string& level_name) const {return "assets/maps/" + level_name + ".tmj";}

        /**
         * @brief 创建一次性特效
         *
         * @param center_pos 特效中心位置
         * @param tag 特效标签（决定特效类型）
         */
        void createEffect(const glm::vec2& center_pos, const std::string& tag);

        // 测试函数
        // void testSaveAndLoad();
        void testTextRenderer();

    };


}   // namespace game::scene
