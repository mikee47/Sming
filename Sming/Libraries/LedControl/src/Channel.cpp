#include "include/LEDC/Channel.h"

namespace LEDC
{
bool Channel::configure(const Config& config)
{
	ledc_channel_config_t cfg{
		.gpio_num = config.gpio,
		.speed_mode = timer.mode,
		.channel = id,
		//.intr_type = ?,
		.timer_sel = timer.id,
		.duty = config.duty,
		.hpoint = config.hpoint,
		.flags =
			{
				.output_invert = config.invert,
			},
	};

	esp_err_t err = ledc_channel_config(&cfg);
	if(err == ESP_OK) {
		err = ledc_bind_channel_timer(timer.mode, id, timer.id);
	}

	return err == ESP_OK;
}

} // namespace LEDC
