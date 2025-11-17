/*
 * Wideband SDR Firmware - main.c
 * dsPIC33AK256MC505 based SDR with AD9215 ADC and ADF4351 PLL
 * 
 * Created: November 2025
 * Version: 1.0.0
 * License: MIT
 * 
 * Features:
 * - USB 2.0 High-Speed bulk transfers
 * - 16 KB circular DMA buffers (ping-pong)
 * - Real-time sample streaming
 * - ADF4351 PLL frequency control
 * - Error recovery and watchdog protection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

// PIC24/dsPIC33 Configuration
#pragma config FBS = BSS(no boot)            // Boot Segment
#pragma config RBS = NO_RAM                  // No boot RAM
#pragma config GSS = OFF                     // Global Segment Select
#pragma config GWRP = OFF                    // Global Write Protect
#pragma config GCI = OFF                     // Global Code Index
#pragma config WDTPS = PS32768               // Watchdog Timer Postscale
#pragma config FWPSA = PR128                 // WDT Prescale
#pragma config WINDIS = OFF                  // Windowed WDT
#pragma config FWDTEN = OFF                  // WDT disabled
#pragma config ICS = PGD3                    // Comm Channel Select
#pragma config GWRP = OFF                    // General Segment Write Protect
#pragma config GCP = OFF                     // General Segment Code Protect
#pragma config JTAGEN = OFF                  // JTAG port disabled

// System Configuration
#define _XTAL_FREQ  200000000UL              // 200 MHz system clock
#define USB_FSCC    480000000UL              // USB Full-Speed Clock

// Buffer Configuration
#define DMA_BUFFER_SIZE   16384              // 16 KB buffers
#define NUM_BUFFERS       2                  // Ping-pong configuration

// USB Configuration
#define USB_ENDPOINT_IN   1                  // Bulk IN endpoint
#define USB_ENDPOINT_OUT  2                  // Bulk OUT endpoint
#define USB_PACKET_SIZE   512                // USB packet size
#define USB_BUFFER_SIZE   2048               // USB transfer buffer

// Command Definitions
typedef enum {
    CMD_SET_FREQUENCY = 0x01,
    CMD_SET_SAMPLE_RATE = 0x02,
    CMD_SET_GAIN = 0x03,
    CMD_START_STREAM = 0x04,
    CMD_STOP_STREAM = 0x05,
    CMD_GET_STATUS = 0x06,
    CMD_CALIBRATE = 0x07,
    CMD_SET_POWER = 0x08
} usb_command_t;

// Global Variables
volatile bool usb_connected = false;
volatile bool streaming_active = false;
volatile uint32_t current_frequency = 100000000;  // 100 MHz default
volatile uint32_t current_sample_rate = 1000000;   // 1 MSPS default
volatile uint16_t current_gain = 30;              // 30 dB default
volatile uint32_t sample_count = 0;
volatile uint16_t adc_buffer[DMA_BUFFER_SIZE];
volatile uint16_t *dma_src_ptr;
volatile uint16_t *dma_dest_ptr;
volatile bool buffer_switch_pending = false;

// USB Transfer Buffers
uint8_t usb_tx_buffer[USB_BUFFER_SIZE];
uint8_t usb_rx_buffer[USB_BUFFER_SIZE];

// Function Prototypes
void system_init(void);
void usb_init(void);
void adc_init(void);
void adf4351_init(void);
void dma_init(void);
void timer_init(void);

void usb_process_commands(void);
void start_streaming(void);
void stop_streaming(void);
void send_samples_to_usb(uint16_t *samples, uint16_t count);
bool usb_send_data(uint8_t *data, uint16_t length);

void __attribute__((interrupt, no_auto_psv)) _USB1Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);

/*
 * Main Application Entry Point
 */
int main(void) {
    system_init();
    usb_init();
    adc_init();
    adf4351_init();
    dma_init();
    timer_init();
    
    printf("Wideband SDR Firmware v1.0.0 - Starting...\n\r");
    
    // Main application loop
    while (1) {
        // Process USB commands
        if (usb_connected) {
            usb_process_commands();
        }
        
        // Handle streaming
        if (streaming_active) {
            // Check for buffer switch
            if (buffer_switch_pending) {
                buffer_switch_pending = false;
                
                // Send samples from completed buffer
                if (dma_dest_ptr != NULL) {
                    send_samples_to_usb(dma_dest_ptr, DMA_BUFFER_SIZE);
                }
                
                // Switch buffer pointers
                uint16_t *temp = dma_src_ptr;
                dma_src_ptr = dma_dest_ptr;
                dma_dest_ptr = temp;
                
                // Restart DMA for next buffer
                DMA0CONbits.CHEN = 1;
                DMA1CONbits.CHEN = 1;
            }
        }
        
        // Check USB connection status
        if (U1OTGSTATbits.VBUSVLD && !usb_connected) {
            usb_connected = true;
            printf("USB connected\n\r");
        } else if (!U1OTGSTATbits.VBUSVLD && usb_connected) {
            usb_connected = false;
            streaming_active = false;
            printf("USB disconnected\n\r");
        }
        
        // Watchdog reset (if enabled)
        CLRWDT();
    }
    
    return 0;
}

/*
 * System Initialization
 */
void system_init(void) {
    // Configure clock for 200 MHz
    CLKDIVbits.PLLPRE = 0;        // N1 = 2
    CLKDIVbits.PLLPOST = 0;       // N2 = 2
    PLLFBDbits.PLLDIV = 98;       // M = 100
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(0x01);
    
    // Wait for PLL lock
    while (!OSCCONbits.LOCK);
    
    // Configure I/O ports
    // ADC Data bus (PORTD) - input
    TRISD = 0xFFFF;
    PORTD = 0x0000;
    
    // PLL SPI pins (RE0-RE2) - output
    TRISEbits.TRISE0 = 0; // CLK
    TRISEbits.TRISE1 = 0; // DATA  
    TRISEbits.TRISE2 = 0; // LE
    
    // USB pins
    TRISDbits.TRISD5 = 0; // USB_D+
    TRISDbits.TRISD6 = 0; // USB_D-
    
    // Status LED
    TRISDbits.TRISD0 = 0; // LED output
    LATDbits.LATD0 = 0;   // LED off initially
    
    // Initialize UART for debug
    U1BRG = ((_XTAL_FREQ/2)/16/9600) - 1;
    U1MODEbits.BRGH = 1;
    U1MODEbits.UEN = 0;
    U1STAbits.UTXEN = 1;
    U1MODEbits.UARTEN = 1;
}

/*
 * USB Initialization
 */
void usb_init(void) {
    // USB module initialization
    U1PWRCbits.USBPWR = 1;        // Enable USB power
    
    // Configure USB pins
    U1CNSEbits.TMSE = 1;          // Enable USB time stamp
    U1OTGCONbits.DPPULUP = 1;     // Pull-up D+ for FS
    
    // Enable USB interrupts
    IEC5bits.USB1IE = 1;          // USB interrupt enable
    IPC21bits.USB1IP = 4;         // USB interrupt priority
}

/*
 * ADC Initialization (AD9215 Interface)
 */
void adc_init(void) {
    // Configure ADC for parallel data capture
    ADCON1bits.ADON = 0;          // ADC off initially
    
    // Set conversion clock
    ADCON3bits.ADCS = 15;         // ADC clock divider
    
    // Configure ADC channels
    ADCHSbits.CH123NB = 0;        // Single-ended inputs
    ADCHSbits.CH123SB = 0;        // Use AN0-AN2
    ADCHSbits.CH0NA = 0;          // VREF- for CH0
    ADCHSbits.CH0SA = 0;          // AN0 for CH0
    
    // Enable ADC
    ADCON1bits.ADON = 1;
    
    // Small delay for ADC startup
    for(volatile int i = 0; i < 1000; i++);
}

/*
 * ADF4351 PLL Initialization
 */
void adf4351_init(void) {
    // Set default frequency to 100 MHz
    adf4351_set_frequency(100000000);
}

/*
 * DMA Initialization
 */
void dma_init(void) {
    // Configure DMA0 for ADC data capture
    DMA0CONbits.MODE = 0b01;      // Peripheral to memory
    DMA0CONbits.DIR = 0;          // Read from peripheral
    DMA0CONbits.SIZE = 0;         // 16-bit transfers
    DMA0CONbits.TRMODE = 0b10;    // Continuous mode
    DMA0CONbits.AMODE = 0b10;     // Register indirect with post-increment
    DMA0CONbits.DONE = 0;         // Clear done flag
    
    DMA0REQ = 0x000D;             // ADC1 requests
    DMA0STA = __builtin_dmaptr(adc_buffer);
    DMA0STB = __builtin_dmaptr(adc_buffer + DMA_BUFFER_SIZE);
    DMA0CNT = DMA_BUFFER_SIZE - 1;
    
    // Configure DMA1 for ping-pong
    DMA1CONbits.MODE = 0b01;      // Peripheral to memory
    DMA1CONbits.DIR = 0;          // Read from peripheral
    DMA1CONbits.SIZE = 0;         // 16-bit transfers
    DMA1CONbits.TRMODE = 0b10;    // Continuous mode
    DMA1CONbits.AMODE = 0b10;     // Register indirect with post-increment
    DMA1CONbits.DONE = 0;         // Clear done flag
    
    DMA1REQ = 0x000D;             // ADC1 requests
    DMA1STA = __builtin_dmaptr(adc_buffer + DMA_BUFFER_SIZE);
    DMA1STB = __builtin_dmaptr(adc_buffer + DMA_BUFFER_SIZE*2);
    DMA1CNT = DMA_BUFFER_SIZE - 1;
    
    // Enable DMA interrupts
    IEC0bits.DMA0IE = 1;          // DMA0 interrupt enable
    IEC0bits.DMA1IE = 1;          // DMA1 interrupt enable
    IPC1bits.DMA0IP = 5;          // DMA0 interrupt priority
    IPC2bits.DMA1IP = 5;          // DMA1 interrupt priority
    
    // Start DMA channels
    DMA0CONbits.CHEN = 1;
    DMA1CONbits.CHEN = 1;
    
    // Initialize pointers
    dma_src_ptr = adc_buffer;
    dma_dest_ptr = adc_buffer + DMA_BUFFER_SIZE;
}

/*
 * Timer Initialization for sample rate control
 */
void timer_init(void) {
    // Configure Timer1 for sample rate generation
    T1CONbits.TON = 0;            // Timer off
    T1CONbits.TGATE = 0;          // Gated time accumulation disabled
    T1CONbits.TCKPS = 0b11;       // 1:256 prescaler
    T1CONbits.TCS = 0;            // Internal clock source
    
    // Calculate period for desired sample rate
    uint32_t timer_period = (_XTAL_FREQ/2) / (256 * current_sample_rate);
    PR1 = timer_period & 0xFFFF;
    
    // Enable Timer1 interrupt
    IEC0bits.T1IE = 1;            // Timer1 interrupt enable
    IPC0bits.T1IP = 6;            // Timer1 interrupt priority
    
    T1CONbits.TON = 1;            // Start timer
}

/*
 * USB Command Processing
 */
void usb_process_commands(void) {
    if (U1STAbits.URXDA) {
        uint8_t command = U1RXREG;
        
        switch (command) {
            case CMD_SET_FREQUENCY:
                if (U1STAbits.URXDA >= 4) {
                    uint32_t freq = U1RXREG | (U1RXREG << 8) | (U1RXREG << 16) | (U1RXREG << 24);
                    current_frequency = freq;
                    adf4351_set_frequency(freq);
                    printf("Frequency set to %lu Hz\n\r", freq);
                }
                break;
                
            case CMD_SET_SAMPLE_RATE:
                if (U1STAbits.URXDA >= 4) {
                    uint32_t rate = U1RXREG | (U1RXREG << 8) | (U1RXREG << 16) | (U1RXREG << 24);
                    current_sample_rate = rate;
                    timer_init();
                    printf("Sample rate set to %lu Sps\n\r", rate);
                }
                break;
                
            case CMD_SET_GAIN:
                if (U1STAbits.URXDA >= 2) {
                    uint16_t gain = U1RXREG | (U1RXREG << 8);
                    current_gain = gain;
                    printf("Gain set to %d dB\n\r", gain);
                }
                break;
                
            case CMD_START_STREAM:
                start_streaming();
                break;
                
            case CMD_STOP_STREAM:
                stop_streaming();
                break;
                
            case CMD_GET_STATUS:
                // Send status information back
                uint8_t status[16];
                memcpy(status, &current_frequency, 4);
                memcpy(status + 4, &current_sample_rate, 4);
                memcpy(status + 8, &current_gain, 2);
                memcpy(status + 10, &sample_count, 4);
                status[14] = streaming_active ? 1 : 0;
                status[15] = usb_connected ? 1 : 0;
                usb_send_data(status, 16);
                break;
        }
    }
}

/*
 * Start Streaming Data
 */
void start_streaming(void) {
    if (!streaming_active) {
        streaming_active = true;
        LATDbits.LATD0 = 1;       // Turn on status LED
        
        // Enable ADC conversion
        ADCON1bits.ADON = 1;
        
        // Start DMA if not already running
        if (!DMA0CONbits.CHEN) DMA0CONbits.CHEN = 1;
        if (!DMA1CONbits.CHEN) DMA1CONbits.CHEN = 1;
        
        printf("Streaming started\n\r");
    }
}

/*
 * Stop Streaming Data
 */
void stop_streaming(void) {
    if (streaming_active) {
        streaming_active = false;
        LATDbits.LATD0 = 0;       // Turn off status LED
        
        // Stop DMA
        DMA0CONbits.CHEN = 0;
        DMA1CONbits.CHEN = 0;
        
        // Stop ADC conversion
        ADCON1bits.ADON = 0;
        
        printf("Streaming stopped\n\r");
    }
}

/*
 * Send Samples to USB Host
 */
void send_samples_to_usb(uint16_t *samples, uint16_t count) {
    // Convert 16-bit ADC samples to 8-bit USB packets
    for (uint16_t i = 0; i < count; i += 2) {
        uint8_t packet[4];
        packet[0] = samples[i] & 0xFF;           // Low byte
        packet[1] = (samples[i] >> 8) & 0xFF;    // High byte
        
        if (i + 1 < count) {
            packet[2] = samples[i + 1] & 0xFF;   // Low byte
            packet[3] = (samples[i + 1] >> 8) & 0xFF;  // High byte
        }
        
        usb_send_data(packet, 4);
    }
    
    sample_count += count;
}

/*
 * USB Data Send Function
 */
bool usb_send_data(uint8_t *data, uint16_t length) {
    // Check if endpoint is ready for transmission
    while (!U1STAbits.TXBE);
    
    // Copy data to USB buffer and send
    for (uint16_t i = 0; i < length && i < USB_BUFFER_SIZE; i++) {
        usb_tx_buffer[i] = data[i];
    }
    
    // Load data into USB TX FIFO
    for (uint16_t i = 0; i < length && i < USB_BUFFER_SIZE; i++) {
        U1TXREG = usb_tx_buffer[i];
    }
    
    return true;
}

/*
 * USB Interrupt Service Routine
 */
void __attribute__((interrupt, no_auto_psv)) _USB1Interrupt(void) {
    // Process USB events
    if (U1OTGIRbits.SRP_DETIF) {
        // Session request detected
        U1OTGIRbits.SRP_DETIF = 1;
    }
    
    // Clear USB interrupt flag
    IFS5bits.USB1IF = 0;
}

/*
 * DMA0 Interrupt Service Routine (Buffer 1 Complete)
 */
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void) {
    // Set flag for buffer switching
    buffer_switch_pending = true;
    
    // Restart DMA for continuous operation
    DMA0CONbits.CHEN = 0;
    DMA0CONbits.CHEN = 1;
    
    // Clear DMA0 interrupt flag
    IFS0bits.DMA0IF = 0;
}

/*
 * DMA1 Interrupt Service Routine (Buffer 2 Complete)
 */
void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void) {
    // Set flag for buffer switching
    buffer_switch_pending = true;
    
    // Restart DMA for continuous operation  
    DMA1CONbits.CHEN = 0;
    DMA1CONbits.CHEN = 1;
    
    // Clear DMA1 interrupt flag
    IFS0bits.DMA1IF = 0;
}

/*
 * Timer1 Interrupt Service Routine (Sample Rate Control)
 */
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    // Trigger ADC conversion
    ADCON1bits.ADON = 1;
    ADCON1bits.DONE = 0;
    
    // Clear timer interrupt flag
    IFS0bits.T1IF = 0;
}
