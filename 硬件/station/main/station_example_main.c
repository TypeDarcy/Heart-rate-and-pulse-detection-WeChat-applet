/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
   VERSION : ESP8266RTOS SDKv3.4
*/
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "driver/uart.h"
#include "user_config.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "esp_task_wdt.h"
#include "spi_flash.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      DEBUG_CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      DEBUG_CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  5

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define BUF_SIZE (256)

static const char *TAG = "MQTTBorad";

static int s_retry_num = 0;

esp_mqtt_client_handle_t client;

TaskHandle_t mqtt_task_handler;
static void mqtt_app_start(void);

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, MQ_MQTT_TOPIC_ATTRIBUTE_PUBLISH, "device connect ok.", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, MQ_MQTT_TOPIC_GET, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, MQ_MQTT_TOPIC_GET, 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, MQ_MQTT_TOPIC_ATTRIBUTE_PUBLISH, "subcribe success.", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            ESP_LOGI(TAG, "TOPIC=%.*s\r\n", event->topic_len, event->topic);
            ESP_LOGI(TAG, "DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {

        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            s_retry_num++;
            esp_wifi_connect();
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

    }
}

void wifi_init_sta(const char* ssid, const char* password)
{

    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config;

    strcpy((char *)wifi_config.sta.ssid, ssid);
    strcpy((char*)wifi_config.sta.password, password);

    /* Setting a password implies station will connect to all security modes including WEP/WPA.
        * However these modes are deprecated and not advisable to be used. Incase your Access point
        * doesn't support WPA2, these mode can be enabled by commenting below line */

    if (strlen((char *)wifi_config.sta.password)) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ssid, password);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ssid, password);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);
}

/**
 * @brief 串口初�?�化
 *
 */
static void uart_initialization(){
    uart_config_t uart_config = {
        .baud_rate = 74880,
        .data_bits = UART_DATA_8_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0,BUF_SIZE * 2, 0 ,0 ,NULL, 0);
}

/**
  * @brief  采用JSON的格式上报数�?，�?�函数的用途在于合�? JSON 段，使用 string.h �?的strcat函数
  *         拼接�?
  *
  * @param temperature      温度值，用来补充 Temp �?值�?�的 Value
  * @param pulse            脉搏�?
  * @param SPO2             血氧度
  * @param publish_buf      接受组合�? JSON 所使用的缓冲区
  */
void combine_publish_data(const char* temperature, const char* pulse,const char* spo2 ,char *publish_buf){

    char temp[1024] = {0};
    strcpy(temp, "{");
    strcat(temp,"\"params\":{");
    strcat(temp,"\"Temp\":");
    strcat(temp, temperature);
    strcat(temp,",\"Pulse\":");
    strcat(temp, pulse);
    strcat(temp,",\"SPO2\":");
    strcat(temp, spo2);

    strcat(temp,"}}");
    strcpy(publish_buf, (const char *)temp);
}


/*  @brief 解析从串口传入的传感器数�? 格式 {A,B}
 *
*/
void user_sensorArgsParse(char* temperature, char* pulse,char* spo2, const char* sensor_str){

    uint8_t i = 1, j, k;

    while (sensor_str[i] != ',') {
        pulse[i - 1] = sensor_str[i];
        i++;
    }

    pulse[i - 1] = '\0';
    j = i + 1;

    while(sensor_str[j] != ','){
        spo2[j - i - 1] = sensor_str[j];
        j++;
    }

    spo2[j - i - 1] = '\0';
    k = j + 1;
    while (sensor_str[k] != '}') {
        temperature[k - j - 1] = sensor_str[k];
        k++;
    }

    temperature[k - j - 1] = '\0';
}

void user_sensorStrintCat(const char* uart_received, char* dest_buf){

    uint8_t i = 1;
    dest_buf[0] = '{';
    while (uart_received[i] != '{') {
        dest_buf[i] = uart_received[i];
        i++;
    }
    dest_buf[i] = uart_received[i];
    i++;
    while(uart_received[i] != '{'){
        dest_buf[i] = uart_received[i];
        i++;
    }
    // 第二个json头舍弃
    dest_buf[i] = '\0';
}

/**
 * @brief mqtt 事件处理程序，用于响应mqtt通信过程�?产生的事件来调度callback函数.
 *        参数列表一�?固定�?
 * @param handler_args
 * @param base
 * @param event_id
 * @param event_data
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

/**
 * @brief mqtt 的启动参数，函数内完成mqtt连接参数的�?�置、�?�户�?创建、响应程序注册，
 *        以及将用户loop程序添加到任务列表�?
 *
 */
static void mqtt_app_start(void){
    esp_mqtt_client_config_t mcli_config = {
        .host = CONFIG_MQTT_BORKER_HOST,
        .port = 1883,
        .client_id    = CONFIG_MQTT_CLIENT_ID,
        .protocol_ver = MQTT_PROTOCOL_V_3_1_1,
        .keepalive = 60
    };
    client = esp_mqtt_client_init(&mcli_config);
    if(client == NULL){
        ESP_LOGI(TAG,"[APP] MQTT client create failed.\r\n");
    }else{
        esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
        esp_mqtt_client_start(client);
    }
    //xTaskCreate(mqtt_publish,"sensor_publish_task",4096,NULL, 10, mqtt_task_handler);
    //publish_temperature();
}

void user_gpio_config(){
    gpio_config_t gc;
    gc.intr_type = GPIO_INTR_DISABLE;
    gc.mode      = GPIO_MODE_OUTPUT;
    gc.pin_bit_mask = (1ULL << GPIO_NUM_2);
    gc.pull_down_en = 0;
    gc.pull_up_en   = 0;
    gpio_config(&gc);
    gc.mode = GPIO_MODE_INPUT;
    gc.pin_bit_mask = (1ULL << GPIO_NUM_5);
    gpio_config(&gc);
}
/**
 * @brief 将WIFI的名称、密码存储掉电保护区�?
 *
 * @param ssid wifi名称
 * @param password wifi密码
 */
void NVS_store_wifi(const char* ssid , const char* password){
    nvs_handle handleNvs;
    if(nvs_open("WIFI_INFO", NVS_READWRITE, &handleNvs) != ESP_OK){
        ESP_LOGE(TAG, "Open NVS Table fail.\n");
    }
    // �? WIFI ssid
    if(nvs_set_str(handleNvs, "ssid", ssid) != ESP_OK){
        ESP_LOGE(TAG, "Save wifi ssid fail.\n");
    }else{
        ESP_LOGI(TAG, "Save wifi ssid success. ssid: %s\n", ssid);
    }
    // �? WIFI 密码
    if(nvs_set_str(handleNvs, "password", password) != ESP_OK){
        ESP_LOGE(TAG, "Save wifi password fail.\n");
    }else{
        ESP_LOGI(TAG, "Save wifi password success. password: %s\n", password);
    }

    nvs_commit(handleNvs);
    nvs_close(handleNvs);
}
/**
 * @brief 从掉电保护区读取wifi数据
 *
 * @param ssid
 * @param password
 */
void NVS_read_wifi(char* ssid, char* password){
    nvs_handle handleNvs;
    char t_ssid[32];
    char t_password[64];
    if (nvs_open("WIFI_INFO", NVS_READWRITE, &handleNvs) != ESP_OK) {
        ESP_LOGE(TAG, "Open NVS Table fail.\n");
    }
    uint32_t len = sizeof(t_ssid);
    // �? WIFI ssid
    if(nvs_get_str(handleNvs, "ssid", t_ssid, &len) != ESP_OK){
        ESP_LOGE(TAG, "Read Wifi ssid fail.\n");
    }else{
        ESP_LOGI(TAG, "Read Wifi ssid success. ssid: %s\n", t_ssid);
    }
    len = sizeof(t_password);
    // �? WIFI 密码
    if(nvs_get_str(handleNvs, "password", t_password, &len) != ESP_OK){
        ESP_LOGE(TAG, "Read Wifi password fail.\n");
    }else{
        ESP_LOGI(TAG, "Read Wifi password success. password: %s\n", t_password);
    }

    strcpy(ssid, (const char *)t_ssid);
    strcpy(password, (const char *)t_password);

    nvs_close(handleNvs);
}

/**
 * @brief 新版�?�? RTOS_SDK 使用 app_main 来作为程序入�?.
 *
 */
void app_main()
{
    char temperature[12] = "0";
    char pulse[12]       = "0";
    char spo2[12]         = "0";
    char ssid[32];
    char password[64];
    uint8_t *dat = (uint8_t *)malloc(BUF_SIZE);
    int len,i = 1, j;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    user_gpio_config();
    uart_initialization();

    ESP_LOGI(TAG, "[APP] Uart Init Finished.");

    if(gpio_get_level(GPIO_NUM_5) == 1){
        gpio_set_level(GPIO_NUM_2, 0);
        ESP_LOGI(TAG, "[USER] Please Input Wifi ssid and password");
        ESP_LOGI(TAG, "[USER] use this format: #ssid,password#\n");
        while (((len = uart_read_bytes(UART_NUM_0, dat, BUF_SIZE,
                                    20 / portTICK_RATE_MS))) == 0 ||
            dat[0] != '#')
            ;
        gpio_set_level(GPIO_NUM_2, 1);
        while (dat[i] != ',') {
            ssid[i - 1] = (char)(dat[i]);
            i++;
        }
        ssid[i - 1] = '\0';
        j = i + 1;
        while (dat[j] != '#') {
            password[j - i - 1] = (char)(dat[j]);
            j++;
        }
        password[j - i - 1] = '\0';

        ESP_LOGI(TAG,"[APP] Confirm. ssid:%s,password:%s\n", ssid, password);
        wifi_init_sta(ssid,
                    password);
        NVS_store_wifi(ssid, password);

    }else{
        gpio_set_level(GPIO_NUM_2, 1);
        NVS_read_wifi(ssid, password);
        ESP_LOGI(TAG, "[APP] Confirm. ssid:%s,password:%s\n",
                 ssid, password);
        wifi_init_sta(ssid,
                      password);
    }

    ESP_LOGI(TAG,"[APP] Wifi Init Finished.\n");

    mqtt_app_start();
    ESP_LOGI(TAG,"[APP] MQTT Start.\n");



    while(1){


        len = uart_read_bytes(UART_NUM_0, dat, BUF_SIZE, 20 / portTICK_RATE_MS);
        //这里用来做用户操�?--------------
        if(len > 0){
            ESP_LOGI(TAG,"[APP] enter mqtt stream\n");
            if (strchr((const char *)dat, '\n') != NULL) {
                // 如果串口�?件发送来的数�?结尾�?带换行�?�，通过该分�?进�?�筛�?
                dat[len] = '\0';
            }

            if (dat[0] == '{') {
                ESP_LOGI(TAG,"[APP] Received data from sensor.\n");
                char publish_buf[512] = "HelloWorld";
                user_sensorArgsParse(temperature, pulse, spo2,
                                     (const char *)dat);
                ESP_LOGI(TAG, "[APP] Publish sensor data.\n");
                combine_publish_data((const char*)temperature,(const char*)pulse,(const char*)spo2, publish_buf);

                ESP_LOGI(TAG,"[DEBUG] The published message:\n");
                ESP_LOGI(TAG,"%s\n", publish_buf);
                /***
                 *  esp_mqtt_client_publish 函数�? 发布 函数�?
                 */
                esp_mqtt_client_publish(client, MQ_MQTT_TOPIC_ATTRIBUTE_PUBLISH, (const char*)publish_buf, 0, 0, 0);
            }

        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
    }

    free(dat);
}
