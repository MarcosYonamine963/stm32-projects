#include "mc_protocol.h"
#include "circular_buffer.h"
#include "uart.h"
#include "string.h"
#include "stdio.h"
#include "my_clock.h"
#include "convert.h"
#include "params.h"

/*
                     PROTOCOL FRAME
    |-----------------------------------------------------|
    |  SOF  |  LEN    |  TYPE  |  CMD    |  DATA   | EOF  |
    |  0x01 | 2 bytes | 1 byte | 2 bytes | n bytes | 0x04 |
    |-----------------------------------------------------|

    SOF: Start Of Frame: 0x01
    LEN: Length of the message. Computed from TYPE field to DATA field
    TYPE: Type of the message. Write (0x00), Read (0x01) or Report (0x02)
    CMD: Command
    DATA: Data (depends on Command)
    EOF: End of Frame: 0x04

    Except SOF and EOF, all the message bytes must be sent with byte stuffing.
    The byte stuffing ESC char is 0x10, and the offset is 0x20.
    The prohibited bytes on the message are: 0x01, 0x10 and 0x04.
    If you want to send one of those bytes, first, send the ESC char, and then,
        add the offset (0x20) to the prohibited byte to be sent, and send it.

    For example, if the frame you want to send is :

    |---------------------------------------------------|
    |  SOF  |  LEN   |  TYPE  |  CMD   |  DATA   | EOF  |
    |   01  | 00 04  |   00   | 10 25  |   01    |  04  |
    |---------------------------------------------------|

    then, you must send the following package:  01 00 10 24 00 10 30 25 10 21 04.
*/

// Protocol Control bytes
#define PROTOCOL_SOF        0x01
#define PROTOCOL_ESC        0x10
#define PROTOCOL_OFFSET     0x20
#define PROTOCOL_EOF        0x04

// Msg Types:
#define TYPE_WRITE          0x00
#define TYPE_READ           0x01
#define TYPE_REPORT         0x02

// Commands:
#define CMD_PERIPH_INFO         0x0000
#define CMD_RESET               0x0001
#define CMD_RESTORE_FACTORY     0x0002
#define CMD_PERIPH_STATUS       0x0003
#define CMD_CONF_DEBUG          0x0004
#define CMD_SET_DEBUG_MASK      0x0005
#define CMD_RESET_DEBUG_MASK    0x0006

// Buzzer commands
#define CMD_CONF_BUZZER_EN      0x0010

// Clock commands
#define CMD_CLOCK_TIME          0x0020  // Write, Read. 3 Bytes: Hour, Min, Sec

#define UART_PROTOCOL UART_MAIN



/*
    CMD DEBUG Data Field: 2 Bytes.
    Bit field:


    |--------------------------------------------------------------------|
    |  Bit 15 (MSB)   |  Bit 14       |  Bit 13         |  Bit 12        |
    |  RESERVED       | RESERVED      | RESERVED        | RESERVED       |
    |--------------------------------------------------------------------|

    |--------------------------------------------------------------------|
    |  Bit 11         |  Bit 10       |  Bit 9         |  Bit 8          |
    |  RESERVED       |  RESERVED     |  RESERVED      | RESERVED        |
    |--------------------------------------------------------------------|

    |--------------------------------------------------------------------|
    |  Bit 7          |  Bit 6        |  Bit 5          |  Bit 4         |
    |  All debug EN   | Clock Seconds | Clock Minutes   | Protocol Parse |
    |--------------------------------------------------------------------|

    |--------------------------------------------------------------------|
    |  Bit 3          |  Bit 2        |  Bit 1          |  Bit 0 (LSB)   |
    |  Buzzer events  |  Leds events  |  Encoder events | Buttons Events |
    |--------------------------------------------------------------------|

    DESCRIPTION
    --------------------------------------------------------------------
    Bit 7: All debug EN:   0: All debug msg are disabled.
                           1: Debug config depend on other bits
    --------------------------------------------------------------------
    Bit 6: Clock seconds:  0: Debug disabled (recommended)
                           1: Show time every second (not recommended)
    --------------------------------------------------------------------
    Bit 5: Clock Minutes:  0: Debug disabled
                           1: Show time every minute
    --------------------------------------------------------------------
    Bit 4: Protocol Parse: 0: Debug disabled
                           1: Show successful parse of received packs
    --------------------------------------------------------------------
    Bit 3: Buzzer events:  0: Debug disabled
                           1: Show when buzzer state changes
    --------------------------------------------------------------------
    Bit 2: Leds events:    0: Debug disabled
                           1: Show when any Led state changes
    --------------------------------------------------------------------
    Bit 1: Encoder events: 0: Debug disabled
                           1: Show when encoder state changes
    --------------------------------------------------------------------
    Bit 0: Button events:  0: Debug disabled
                           1: Show when any button state changes
    --------------------------------------------------------------------


    Ex: To enable clock minutes, protocol parse, encoder and buttons:
        set Data field byte to 0b00000000 10110011 (0x00 B3)

        Package to send: 01 00 05 00 00 10 24 00 B3 04

    Ex: To individually change a Debug bit, use commands
        CMD_SET_DEBUG_MASK (0x0005) and CMD_REET_DEBUG_MASK (0x0006)

    Ex: To enable Encoder Events without changing other settings:
        01 00 05 00 00 05 00 02 04

    Ex: To disable Encoder Events without changing other settings:
        01 00 05 00 00 06 00 02 04

 */

typedef enum
{
    PROTOCOL_SM_IDLE = 0,
    PROTOCOL_SM_RECEIVING,
    PROTOCOL_SM_REMOVE_STUFFING
}mc_protocol_sm_e;

typedef enum
{
    PARSE_OK,
    PARSE_ERR_PACK_INVALID,
    PARSE_ERR_MSG_LEN_INVALID
}mc_protocol_parse_status_e;

static mc_protocol_sm_e mc_protocol_sm = PROTOCOL_SM_IDLE;
static mc_protocol_sm_e mc_protocol_prev_state = PROTOCOL_SM_IDLE;
static uint8_t protocol_buffer[1024];
static uint16_t buffer_len;
static uint8_t uart_read_value;






/*################################################################*/
/*################   PRIVATE FUNCTIONS   #########################*/

/*
 * @brief send debug message
 * */
static void Mc_Protocol_Debug_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf)
{
    if( !( (params.debug_cfg) & (1<<4) ) )
    {
        return;
    }

    debug_send_msg(msg, len);
    if(send_cr_lf)
    {
        debug_send_msg((uint8_t *)"\r\n", strlen("\r\n"));
    }

}

/*
 * @brief Send debug message of the package
 * */
static void Mc_Protocol_Debug_Package(uint8_t *Pack, uint16_t pack_len,             \
                                    uint16_t msg_len, uint8_t type, uint16_t cmd,   \
                                    uint8_t *data, uint16_t data_len)
{

    char msg[1024];

    // Pack
    sprintf(msg, "Pacote recebido: ");
    Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);

    convert_uint8_t_array_to_hexascii(Pack, pack_len, (uint8_t *)msg, 1);
    Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);

//    // pack_len
//    convert_uint_16_to_hexascii(pack_len, (uint8_t *)msg, 1);
//    Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
//    sprintf(msg, ": Tamanho do pacote: %d", pack_len);
//    Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
//
//    // msg_len
//    convert_uint_16_to_hexascii(msg_len, (uint8_t *)msg, 1);
//    Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
//    sprintf(msg, ": Tamanho da mensagem: %d", msg_len);
//    Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);

    switch(type)
    {
        case TYPE_READ:
//            convert_uint8_t_to_hexascii(type, (uint8_t *)msg);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "[READ] ");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            break;

        case TYPE_WRITE:
//            convert_uint8_t_to_hexascii(type, (uint8_t *)msg);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "[WRITE] ");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            break;

        case TYPE_REPORT:
//            convert_uint8_t_to_hexascii(type, (uint8_t *)msg);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "[REPORT] ");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);

        default:
//            convert_uint8_t_to_hexascii(type, (uint8_t *)msg);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "[Tipo Desconhecido] ");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            break;
    }

    switch(cmd)
    {
        case CMD_PERIPH_INFO:
//            convert_uint_16_to_hexascii(cmd, (uint8_t *)msg, 1);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "CMD: Requisitar Info de periferico");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
            break;

        case CMD_RESET:
//            convert_uint_16_to_hexascii(cmd, (uint8_t *)msg, 1);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "CMD: Sytem Reset");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
            break;

        case CMD_RESTORE_FACTORY:
//            convert_uint_16_to_hexascii(cmd, (uint8_t *)msg, 1);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "CMD: Restaurar Padrao de Fabrica");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
            break;

        case CMD_PERIPH_STATUS:
//            convert_uint_16_to_hexascii(cmd, (uint8_t *)msg, 1);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "CMD: Requisitar Status");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
            break;

        case CMD_CONF_DEBUG:
            sprintf(msg, "CMD: Config Debug");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);

        case CMD_SET_DEBUG_MASK:
//            convert_uint_16_to_hexascii(cmd, (uint8_t *)msg, 1);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "CMD: Set Debug Mask");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
            break;

        case CMD_RESET_DEBUG_MASK:
//            convert_uint_16_to_hexascii(cmd, (uint8_t *)msg, 1);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "CMD: Reset Debug Mask");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
            break;

        case CMD_CONF_BUZZER_EN:
//            convert_uint_16_to_hexascii(cmd, (uint8_t *)msg, 1);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "CMD: Configurar Buzzer");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
            break;

        default:
//            convert_uint_16_to_hexascii(cmd, (uint8_t *)msg, 1);
//            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);
            sprintf(msg, "CMD: Comando Desconhecido");
            Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 1);
            break;
    }// end switch(cmd)

    if(data_len)
    {
        sprintf(msg, "Dados do pacote: ");
        Mc_Protocol_Debug_Send((uint8_t *)msg, strlen(msg), 0);

        uint8_t ascii_data[(data_len)*3];
        convert_uint8_t_array_to_hexascii(data, data_len, ascii_data, 1);
        Mc_Protocol_Debug_Send((uint8_t *)ascii_data, strlen((char *)ascii_data), 1);
    }
}// end Mc_Protocol_Debug_Package

/*
 * @brief Parse the Package
 * @param Pack [IN]:        The complete received Protocol Frame (from SOF to EOF),
 *                          with byte stuffing already removed
 * @param pack_len [IN]:    Length of the received Pack (from SOF to EOF)
 * @param msg_len [OUT]:    Length of the msg (form TYPE field to DATA field)
 * @param type [OUT]:       The type of the package (see protocol define)
 * @param cmd [OUT]:        The command received (see protocol define)
 * @param data [OUT]:       Package Data field
 * @param data_len [OUT]:   Length of the Data field
 * */
static mc_protocol_parse_status_e Mc_Protocol_Parse(uint8_t *Pack, uint16_t pack_len, uint16_t *msg_len,
                                                    uint8_t *type, uint16_t *cmd,
                                                    uint8_t *data, uint16_t *data_len)
{
    if(pack_len < 7)
    {
        // invalid package
        msg_len = 0;
        return PARSE_ERR_PACK_INVALID;
    }


    *msg_len = (Pack[1] << 8) | (Pack[2]);
    *type = (Pack[3]);
    *cmd = (Pack[4] << 8) | (Pack[5]);

    if(*msg_len != (pack_len - 4))
    {
        // invalid msg length
        *msg_len = 0;
        return PARSE_ERR_MSG_LEN_INVALID;
    }

    if(*msg_len > 3)
    {
        *data_len = *msg_len-3;
        for(uint8_t i = 0; i < *data_len; i++)
        {
            data[i] = Pack[6+i];
        }
    }
    else
    {
        *data_len = 0;
    }

    return PARSE_OK;
}

/*
 * @brief Receive a Complete Package (with SOF and EOF), and insert byte stuffing
 *          except on SOF and EOF.
 *
 * @retval Return the size of the new Package. If 0, error
 * */
static uint16_t Mc_Protocol_Insert_Byte_Stuffing(uint8_t *Package, uint16_t pack_len)
{
    if( (Package[0] != PROTOCOL_SOF) || (Package[pack_len-1] != PROTOCOL_EOF) )
    {
        return 0;
    }

    uint8_t new_pack[pack_len*2];
    uint16_t new_pack_size = 0;

    new_pack[new_pack_size++] = Package[0];

    for(uint16_t i = 1; i < pack_len-1; i++)
    {
        if( (Package[i] == PROTOCOL_SOF) || (Package[i] == PROTOCOL_EOF) || (Package[i] == PROTOCOL_ESC))
        {
            new_pack[new_pack_size++] = PROTOCOL_ESC;
            new_pack[new_pack_size++] = Package[i] + PROTOCOL_OFFSET;
        }
        else
        {
            new_pack[new_pack_size++] = Package[i];
        }
    }

    new_pack[new_pack_size++] = Package[pack_len-1];

    // Copies new_pack to Package
    for(uint16_t i = 0; i < new_pack_size; i++)
    {
        Package[i] = new_pack[i];
    }

    return new_pack_size;
}// end Mc_Protocol_Insert_Byte_Stuffing

/*
 * @brief Mount a Package with byte stuffing
 *
 * @param cmd [IN] command
 * @param type [IN] type
 * @param data [IN] data
 * @param data_len [IN] length of data
 * @param Package [OUT] mounted package
 * @param pack_len [OUT] length of the package
 * */
static void Mc_Protocol_Mount_Package(uint16_t cmd, uint8_t type, uint8_t *data, uint16_t data_len, \
        uint8_t *Package, uint16_t *pack_len)
{
    *pack_len = 0;
    Package[(*pack_len)++] = PROTOCOL_SOF;
    Package[(*pack_len)++] = (data_len + 3)>>8;
    Package[(*pack_len)++] = (data_len + 3);
    Package[(*pack_len)++] = type;
    Package[(*pack_len)++] = (cmd)>>8;
    Package[(*pack_len)++] = (cmd);

    for(uint16_t i = 0; i < data_len; i++)
    {
        Package[(*pack_len)++] = data[i];
    }

    Package[(*pack_len)++] = PROTOCOL_EOF;

    (*pack_len) = Mc_Protocol_Insert_Byte_Stuffing(Package, *pack_len);
}

static void Mc_Protocol_Send_Report(uint8_t *Package, uint16_t pack_len)
{

    Uart_Transmit(UART_PROTOCOL, Package, pack_len);

}

static void Mc_Protocol_CMD_Read_Handler(uint16_t cmd)
{
    switch(cmd)
    {
        case CMD_PERIPH_INFO:
            uint8_t Package[1024];
            uint16_t pack_len = 0;
            uint8_t data[] = {6,7,8,9};
            Mc_Protocol_Mount_Package(CMD_PERIPH_INFO, TYPE_REPORT, data, 4, Package, &pack_len);

//            Package[pack_len++] = PROTOCOL_SOF;
//            Package[pack_len++] = 0x00;
//            Package[pack_len++] = 0x10;
//            Package[pack_len++] = 0x24;
//            Package[pack_len++] = TYPE_REPORT;
//            Package[pack_len++] = CMD_PERIPH_INFO >> 8;
//            Package[pack_len++] = CMD_PERIPH_INFO;
//            Package[pack_len++] = 0xAB;
//            Package[pack_len++] = 04;

            Mc_Protocol_Send_Report(Package, pack_len);

            break;
        case CMD_RESET:
            // TODO
            break;
        case CMD_RESTORE_FACTORY:
            // TODO
            break;
        case CMD_PERIPH_STATUS:
            // TODO
            break;
        case CMD_CONF_DEBUG:
            // TODO
            break;
        case CMD_SET_DEBUG_MASK:
            // TODO
            break;
        case CMD_RESET_DEBUG_MASK:
            // TODO
            break;
        case CMD_CONF_BUZZER_EN:
            // TODO
            break;
    }// end switch(cmd)

}// end Mc_Protocol_CMD_Read_Handler


static void Mc_Protocol_CMD_Write_Handler(uint16_t cmd, uint8_t *data, uint16_t data_len)
{
    switch(cmd)
    {
        case CMD_PERIPH_INFO:
            // TODO ERROR (WRITE)
            break;
        case CMD_RESET:
            // TODO ERROR (WRITE)
            break;
        case CMD_RESTORE_FACTORY:
            // TODO ERROR (WRITE)
            break;
        case CMD_PERIPH_STATUS:
            // TODO ERROR (WRITE)
            break;
        case CMD_CONF_DEBUG:
            if(data_len < 2)
                return;
            params.debug_cfg = (data[0] << 8) | (data[1]);
            break;
        case CMD_SET_DEBUG_MASK:
            if(data_len < 2)
                return;
            params.debug_cfg |= (data[0] << 8) | (data[1]);
            break;
        case CMD_RESET_DEBUG_MASK:
            if(data_len < 2)
                return;
            params.debug_cfg &= ~( (data[0] << 8) | (data[1]) );
            break;




        case CMD_CONF_BUZZER_EN:
            if(!data_len)
                return;
            params.buzzer_cfg = data[0];
            break;
    }// end switch(cmd)
}



static void Mc_Protocol_Handler(uint8_t *Pack, uint16_t pack_len)
{
    uint16_t msg_len, cmd, data_len;
    uint8_t type;
    uint8_t data[pack_len - 7];

    if(Mc_Protocol_Parse(Pack, pack_len, &msg_len, &type, &cmd, data, &data_len) != PARSE_OK)
    {
        // Parse fail
        return;
    }
    // Parse OK

    // If config to disable debug, send msg before it
    if(cmd == CMD_CONF_DEBUG&& type == TYPE_WRITE && (data[0] == 0 || data[1] == 0))
    {
        Mc_Protocol_Debug_Package(Pack, pack_len, msg_len, type, cmd, data, data_len);
    }
    else if(cmd == CMD_RESET_DEBUG_MASK && (data[1] == (1<<7) || data[1] == (1<<4)))
    {
        Mc_Protocol_Debug_Package(Pack, pack_len, msg_len, type, cmd, data, data_len);
    }

    switch(type)
    {
        case TYPE_READ:
            Mc_Protocol_CMD_Read_Handler(cmd);
            break;
        case TYPE_WRITE:
            Mc_Protocol_CMD_Write_Handler(cmd, data, data_len);
            break;
    }

    // Debug send msg: Package parsing
    Mc_Protocol_Debug_Package(Pack, pack_len, msg_len, type, cmd, data, data_len);


}




/*################################################################*/
/*################   PUBLIC FUNCTIONS   ##########################*/



void Mc_Protocol_Config(void)
{
    Uart_config(UART_PROTOCOL, 115200, UART_NO_REMAP);
}


void Mc_Protocol_SM(void)
{
    if(Uart_Read_from_buffer(UART_PROTOCOL, &uart_read_value) != UART_OK)
    {
        return;
    }

    switch(mc_protocol_sm)
    {
        case PROTOCOL_SM_IDLE:
            if(uart_read_value == PROTOCOL_SOF)
            {
                protocol_buffer[0] = uart_read_value;
                buffer_len = 1;
                mc_protocol_sm = PROTOCOL_SM_RECEIVING;
            }
            break;

        case PROTOCOL_SM_RECEIVING:

            if(uart_read_value == PROTOCOL_SOF)
            {
                protocol_buffer[0] = uart_read_value;
                buffer_len = 1;
                mc_protocol_sm = PROTOCOL_SM_RECEIVING;
            }
            else if(uart_read_value == PROTOCOL_ESC)
            {
                mc_protocol_prev_state = PROTOCOL_SM_RECEIVING;
                mc_protocol_sm = PROTOCOL_SM_REMOVE_STUFFING;
            }
            else if(uart_read_value == PROTOCOL_EOF)
            {
                protocol_buffer[buffer_len++] = uart_read_value;
                mc_protocol_sm = PROTOCOL_SM_IDLE;
                Mc_Protocol_Handler(protocol_buffer, buffer_len);

            }
            else
            {
                protocol_buffer[buffer_len++] = uart_read_value;
            }

            break;

        case PROTOCOL_SM_REMOVE_STUFFING:

            protocol_buffer[buffer_len++] = uart_read_value - 0x20;
            mc_protocol_sm = mc_protocol_prev_state;

            break;


    }// end switch(mc_protocol_sm)

}
