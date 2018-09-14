DICE
/*
Bootloader usa um total de ROM 20% RAM 24% até 32%
V7.0 com bootloader
Exemplos de mensagens para endereço 01:
 **************MESTRE => SLAVE
Solicita temperatura => 7E 06 01 01 FD 2A
Solicita luminosidade =>7E 06 01 02 FC 2A
Solicita presença => 7E 06 01 03 FB 2A
Solicita umidade => 7E 06 01 09 F5 2A
 */
//----------------------------------------------------------------------------
// MCU e ADC de 10bits
//----------------------------------------------------------------------------
#include <18F4550.h>
#device ICD=TRUE
#device adc=10
//----------------------------------------------------------------------------
// Fuses, clock em PLL e UART
//----------------------------------------------------------------------------
#fuses HSPLL,NOWDT,PUT,BROWNOUT,NOLVP,PLL5,USBDIV,CPUDIV1
#use delay(clock=48M)
//----------------------------------------------------------------------------
// IO Pin Configuration
//----------------------------------------------------------------------------
//#define VENT PIN_C2 // Ventoinha
#define LAMPADA PIN_C1 // Aquecedor
#define EN485 PIN_A4 // Se '1' (transmissão) se '0' (recepção)
#define PRESENCA PIN_B6 // Se '1' encontrou alguém está em paralelo com um BIT de endereço.
//#define UMIDADE PIN_B5 // Umidade, setado em DTH11.h
//----------------------------------------------------------------------------
// LCD Pin Configuration
//----------------------------------------------------------------------------
#define LCD_ENABLE_PIN PIN_E1
#define LCD_RS_PIN PIN_E0
#define LCD_RW_PIN PIN_E2

#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7
//----------------------------------------------------------------------------
// Bytes fixos
//----------------------------------------------------------------------------
#define INICIO_QUADRO 0x7E
#define FIM_QUADRO 0x2A
//----------------------------------------------------------------------------
// Tipo de sensor/atuador
//----------------------------------------------------------------------------
#define KEAP_ALIVE 0X40
#define SENSOR_TEMPERATURA 0X01
#define SENSOR_LUMINOSIDADE 0X02
#define SENSOR_PRESENCA 0X03
#define SENSOR_UMIDADE 0X09
//----------------------------------------------------------------------------
// Bibliotecas usadas
//----------------------------------------------------------------------------
#include <usb_bootloader.h> // Para entrar no bootloader segurar o botão INT2(RB2) e ligar a placa
#include <lcd.c>
#include <string.h>
#include <stdlib.h>
#include <usb_cdc.h>
#include "DHT11.c"
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, stream=HW)
//#use rs232(baud=9600, rcv=PIN_B5, stream=SW)
//#include "timer.c" // Rotinas de tempo
///----------------------------------------------------------------------------
// Variáveis globais
//-----------------------------------------------------------------------------
#define BUFFER_LEN_SERIAL 20 // Tamanho máximo do vetor de RX
int8 rx_serial;
char buffer_serial_rx[BUFFER_LEN_SERIAL];
int16 buffidx_serial = 0;
int1 recebeu_dado_serial = false;
//int32 timeout_rx_serial = 0;
int16 i = 0;
int16 readLUM, new_readLUM;
int8 conv_readLUM;
char endereco_fixo = 0;
char endereco_remoto = 1;

int1 sou_mestre = 0;
int1 flag_presenca = 0;
int16 timeout_presenca = 0;
unsigned char state = 0;
///----------------------------------------------------------------------------
// Funções
//-----------------------------------------------------------------------------
int1 CRC_ATM(int8 b[]);
void decodifica_mensagem_serial_externa(int8 length, int8 *buffer[]);
void verifica_chegada_dado_serial_externa();
///----------------------------------------------------------------------------
// Contador de tempo em Timer1 - 16bits
//-----------------------------------------------------------------------------
#INT_TIMER1

void time() { // 48M/4 = 12M DEFAULT
    // 12M/4(DIVISÃO DO CLOCK) = 3MHz
    set_timer1(0); // 3MHz/65536(ESTOURO DO TIMER1) = 45, 7763671875HZ
    i++;
    // 45HZ = 0,0218453333s = 21,845333ms
    // ESTOURO A CADA 21,84ms
    if (flag_presenca) {
        timeout_presenca++;
    }
}
///----------------------------------------------------------------------------
// Verificar o endereço da placa em PortB
//-----------------------------------------------------------------------------

void verificar_endereco() { // Configuração dos endereços para até 31 dispositivos + mestre = > 32
    int temp_state;
    temp_state = input_B();
    switch (temp_state) { // Lembrar que as chaves são invertidas, 0 aqui significa ON no MCU
        case 0b01111111: endereco_fixo = 0x00; //Reservado para o mestre
            sou_mestre = 1;
            //lcd_gotoxy(9, 1);
            //lcd_putc("*");
            break;
        case 0b11100000: endereco_fixo = 0x31;
            break;
        case 0b11100001: endereco_fixo = 0x30;
            break;
        case 0b11100010: endereco_fixo = 0x29;
            break;
        case 0b11100011: endereco_fixo = 0x28;
            break;
        case 0b11100100: endereco_fixo = 0x27;
            break;
        case 0b11100101: endereco_fixo = 0x26;
            break;
        case 0b11100110: endereco_fixo = 0x25;
            break;
        case 0b11100111: endereco_fixo = 0x24;
            break;
        case 0b11101000: endereco_fixo = 0x23;
            break;
        case 0b11101001: endereco_fixo = 0x22;
            break;
        case 0b11101010: endereco_fixo = 0x21;
            break;
        case 0b11101011: endereco_fixo = 0x20;
            break;
        case 0b11101100: endereco_fixo = 0x19;
            break;
        case 0b11101101: endereco_fixo = 0x18;
            break;
        case 0b11101110: endereco_fixo = 0x17;
            break;
        case 0b11101111: endereco_fixo = 0x16;
            break;
        case 0b11110000: endereco_fixo = 0x15;
            break;
        case 0b11110001: endereco_fixo = 0x14;
            break;
        case 0b11110010: endereco_fixo = 0x13;
            break;
        case 0b11110011: endereco_fixo = 0x12;
            break;
        case 0b11110100: endereco_fixo = 0x11;
            break;
        case 0b11110101: endereco_fixo = 0x10;
            break;
        case 0b11110110: endereco_fixo = 0x09;
            break;
        case 0b11110111: endereco_fixo = 0x08;
            break;
        case 0b11111000: endereco_fixo = 0x07;
            break;
        case 0b11111001: endereco_fixo = 0x06;
            break;
        case 0b11111010: endereco_fixo = 0x05;
            break;
        case 0b11111011: endereco_fixo = 0x04;
            break;
        case 0b11111100: endereco_fixo = 0x03;
            break;
        case 0b11111101: endereco_fixo = 0x02;
            break;
        case 0b11111110: endereco_fixo = 0x01;
            break;
        default:endereco_fixo = 0x99;
            break; // Configuração da chave errada, endereço não existe = > erro
    }
}
// solicita aos escravos o status - ainda não usado

void verificar_status(char temp_endereco_remoto) {
    output_high(EN485);
    // delay_ms(10);
    printf("%c", temp_endereco_remoto);
    printf(usb_cdc_putc, "%c", temp_endereco_remoto);
    output_low(EN485);
    // delay_ms(10);
}
///----------------------------------------------------------------------------
// Atualiza a temperatura localmente
//-----------------------------------------------------------------------------

/*void ler_temperatura_local(){
 readADC = new_readADC;
 conv_readADC = (readADC*0.488);
 lcd_gotoxy(13,1);
 printf(lcd_putc,"%d%cc",conv_readADC,0xdf);
}*/
///----------------------------------------------------------------------------
// Verifica a presença localmente, reinicia o contador caso exista movimento
//-----------------------------------------------------------------------------
void verficar_presenca_local() {
    if (input(PRESENCA)) { // Novo movimento somar com o tempo de 5s mínimo do sensor
        output_high(LAMPADA);
        flag_presenca = 1;
        timeout_presenca = 0; // reinicia o contador
    }

    if (timeout_presenca > 2747) { // 2747 = 59,59segundos ~1minuto sem movimento, desliga a lampada
        output_low(LAMPADA);
        flag_presenca = 0;
        timeout_presenca = 0;
    }
}
///----------------------------------------------------------------------------
// Faz a leitura dos dados (temperatura e umidade) do sensor DHT11
//-----------------------------------------------------------------------------

void ler_DHT11_local() {
    state = get_data();
    switch (state) {
        case 1:
        {
            break;
        }
        case 2:
        { // Erro de sensor
            lcd_gotoxy(1, 2);
            lcd_putc("SEN");
            break;
        }
        case 3:
        { // Erro de Checksun
            lcd_gotoxy(1, 2);
            lcd_putc("CKS");
            break;
        }
        default:
        { // Mensagem chegou correta
            lcd_gotoxy(7, 1); // Temperatura
            printf(lcd_putc, "%3u%c", values[2], 0xdf);

            lcd_gotoxy(14, 1); // Umidade
            printf(lcd_putc, "%u%%", values[0]);
            break;
        }
            Trabalho de Conclusão de Curso, Tayse Maria Bandeira da Silva - 2017
                    56
    }
}
///----------------------------------------------------------------------------
// Atualiza a luminosidade localmente
//-----------------------------------------------------------------------------

void ler_luminosidade_local() {
    set_adc_channel(3); // Leitura da luminosidade, canal 3
    delay_ms(1); // Estabilização
    new_readLUM = Read_ADC();
    if (readLUM != new_readLUM) {
        readLUM = new_readLUM;
        conv_readLUM = (readLUM * 0.09765625); // 100/1024
        lcd_gotoxy(14, 2);
        printf(lcd_putc, "%u%%", conv_readLUM);
    }
}
///----------------------------------------------------------------------------
// Interrupção de RX - adicionar o timeout
//-----------------------------------------------------------------------------
#INT_RDA

void serial() {
    rx_serial = getc();
    buffer_serial_rx[buffidx_serial] = rx_serial;
    if (buffer_serial_rx[buffidx_serial] == 0x2A) recebeu_dado_serial = true;
    buffidx_serial++;
    // timer_start(&timeout_rx_serial);
}
//----------------------------------------------------------------------------
// Calcula o Checksum() - para tamanho 6 OK
//----------------------------------------------------------------------------

int1 CRC_ATM(int8 b[]) {
    int1 out = 0;
    int k, sum = 0;
    for (k = 2; k < b[1] - 2; k++) {
        sum += b[k];
    }
    if ((0xFF - sum) == (b[b[1] - 2])) out = 1;
    return out;
}
//----------------------------------------------------------------------------
// Informa no LCD que respondeu uma solicitação
//----------------------------------------------------------------------------

void enviar_setas_lcd() {
    lcd_gotoxy(1, 2);
    lcd_putc(">>>");
}
//----------------------------------------------------------------------------

Trabalho de Conclusão de Curso, Tayse Maria Bandeira da Silva - 2017
57
// Apaga as setas no LCD após um tempo determinado
//----------------------------------------------------------------------------
void apagar_setas_lcd() {
    lcd_gotoxy(1, 2);
    lcd_putc(" ");
}
//----------------------------------------------------------------------------
// Envia a temperatura, a medição já foi feita
//----------------------------------------------------------------------------

void enviar_temperatura_local() {
    int8 cks;
    enviar_setas_lcd();
    putc(INICIO_QUADRO);
    putc(0x07);
    putc(endereco_fixo);
    putc(SENSOR_TEMPERATURA);
    putc(values[2]);
    cks = 0xFF - (endereco_fixo + SENSOR_TEMPERATURA + values[2]);
    putc(cks);
    putc(FIM_QUADRO);

    // printf(usb_cdc_putc,"%c%c%c%c%c%c%c",INICIO_QUADRO, 0x07, endereco_fixo,
    0x01, conv_readADC, cks, FIM_QUADRO);
}
//----------------------------------------------------------------------------
// Envia a umidade, a medição já foi feita
//----------------------------------------------------------------------------

void enviar_umidade_local() {
    int8 cks;
    enviar_setas_lcd();
    putc(INICIO_QUADRO);
    putc(0x07);
    putc(endereco_fixo);
    putc(SENSOR_UMIDADE);
    putc(values[0]);
    cks = 0xFF - (endereco_fixo + SENSOR_UMIDADE + values[0]);
    putc(cks);
    putc(FIM_QUADRO);
}
//----------------------------------------------------------------------------
// Envia o status de presença no ambiente
//----------------------------------------------------------------------------

void enviar_presenca() {
    int8 cks;
    enviar_setas_lcd();// Trabalho de Conclusão de Curso, Tayse Maria Bandeira da Silva - 2017

    putc(INICIO_QUADRO);
    putc(0x07);
    putc(endereco_fixo);
    putc(SENSOR_PRESENCA);
    if (flag_presenca) { // Testa a flag para informar o resultado
        putc(0x2B);
        cks = 0xFF - (endereco_fixo + SENSOR_PRESENCA + 0X2B);
    } else {
        putc(0x2D);
        cks = 0xFF - (endereco_fixo + SENSOR_PRESENCA + 0X2D);
    }
    putc(cks);
    putc(FIM_QUADRO);
}
//----------------------------------------------------------------------------
// Envia a luminosidade do ambiente
//----------------------------------------------------------------------------

void enviar_luminosidade_local() {
    int8 cks;
    int new_ReadLUM_msb = 0, new_ReadLUM_lsb = 0;
    enviar_setas_lcd();
    putc(INICIO_QUADRO);
    putc(0x08);
    putc(endereco_fixo);
    putc(SENSOR_LUMINOSIDADE);
    new_ReadLUM_msb = make8(new_readLUM, 1);
    putc(new_readLUM_msb);
    new_ReadLUM_lsb = make8(new_readLUM, 0);
    putc(new_readLUM_lsb);
    cks = 0xFF - (endereco_fixo + SENSOR_LUMINOSIDADE + new_readLUM_msb +
            new_readLUM_lsb);
    putc(cks);
    putc(FIM_QUADRO);
}
//----------------------------------------------------------------------------
// Valores de início da placa
//----------------------------------------------------------------------------

void iniciar_valores_padrao() {
    // iniciar_time();
    //lcd_init();

    // usb_init(); // alimentado pelo USB, loop infinito até ser conectado
    usb_init_cs(); // sem alimentação pelo USB e funciona sem estar conectado
    output_low(EN485);
    // memset(&buffer_serial_externa_rx, 0x00, BUFFER_LEN_SERIAL_EXTERNA);

    // readADC = 0;
    //lcd_putc("\f"); // Limpa LCD
    verificar_endereco(); // Única vez que chama o endereço, depois ele fica fixo para sempre
    //lcd_gotoxy(1, 1);
    //lcd_putc("End:");
    //printf(lcd_putc, "%2X", endereco_fixo);
    //lcd_gotoxy(12, 1);
    //lcd_putc("U:");
    //lcd_gotoxy(12, 2);
    //lcd_putc("L:");
}

void main(void) {
    setup_timer_1(T1_INTERNAL | T1_DIV_BY_4); // setup interrupts
    enable_interrupts(INT_TIMER1);

    setup_adc(ADC_CLOCK_INTERNAL);
    port_b_pullups(TRUE); // habilita os resistores de pull up do PORTB para os SWITCHS
    enable_interrupts(INT_RDA);
    enable_interrupts(GLOBAL);

    iniciar_valores_padrao();

    char c;

    int8 laco; // endereco_fixo_int;
    while (1) {
        usb_task();

        if ((recebeu_dado_serial)) {// && (timer_hasExpired(&timeout_rx_serial, 5))){ //Chegou algo na porta serial ?
            // c = getc();

            if (sou_mestre) {
                // usb_cdc_putc(c);
                for (laco = 0; laco <= buffidx_serial; laco++) {
                    usb_cdc_putc(buffer_serial_rx[laco]);
                }
            } else if (CRC_ATM(buffer_serial_rx)) { // Função para testar se CKs está OK, para tamanho = 6 OK

                if (buffer_serial_rx[2] == endereco_fixo) { // Mensagem de RX => 7E 06 ID TIPO CKS 2A = > 7E 06 01 01 FE 2A
                    switch (buffer_serial_rx[3]) {
                        case SENSOR_TEMPERATURA: enviar_temperatura_local();
                            break;
                            // Envia a temperatura quando solicitado pelo MESTRE
                        case SENSOR_LUMINOSIDADE: enviar_luminosidade_local();
                            break; // Envia a luminosidade quando solicitado pelo MESTRE
                        case SENSOR_PRESENCA: enviar_presenca();
                            break; // Envia a presenca quando solicitado pelo MESTRE
                        case SENSOR_UMIDADE: enviar_umidade_local();
                            break; //Envia a umidade quando solicitado pelo MESTRE

                        default: break;
                    }
                }
            }
            recebeu_dado_serial = false;
            buffidx_serial = 0;
        }

        if (usb_cdc_kbhit()) {
            c = usb_cdc_getc();
            fputc(c);
        }

        if (i >= 200) { // ~4,4s
            i = 0;

            if (sou_mestre) {
                if (endereco_remoto < 10) {
                    verificar_status(endereco_remoto);
                    endereco_remoto++;
                } else {
                    endereco_remoto = 0x01;
                }
            }

            // Funções criadas
            verficar_presenca_local();
            ler_luminosidade_local();
            ler_DHT11_local();
            apagar_setas_lcd();
        }
    }
}
