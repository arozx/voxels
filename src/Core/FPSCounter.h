#pragma once
#include <pch.h>

/**
 * @namespace Engine
 * @brief Core engine namespace containing fundamental engine components
 */

/**
 * @class FPSCounter
 * @brief A class for measuring and analyzing frame rate performance metrics
 * 
 * The FPSCounter class tracks frame rates and provides various performance metrics
 * including current FPS, average FPS, 1% low/high values, and frame time.
 * It maintains a rolling buffer of frame time samples for statistical analysis.
 * 
 * @note The class maintains SAMPLE_COUNT (200) most recent samples for calculations
 * 
 * @see SAMPLE_COUNT
 */
namespace Engine {
    class FPSCounter {
    public:
        void Update(float deltaTime);
        float GetCurrentFPS() const { return m_CurrentFPS; }
        float GetAverageFPS() const { return m_AverageFPS; }
        float GetOnePercentLow() const { return m_OnePercentLow; }
        float GetOnePercentHigh() const { return m_OnePercentHigh; }
        float GetFrameTime() const { return m_FrameTime; }

        // Add new methods
        const std::vector<float>& GetFrameTimeHistory() const { return m_FrameTimeHistory; }
        size_t GetHistorySize() const { return HISTORY_SIZE; }

    private:
        static constexpr size_t SAMPLE_COUNT = 200;
        static constexpr size_t HISTORY_SIZE = 100; // Store last 100 frame times for flame graph
        std::vector<float> m_FrameTimeHistory;
        std::vector<float> m_Samples;
        float m_CurrentFPS = 0.0f;
        float m_AverageFPS = 0.0f;
        float m_OnePercentLow = 0.0f;
        float m_OnePercentHigh = 0.0f;
        float m_FrameTime = 0.0f;
        float m_UpdateTimer = 0.0f;
        size_t m_CurrentSample = 0;
    };
}