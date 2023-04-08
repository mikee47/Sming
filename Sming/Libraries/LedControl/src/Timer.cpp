#include "include/LEDC/Timer.h"

namespace LEDC
{
std::bitset<LEDC_TIMER_MAX> Timer::usedTimers[LEDC_SPEED_MODE_MAX];
std::bitset<SOC_LEDC_CHANNEL_NUM> Timer::usedChannels[LEDC_SPEED_MODE_MAX];
} // namespace LEDC
