/*
 * File:   main.c
 * Author: mukesh
 *
 * Created on July 24, 2024, 7:21 PM
 */
#include <xc.h>

#define _XTAL_FREQ 20000000 // Define the system operating frequency

// Configuration bits
#pragma config FOSC = HS    // High-speed Oscillator
#pragma config WDTE = OFF   // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF  // Power-up Timer Enable (PWRT disabled)
#pragma config BOREN = ON   // Brown-out Reset Enable (BOR enabled)
#pragma config LVP = OFF    // Low Voltage In-Circuit Serial Programming Disable (RB3 pin has digital I/O)
#pragma config CPD = OFF    // Data EEPROM Memory Code Protection (Data EEPROM code protection off)
#pragma config WRT = OFF    // Flash Program Memory Write Enable (Write protection off)
#pragma config CP = OFF     // Flash Program Memory Code Protection (Code protection off)

// Function to initialize the ADC
void ADC_Init() {
    ADCON0 = 0x41; // ADC enable, AN0 selected
    ADCON1 = 0x8E; // Right justify result, Fosc/32, AN0 analog, others digital
}

// Function to read ADC value from AN0
unsigned int ADC_Read() {
    ADCON0bits.GO_DONE = 1; // Start conversion
    while (ADCON0bits.GO_DONE); // Wait for conversion to complete
    return (ADRESH << 8) | ADRESL; // Return 10-bit result
}

// Lookup table for seven-segment display digits (common anode)
const unsigned char seven_seg_digits[10] = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80, // 8
    0x90  // 9
};

// Function to display a digit on the seven-segment display
void display_digit(unsigned char digit) {
    PORTB = seven_seg_digits[digit];
}

void main() {
    unsigned int adcValue;
    float voltage, inputVoltage;
    unsigned char digit1, digit2, digit3;
    const float R1 = 190000.0; // Ohms
    const float R2 = 10000.0;  // Ohms
    const float referenceVoltage = 5.0; // ADC reference voltage

    ADC_Init(); // Initialize ADC
    TRISB = 0x00; // Set PORTB as output (for seven-segment display)
    
    while (1) {
        adcValue = ADC_Read(); // Read ADC value from AN0
        voltage = adcValue * (referenceVoltage / 1023.0); // Convert ADC value to voltage
        inputVoltage = voltage * ((R1 + R2) / R2); // Calculate input voltage

        // Convert inputVoltage to individual digits
        digit1 = (unsigned char)inputVoltage / 10; // Tens place
        digit2 = (unsigned char)inputVoltage % 10; // Ones place
        digit3 = (unsigned char)(inputVoltage * 10) % 10; // Tenths place

        // Display the digits sequentially on the seven-segment display
        display_digit(digit1);
        __delay_ms(5); // Short delay for multiplexing
        display_digit(digit2);
        __delay_ms(5); // Short delay for multiplexing
        display_digit(digit3);
        __delay_ms(5); // Short delay for multiplexing
    }
}