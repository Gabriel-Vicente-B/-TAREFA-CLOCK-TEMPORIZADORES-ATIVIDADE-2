#include "pico/stdlib.h"  // Biblioteca padrão para entrada e saída no Raspberry Pi Pico
#include "hardware/timer.h"  // Biblioteca para manipulação de temporizadores

// Definição dos pinos GPIO para os LEDs e o botão
#define AZUL 11      // LED azul conectado ao pino GPIO 11
#define VERMELHO 12  // LED vermelho conectado ao pino GPIO 12
#define VERDE 13     // LED verde conectado ao pino GPIO 13
#define Botao 5      // Botão conectado ao pino GPIO 5

volatile bool botao_pressionado = false;  // Variável global para indicar se o botão foi pressionado

// Enumeração para representar o estado dos LEDs
enum led_state {todos, dois, um} estado_atual;

// Função chamada para desligar os LEDs em sequência após um tempo
void turn_off_callback() {
    if (estado_atual == todos) {
        gpio_put(AZUL, 0);  // Desliga o LED azul
        estado_atual = dois;  // Avança para o próximo estado
    } else if (estado_atual == dois) {
        gpio_put(VERMELHO, 0);  // Desliga o LED vermelho
        estado_atual = um;  // Avança para o próximo estado
    } else if (estado_atual == um) {
        gpio_put(VERDE, 0);  // Desliga o LED verde (todos desligados)
        estado_atual = todos;  // Retorna ao estado inicial
    }
    botao_pressionado = false;  // Libera a variável para permitir uma nova ativação do botão
}

// Função chamada quando o botão é pressionado (com debounce)
bool button_debounce_callback(uint gpio, uint32_t events) {
    if (gpio == Botao && botao_pressionado == false) {  // Verifica se o botão foi pressionado e ainda não está em uso
        estado_atual = todos;  // Define o estado inicial
        botao_pressionado = true;  // Marca o botão como pressionado
        gpio_put(AZUL, 1);  // Liga o LED azul
        gpio_put(VERMELHO, 1);  // Liga o LED vermelho
        gpio_put(VERDE, 1);  // Liga o LED verde
        
        // Inicia o temporizador para desligar os LEDs após 3 segundos (a configuração do temporizador está no loop principal)
    }
    return true;  // Retorna verdadeiro para manter a interrupção ativa
}

int main() {
    stdio_init_all();  // Inicializa a comunicação serial (para debugging, caso necessário)

    // Configura os LEDs como saída
    gpio_init(AZUL);
    gpio_set_dir(AZUL, GPIO_OUT);
    
    gpio_init(VERMELHO);
    gpio_set_dir(VERMELHO, GPIO_OUT);
    
    gpio_init(VERDE);
    gpio_set_dir(VERDE, GPIO_OUT);

    // Configura o botão como entrada com pull-up interno (estado padrão = 1, pressionado = 0)
    gpio_init(Botao);
    gpio_set_dir(Botao, GPIO_IN);
    gpio_pull_up(Botao);  // Ativa o resistor de pull-up interno

    // Configura uma interrupção no botão para detectar borda de descida (pressionamento)
    gpio_set_irq_enabled_with_callback(Botao, GPIO_IRQ_EDGE_FALL, true, button_debounce_callback);
    
    while (1) {
        // Se o botão ainda não foi pressionado, agenda um temporizador para desligar os LEDs em 3 segundos
        if (botao_pressionado == false) {
            add_alarm_in_ms(3000, turn_off_callback, NULL, true);
        }

        sleep_ms(500);  // Espera 500 ms antes de repetir o loop
    }
}
