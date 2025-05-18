//
// Included Files
//
#include <stdio.h>
#include <math.h>



#pragma DATA_SECTION(samples,"ramgs2");

#define SAMPLING_RATE 8000     // Sampling rate in Hz (for generating the tones)
#define NUM_TARGETS 8          // Number of target frequencies (DTMF frequencies)
#define NUM_SAMPLES 205        // Number of samples for Goertzel algorithm

// Global variables
float magnitudes[NUM_TARGETS];  // Stores magnitudes calculated by the Goertzel algorithm
float samples[NUM_SAMPLES];     // Array to store generated sample values
int Flag = 1;                   // Flag to track valid/invalid key presses
char digit;                     // Holds the user input (digit) from the keypad
float targetFrequencies[NUM_TARGETS] = {697, 770, 852, 941, 1209, 1336, 1477, 1633};


// Function prototypes
void goertzel_multi(const float samples[], int numSamples, float targetFrequencies[]);
void Frequency_detection(float magnitudes[]);
void Generate_tones(char digit);

// Function to perform the Goertzel algorithm for multiple frequencies
void goertzel_multi(const float samples[], int numSamples, float targetFrequencies[]) {
    float coeff[NUM_TARGETS];
    float omega;
    float q0,q1,q2;
    int i, j;

    // Pre-compute the coefficient for each target frequency
    for (i = 0; i < NUM_TARGETS; ++i) {
        omega = (2.0 * M_PI * targetFrequencies[i]/SAMPLING_RATE);
        coeff[i] = 2.0 * cos(omega);
    }

    // Apply the Goertzel algorithm for each target frequency
    for (i = 0; i < NUM_TARGETS; ++i) {
        q1 = 0;
        q2 = 0;
        for (j = 0; j < numSamples; ++j) {
            q0 = samples[j] + coeff[i] * q1 - q2;
            q2 = q1;
            q1 = q0;
        }

        // Calculate magnitude based on the final Goertzel filter values
        //
        magnitudes[i] = q1 * q1 + q2 * q2 - coeff[i] * q1 * q2;

    }

    // Print the magnitudes for each frequency
    for (i = 0; i < NUM_TARGETS; ++i) {
        printf("Magnitude for frequency: %d Hz, magnitude: %d.%d\n", (int)targetFrequencies[i], (int)magnitudes[i],(int)(magnitudes[i]*100)%100);
    }
}
// Function to detect the key based on frequency magnitudes
void Frequency_detection(float magnitudes[]) {
    int i;
    char pad[4][4] = {  // 4x4 keypad representation (DTMF)
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };
    char result;
    float max1 = 0.0, max2 = 0.0;
    int i1 = -1, i2 = -1;

    // Find the strongest frequency in the low group (697, 770, 852, 941)
    for (i = 0; i < 4; i++) {
        if (magnitudes[i] > max1) {
            max1 = magnitudes[i];
            i1 = i;
        }
        // Find the strongest frequency in the high group (1209, 1336, 1477, 1633)
        if (magnitudes[i + 4] > max2) {
            max2 = magnitudes[i + 4];
            i2 = i;
        }
    }

    // Determine which key was pressed
    result = pad[i1][i2];

    // Print the result based on the flag
    if (Flag == 1) {
        printf("The key you pressed is: %c\n", result);  // Valid key press
    } else {
        printf("Invalid key press detected. Please try again.\n");
        Flag = 1;  // Reset flag
    }
}

// Function to generate DTMF tones for the corresponding key pressed
void Generate_tones(char digit) {
    int i = 0, f1 = 0, f2 = 0;

    // Map the key press to the corresponding DTMF frequencies
    switch (digit) {
        case '0': f1 = 941; f2 = 1335; break;
        case '1': f1 = 697; f2 = 1209; break;
        case '2': f1 = 697; f2 = 1335; break;
        case '3': f1 = 697; f2 = 1477; break;
        case '4': f1 = 770; f2 = 1209; break;
        case '5': f1 = 770; f2 = 1335; break;
        case '6': f1 = 770; f2 = 1477; break;
        case '7': f1 = 852; f2 = 1209; break;
        case '8': f1 = 852; f2 = 1335; break;
        case '9': f1 = 852; f2 = 1477; break;
        case '*': f1 = 941; f2 = 1209; break;
        case '#': f1 = 941; f2 = 1477; break;
        case 'A': f1 = 697; f2 = 1633; break;
        case 'B': f1 = 770; f2 = 1633; break;
        case 'C': f1 = 852; f2 = 1633; break;
        case 'D': f1 = 941; f2 = 1633; break;
        default : f1 = 0; f2 = 0; Flag = 0; break;  // Invalid key
    }

    // Generate the combined tones for the pressed key
    for (i = 0; i < NUM_SAMPLES; ++i) {
        samples[i] = sin(2.0 * M_PI * f1 * i / SAMPLING_RATE) + 0.5 * sin(2.0 * M_PI * f2 * i / SAMPLING_RATE);
    }
}

// Main function
int main() {
    while (1) {
        // Get user input (key press)
        printf("Type a key (0-9, A-D, *, #) and press return:\n");
        scanf("\n%c", &digit);

        // Generate tones based on the pressed key
        Generate_tones(digit);

        // Call the Goertzel algorithm to calculate the magnitudes for each target frequency
        goertzel_multi(samples, NUM_SAMPLES, targetFrequencies);

        // Detect the pressed key based on the frequency magnitudes
        Frequency_detection(magnitudes);
    }
}
