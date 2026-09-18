#include "home_aht20.h"
#include "home_runtime.h"
#include "driver/i2c_master.h" // Zmieniono na nowe API
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO           38
#define I2C_MASTER_SDA_IO           39
#define I2C_MASTER_FREQ_HZ          100000
#define AHT20_SENSOR_ADDR           0x38
#define AHT20_PWR_PIN               40

static const char *TAG = "home_aht20";

void home_aht20_task(void *unused) {
    // 1. Włącz zasilanie czujnika
    gpio_set_direction(AHT20_PWR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(AHT20_PWR_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    // 2. Skonfiguruj nową magistralę I2C (v6.0+)
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    // 3. Dodaj urządzenie AHT20 do magistrali
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AHT20_SENSOR_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    // 4. Główna pętla pomiarowa
    while (1) {
        uint8_t cmd[3] = {0xAC, 0x33, 0x00};
        // Transmisja komendy inicjującej pomiar
        esp_err_t err = i2c_master_transmit(dev_handle, cmd, sizeof(cmd), -1);
        
        if (err == ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(80)); // Czas na wykonanie pomiaru
            
            uint8_t data[6];
            // Odbiór danych z czujnika
            err = i2c_master_receive(dev_handle, data, sizeof(data), -1);
            
            if (err == ESP_OK && (data[0] & 0x80) == 0) { // Sprawdzenie bitu zajętości
                uint32_t hum_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
                uint32_t temp_raw = (((uint32_t)(data[3] & 0x0F)) << 16) | ((uint32_t)data[4] << 8) | data[5];
                
                float humidity = (hum_raw * 100.0f) / 1048576.0f;
                float temperature = ((temp_raw * 200.0f) / 1048576.0f) - 50.0f;
                
                home_lock();
                home_runtime.data.local_temperature = temperature;
                home_runtime.data.local_humidity = humidity;
                home_runtime.data.local_sensor_valid = true;
                home_unlock();
                
                ESP_LOGI(TAG, "Odczyt AHT20: Temp: %.1f C, Wilg: %.1f %%", temperature, humidity);
            } else {
                home_lock();
                home_runtime.data.local_sensor_valid = false;
                home_unlock();
                ESP_LOGE(TAG, "Błąd weryfikacji danych AHT20");
            }
        } else {
            ESP_LOGE(TAG, "Błąd komunikacji I2C z AHT20");
        }
        
        vTaskDelay(pdMS_TO_TICKS(10000)); // Przerwa 10s
    }
}