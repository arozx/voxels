#include "FPSCounter.h"

/**
 * @brief Updates the FPS counter with new frame time data
 * 
 * This method updates the FPS counter by:
 * - Calculating current FPS from delta time
 * - Maintaining a rolling window of FPS samples
 * - Every 0.5 seconds:
 *   - Calculates average FPS across all samples
 *   - Calculates 1% low FPS (average of lowest 1% of samples) 
 *   - Calculates 1% high FPS (average of highest 1% of samples)
 *
 * @param deltaTime Time elapsed since last frame in seconds
 */

namespace Engine {
    void FPSCounter::Update(float deltaTime) {
        m_FrameTime = deltaTime;
        m_CurrentFPS = 1.0f / m_FrameTime;
        
        if (m_Samples.size() != SAMPLE_COUNT) {
            m_Samples.resize(SAMPLE_COUNT, 0.0f);
        }
        
        m_Samples[m_CurrentSample] = m_CurrentFPS;
        m_CurrentSample = (m_CurrentSample + 1) % SAMPLE_COUNT;

        m_UpdateTimer += deltaTime;
        if (m_UpdateTimer >= 0.5f) {
            // Calculate average FPS
            float sum = 0.0f;
            for (float fps : m_Samples) {
                sum += fps;
            }
            m_AverageFPS = sum / SAMPLE_COUNT;

            // Calculate 1% highs and lows
            std::vector<float> sortedFPS = m_Samples;
            std::sort(sortedFPS.begin(), sortedFPS.end());
            
            size_t onePercent = SAMPLE_COUNT / 100;
            if (onePercent < 1) onePercent = 1;

            // Calculate 1% lows
            float lowSum = 0.0f;
            for (size_t i = 0; i < onePercent; i++) {
                lowSum += sortedFPS[i];
            }
            m_OnePercentLow = lowSum / onePercent;

            // Calculate 1% highs
            float highSum = 0.0f;
            for (size_t i = 0; i < onePercent; i++) {
                highSum += sortedFPS[SAMPLE_COUNT - 1 - i];
            }
            m_OnePercentHigh = highSum / onePercent;

            m_UpdateTimer = 0.0f;
        }
    }
}