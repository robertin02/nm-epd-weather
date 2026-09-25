/* NOTE4C PCB1.0: GPIO4 ADC1_CH3, 1:2 divider; CHG GPIO2 low, FULL GPIO1 high.
 * Hardware source and voltage-only estimate: docs/HARDWARE.md.
 * No charger control, power-off, persistence, or runtime prediction. */
#include "home_battery.h"
#include "home_runtime.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/task.h"

void home_battery_task(void *unused)
{   
    (void)unused;
    adc_oneshot_unit_handle_t adc = NULL;
    adc_cali_handle_t calibration = NULL;
    adc_oneshot_unit_init_cfg_t unit = {.unit_id = ADC_UNIT_1};
    adc_oneshot_chan_cfg_t channel = {.atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_12};
    adc_cali_curve_fitting_config_t cal = {
        .unit_id = ADC_UNIT_1, .chan = ADC_CHANNEL_2,
        .atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_12};
    if (adc_oneshot_new_unit(&unit, &adc) != ESP_OK ||
        adc_oneshot_config_channel(adc, ADC_CHANNEL_2, &channel) != ESP_OK ||
        adc_cali_create_scheme_curve_fitting(&cal, &calibration) != ESP_OK) {
        if (adc) adc_oneshot_del_unit(adc);
        ESP_LOGW("home_battery", "Calibrated battery reading unavailable");
        vTaskDelete(NULL);
        return;
    }

    gpio_set_direction(GPIO_NUM_43, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_43, 1);

    int64_t next_voltage = 0;
    home_battery_t sample = {.percent_estimate = -1};
    while (true) {
        sample.charge_valid = true;
        sample.charging = false;
        sample.full = false;
        int64_t now = esp_timer_get_time();
        if (now >= next_voltage) {
            int sum = 0, count = 0;
            for (int i = 0; i < 10; i++) {
                int raw, mv;
                if (adc_oneshot_read(adc, ADC_CHANNEL_2, &raw) == ESP_OK &&
                    adc_cali_raw_to_voltage(calibration, raw, &mv) == ESP_OK) {
                    sum += mv * 2;
                    count++;
                }
            }
            sample.millivolts = count == 10 ? sum / 10 : 0;
            sample.valid = count == 10 && sample.millivolts >= 2800 && sample.millivolts <= 4350;
            sample.measured_at = now / 1000000; /* Monotonic uptime, not wall time. */
            next_voltage = now + INT64_C(30000000);
        }
        sample.percent_estimate = -1;
        if (sample.valid && sample.charge_valid && !sample.charging) {
            int v = sample.millivolts;
            /* Curve from LazyYoun zectrix-s3-epaper-4.2.cc@51812e4, MIT: see home_panel.c. */
            int percent = (-v * v + 9016 * v - 19189000) / 10000;
            sample.percent_estimate = percent < 0 ? 0 : percent > 100 ? 100 : percent;
        }
        home_lock();
        home_runtime.battery = sample;
        home_unlock();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
