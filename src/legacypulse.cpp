#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

float legacyFPS = Mod::get()->getSettingValue<int64_t>("pulse-fps");
bool legacyPulseEnabled = Mod::get()->getSettingValue<bool>("legacy-pulse");

$on_mod(Loaded){
	listenForSettingChanges("pulse-fps", [](int value) {
        legacyFPS = value;
    });
    listenForSettingChanges("legacy-pulse", [](bool value) {
        legacyPulseEnabled = value;
    });
}


void updarePulseOldLogic(FMODAudioEngine* self) {
    self->m_system->update();

    if (self->m_metering != false) {
        FMOD_DSP_METERING_INFO meteringInfo = {};
        self->m_mainDSP->getMeteringInfo(0x0, &meteringInfo);

        if (meteringInfo.numchannels == 2) {
            float baseVolume = self->m_musicVolume;
            float leftChannelLevel = meteringInfo.peaklevel[0];
            float rightChannelLevel = meteringInfo.peaklevel[1];

            if (baseVolume > 0.0f) {
                leftChannelLevel /= baseVolume;
                rightChannelLevel /= baseVolume;
            }

            float averageLevel = (leftChannelLevel + rightChannelLevel) * 0.5f + 0.1f;
            self->m_pulse1 = averageLevel;

            int* pulseCounter = &self->m_pulseCounter;
            float* previousAverageLevel = &self->m_pulse3;
            float* previousPulseLevel = &self->m_pulse2;

            if ((*pulseCounter < 3) ||
                (averageLevel < *previousPulseLevel * 1.1f) ||
                (averageLevel < *previousAverageLevel * 0.95f && *previousAverageLevel * 0.2f < *previousPulseLevel)) {

                self->m_pulse1 = *previousPulseLevel * 0.93f;
            }
            else {
                *previousAverageLevel = averageLevel;
                *pulseCounter = 0;
                self->m_pulse1 = averageLevel * 1.1f;
            }

            if (self->m_pulse1 <= 0.1f) {
                *previousAverageLevel = 0.0f;
            }

            (*pulseCounter)++;
            *previousPulseLevel = self->m_pulse1;
        }
    }
}

class $modify(LegacyPulseFMODHook, FMODAudioEngine) {

    void update(float deltaTime) {
        static float accumulatedTime = 0.0f;

        FMODAudioEngine::update(deltaTime);

        bool forceFPS = legacyPulseEnabled;

        if (forceFPS) {
            float fixedTimestep = 1.0f / legacyFPS;

            accumulatedTime += deltaTime;
            while (accumulatedTime >= fixedTimestep) {
                updarePulseOldLogic(this);
                accumulatedTime -= fixedTimestep;
            }
        }
        else {
            updarePulseOldLogic(this);
            accumulatedTime = 0.0f;
        }
    }
};