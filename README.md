# U4C8-conversoresAD
 
Com o objetivo de consolidar os conceitos sobre o uso de conversores analógico-digitais (ADC) no
RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab, propõe-se a realização da
seguinte atividade prática individual.

## Objetivos
1) Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.
2) Utilizar o PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick.
3) Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
4) Aplicar o protocolo de comunicação I2C na integração com o display.

## Descrição do Projeto:
O joystick fornecerá valores analógicos correspondentes aos eixos X e Y, que serão utilizados para:
### Controlar a intensidade luminosa dos LEDs RGB, onde:
1) O LED Azul terá seu brilho ajustado conforme o valor do eixo Y. Quando o joystick estiver solto
(posição central - valor 2048), o LED permanecerá apagado. À medida que o joystick for movido para
cima (valores menores) ou para baixo (valores maiores), o LED aumentará seu brilho gradualmente,
atingindo a intensidade máxima nos extremos (0 e 4095.

2)  O LED Vermelho seguirá o mesmo princípio, mas de acordo com o eixo X. Quando o joystick estiver
solto (posição central - valor 2048), o LED estará apagado. Movendo o joystick para a esquerda
(valores menores) ou para a direita (valores maiores), o LED aumentará de brilho, sendo mais intenso nos extremos (0 e 4095).

3) Os LEDs serão controlados via PWM para permitir variação suave da intensidade luminosa.
Exibir no display SSD1306 um quadrado de 8x8 pixels, inicialmente centralizado, que se moverá
proporcionalmente aos valores capturados pelo joystick.
Adicionalmente, o botão do joystick terá as seguintes funcionalidades:

4) Alternar o estado do LED Verde a cada acionamento.

5) Modificar a borda do display para indicar quando foi pressionado, alternando entre diferentes estilos de borda a cada novo acionamento.

### Finalmente, o botão A terá a seguinte funcionalidade:

1) Ativar ou desativar os LED PWM a cada acionamento.
Neste projeto, deverá ser utilizados os seguintes componentes já interconectados à placa BitDogLab:

2) LED RGB, com os pinos conectados às GPIOs (11, 12 e 13).

3) Botão do Joystick conectado à GPIO 22.

4) Joystick conectado aos GPIOs 26 e 27.

5) Botão A conectado à GPIO 5.

6) Display SSD1306 conectado via I2C (GPIO 14 e GPIO15).

## Funcionalidades Implementadas
- foi adicionada a função `debounce` para controle do tempo de acionamento dos botões para evitar problemas de leitura.

- foi implementada a função `button_pressed_isr` para como função de retorno para a interrupção quando pressionado o botão A ou do Joystick. Quando pressinado o botão A alterada o estado das funções do PWM dos leds vermelhos e azul, mudando o estado dos mesmos. Já para o do joystick altera o estado do led verde e a exibição da borda do monitor.

- A função `setup_pwm` é responsável pela ativação dos leds vermelho e azul com PWM, com intensidade mexima de 4095.

- A função `intensidade_led` vai ser responsável para controle dos niveis de intensidade de acordo com a movimentação do joystick que está na função principal.

- A função principal `MAIN` está com o loop para funcionamento continuo, as ativações dos pinos, funções de interrupção e chamada das demais funções.

## Vídeo demonstrativo
- [Vídeo demonstrando funcionamento](https://youtu.be/sUFq4Fe7Mzk)