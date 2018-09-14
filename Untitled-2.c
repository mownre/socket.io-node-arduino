#include <18F4550.h>

#fuses HSPLL,NOWDT,PUT,BROWNOUT,NOLVP,PLL5,USBDIV,CPUDIV1

#use delay(clock=48M)

#define PRESENCA PIN_B6
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


#include <string.h>
#include <stdlib.h>
#include <usb_cdc.h>
#include "DHT11.c"


#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, stream=HW)

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

int1 CRC_ATM(int8 b[]);
void decodifica_mensagem_serial_externa(int8 length, int8 *buffer[]);
void verifica_chegada_dado_serial_externa();

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