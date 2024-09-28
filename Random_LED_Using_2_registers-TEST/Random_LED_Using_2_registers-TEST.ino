// Class to encapsulate 74HC595 shift register operations
class ShiftRegister {
public:
    int latchPin;
    int clockPin;
    int dataPin;
    byte registerState;

    ShiftRegister(int latchPin, int clockPin, int dataPin) {
        this->latchPin = latchPin;
        this->clockPin = clockPin;
        this->dataPin = dataPin;
        this->registerState = 0;

        pinMode(latchPin, OUTPUT);
        pinMode(clockPin, OUTPUT);
        pinMode(dataPin, OUTPUT);
    }

    void writeState() {
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, registerState); // Shifts out the register state
        digitalWrite(latchPin, HIGH);
    }

    void setPin(int pin, bool state) {
        bitWrite(registerState, pin, state);
        writeState();
    }
};

// Class to handle the effects logic and loop management
class EffectController {
private:
    int effectId;
    int prevEffect;
    int effectRepeat;
    int effectSpeed;
    ShiftRegister* shiftRegisters;  // Array of shift registers
    int numRegisters;               // Number of shift registers

public:
    // Constructor takes the array of ShiftRegister objects and the number of registers
    EffectController(ShiftRegister* shiftRegisters, int numRegisters)
        : shiftRegisters(shiftRegisters), numRegisters(numRegisters), effectId(0), prevEffect(-1), effectRepeat(1), effectSpeed(500) {}

    // Function to select a new random effect, ensuring it's not the same as the last one
    void selectRandomEffect() {
        do {
            effectId = random(6); // Random effect between 0 and 5
        } while (effectId == prevEffect);
        prevEffect = effectId;
    }

    // Function to determine how many times the effect should repeat
    void determineRepetitions() {
        switch (effectId) {
            case 0:
            case 1:
            case 4:
            case 5:
                effectRepeat = random(1, 2);
                break;
            case 3:
                effectRepeat = random(1, 5);
                break;
            default:
                effectRepeat = 1; // Default to 1 repetition
        }
    }

    // Function to execute the selected effect a specified number of times
    void executeEffect() {
        for (int i = 0; i < effectRepeat; i++) {
            effectSpeed = random(10, 90); // Random speed for each execution

            switch (effectId) {
                case 0:
                    runEffectA(effectSpeed);
                    break;
                case 1:
                    runEffectB(effectSpeed);
                    break;
                case 3:
                    runEffectC(effectSpeed);
                    break;
                case 4:
                    runEffectD(effectSpeed);
                    break;
                case 5:
                    runEffectE(effectSpeed);
                    break;
                default:
                    // Handle unknown effects or do nothing
                    break;
            }
        }
    }

private:
    // Function to write to a specific pin across shift registers
    void regWrite(int pin, bool state) {
        int reg = pin / 8;  // Determine which register
        int actualPin = pin % 8;  // Get the pin within the register

        if (reg < numRegisters) {
            shiftRegisters[reg].setPin(actualPin, state);
        }
    }

    // Encapsulated effect methods (the same logic as the previous functions)
    void runEffectA(int speed) {
        for (int i = 0; i < 16; i++) {
            for (int k = i; k < 16; k++) {
                regWrite(k, HIGH);
                delay(speed);
                regWrite(k, LOW);
            }
            regWrite(i, HIGH);
        }
    }

    void runEffectB(int speed) {
        for (int i = 15; i >= 0; i--) {
            for (int k = 0; k < i; k++) {
                regWrite(k, HIGH);
                delay(speed);
                regWrite(k, LOW);
            }
            regWrite(i, HIGH);
        }
    }

    void runEffectC(int speed) {
        int prevI = 0;
        for (int i = 0; i < 16; i++) {
            regWrite(prevI, LOW);
            regWrite(i, HIGH);
            prevI = i;
            delay(speed);
        }

        for (int i = 15; i >= 0; i--) {
            regWrite(prevI, LOW);
            regWrite(i, HIGH);
            prevI = i;
            delay(speed);
        }
    }

    void runEffectD(int speed) {
        for (int i = 0; i < 8; i++) {
            for (int k = i; k < 8; k++) {
                regWrite(k, HIGH);
                regWrite(15 - k, HIGH);
                delay(speed);
                regWrite(k, LOW);
                regWrite(15 - k, LOW);
            }
            regWrite(i, HIGH);
            regWrite(15 - i, HIGH);
        }
    }

    void runEffectE(int speed) {
        for (int i = 7; i >= 0; i--) {
            for (int k = 0; k <= i; k++) {
                regWrite(k, HIGH);
                regWrite(15 - k, HIGH);
                delay(speed);
                regWrite(k, LOW);
                regWrite(15 - k, LOW);
            }
            regWrite(i, HIGH);
            regWrite(15 - i, HIGH);
        }
    }
};

// Create an array of shift registers (4 in this case)
ShiftRegister shiftRegisters[] = {
    ShiftRegister(2, 4, 3),    // 595_1
    ShiftRegister(5, 7, 6),    // 595_2
    ShiftRegister(8, 10, 9),   // 595_3
    ShiftRegister(11, 13, 12)  // 595_4
};

const int numRegisters = sizeof(shiftRegisters) / sizeof(shiftRegisters[0]);

// Create an instance of the EffectController, passing in the shift registers
EffectController effectController(shiftRegisters, numRegisters);

void setup() {
    // No additional setup is required for the effects controller
}

void loop() {
    effectController.selectRandomEffect();
    effectController.determineRepetitions();
    effectController.executeEffect();
}
