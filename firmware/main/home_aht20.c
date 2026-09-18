#include "home_aht20.h"
#include "home_runtime.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO           38
#define I2C_MASTER_SDA_IO           39
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000
#define AHT20_SENSOR_ADDR           0x38
#define AHT20_PWR_PIN               40

static const char *TAG = "home_aht20";

void home_aht20_init(void) {
    // 1. Włącz zasilanie czujnika
    gpio_set_direction(AHT20_PWR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(AHT20_PWR_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(50)); // Czas na uruchomienie czujnika

    // 2. Skonfiguruj interfejs I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void home_aht20_task(void *unused) {
    home_aht20_init();
    
    while (1) {
        uint8_t cmd[3] = {0xAC, 0x33, 0x00}; // Komenda pomiaru
        i2c_master_write_to_device(I2C_MASTER_NUM, AHT20_SENSOR_ADDR, cmd, sizeof(cmd), pdMS_TO_TICKS(100));
        
        vTaskDelay(pdMS_TO_TICKS(80)); // AHT20 potrzebuje min. 80ms na pomiar
        
        uint8_t data[6];
        esp_err_t err = i2c_master_read_from_device(I2C_MASTER_NUM, AHT20_SENSOR_ADDR, data, sizeof(data), pdMS_TO_TICKS(100));
        
        if (err == ESP_OK && (data[0] & 0x80) == 0) { // Bit zajętości musi być 0
            uint32_t hum_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
            uint32_t temp_raw = (((uint32_t)(data[3] & 0x0F)) << 16) | ((uint32_t)data[4] << 8) | data[5];
            
            float humidity = (hum_raw * 100.0f) / 1048576.0f;
            float temperature = ((temp_raw * 200.0f) / 1048576.0f) - 50.0f;
            
            home_lock();
            home_runtime.local_sensor.local_temperature = temperature;
            home_runtime.local_sensor.local_humidity = humidity;
            home_runtime.local_sensor.sensor_valid = true;
            home_unlock();
            
            ESP_LOGI(TAG, "Odczyt AHT20: Temp: %.1f C, Wilg: %.1f %%", temperature, humidity);
        } else {
            home_lock();
            home_runtime.local_sensor.sensor_valid = false;
            home_unlock();
            ESP_LOGE(TAG, "Błąd odczytu AHT20");
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Czekaj 10 sekund przed kolejnym pomiarem
    }
}