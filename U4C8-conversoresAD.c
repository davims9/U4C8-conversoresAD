#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"

// Definições dos pinos

#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13
#define JOYSTICK_BUTTON 22
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define BUTTON_A_PIN 5
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define SQUARE_SIZE 8

// Variáveis globais
volatile bool green_led_state = false;
volatile bool status_borda = true;
volatile bool pwm_enabled = true;
volatile bool button_pressed = false;
volatile uint32_t last_button_a_time = 200;
volatile uint32_t last_button_joystick_time = 200;

// Estrutura para o ssd SSD1306
ssd1306_t ssd;


// Função de debouncing
bool debounce(uint32_t *last_time) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - *last_time > 50) {
        *last_time = current_time;
        return true;
    }
    return false;
}


// Função de interrupção para o botão A
void button_pressed_isr(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A_PIN && debounce(&last_button_a_time)) {
        pwm_enabled = !pwm_enabled;
    }
    if (gpio == JOYSTICK_BUTTON && debounce(&last_button_joystick_time)) {
        green_led_state = !green_led_state;
        gpio_put(LED_GREEN, green_led_state);
        status_borda = !status_borda;        
    }

}


// Função para configurar o PWM
void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice_num, 4095);
    pwm_set_enabled(slice_num, true);
}

// Função para ajustar o brilho do LED
void brilho_led(uint pin, uint16_t value) {
    pwm_set_gpio_level(pin, value);
}


uint16_t intensidade_led(uint16_t value) {
    if (value == 2048) {
        return 0; // Apagado na posição central
    } else if (value < 2048) {
        return 2048 - value; // Intensidade aumenta à medida que se move para 0
    } else {
        return value - 2048; // Intensidade aumenta à medida que se move para 4095
    }
}


// Função principal
int main() {
    stdio_init_all();

    // Inicialização do ADC para o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Configuração dos pinos dos LEDs
    setup_pwm(LED_RED);
    setup_pwm(LED_BLUE);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    pwm_set_gpio_level(LED_GREEN, 4095);

    // Configuração dos botões com interrupções

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON);


    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_pressed_isr);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON, GPIO_IRQ_EDGE_FALL, true, &button_pressed_isr);
   
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    ssd1306_t ssd; // Inicializa a estrutura do ssd
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o ssd
    ssd1306_config(&ssd); // Configura o ssd
    ssd1306_send_data(&ssd); // Envia os dados para o ssd

    // Limpa o ssd. O ssd inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Posição inicial do quadrado
    uint8_t square_x = (WIDTH - SQUARE_SIZE) / 2;
    uint8_t square_y = (HEIGHT - SQUARE_SIZE) / 2;

    

    while (1) {
        // Leitura dos valores do joystick
        adc_select_input(1);
        uint16_t x_value = adc_read();
        adc_select_input(0);
        uint16_t y_value = adc_read();

        // Controle dos LEDs
        if (pwm_enabled) {
            // Ajusta a intensidade do LED Vermelho com base no eixo X
            uint16_t red_intensity = intensidade_led(x_value);
            brilho_led(LED_RED, red_intensity);

            // Ajusta a intensidade do LED Azul com base no eixo Y
            uint16_t blue_intensity = intensidade_led(y_value);
            brilho_led(LED_BLUE, blue_intensity);
        } else {
            brilho_led(LED_RED, 0);
            brilho_led(LED_BLUE, 0);
        }
        

        // Movimento do quadrado no ssd
        square_x = (x_value * (WIDTH - SQUARE_SIZE)) / 4095;
        square_y = ((4095 - y_value) * (HEIGHT - SQUARE_SIZE)) / 4095;

        // Desenho no ssd
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, square_y, square_x, SQUARE_SIZE, SQUARE_SIZE, true, true);
        ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, status_borda, false);
        ssd1306_send_data(&ssd);
        
        sleep_ms(10);
    }

    return 0;
}