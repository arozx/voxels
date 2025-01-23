#pragma once
#include "../external/sol2/sol.hpp"
#include "Core/AssetManager.h"

namespace Engine {
    class LuaScriptSystem {
    public:
        LuaScriptSystem();
        ~LuaScriptSystem();

        void Initialize();
        void RegisterEngineAPI();
        bool ExecuteScript(const std::string& script);
        bool ExecuteFile(const std::string& filepath);

    private:
        std::unique_ptr<sol::state> m_LuaState;
    };
}