#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    const int windowSize = 5; // Tamanho da janela para a média móvel
    int window[5] = {0,0,0,0,0}; // Inicializa a janela com 0
    int windowSum = 0; // Soma dos valores na janela
    int dataIndex = 0; // Índice atual na janela
    int dataCount = 0; // Contador de dados recebidos (para inicialização)
    int data = 0; // Dado recebido da fila

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!

            // Subtrai o valor antigo e adiciona o novo valor
            windowSum -= window[dataIndex];
            window[dataIndex] = data;
            windowSum += data;

            // Calcula a média móvel
            int movingAverage = windowSum / windowSize;

            // Imprime o dado filtrado
            printf("%d\n", movingAverage);

            // Avança o índice da janela circularmente
            dataIndex = (dataIndex + 1) % windowSize;



            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
