#include <sstream>
#include "Cpu.h"
#include "../constants/Constants.h"
#include "../constants/OpcodeBitshifts.h"
#include "../utils/RandomUtil.h"

Cpu::Cpu(Memory &memory, Display &display, InputController &inputController) : memory(memory), display(display), inputController(inputController) {
    programCounter = Constants::MEMORY_PROGRAM_START_LOCATION;
    indexRegister = 0;
    currStackLevel = 0;

    for (int i = 0; i < NUM_GENERAL_PURPOSE_REGISTERS; i++) {
        generalPurposeRegisters[i] = 0;
    }
    //TODO: figure out how to initialize these
//    delayTimerRegister = 0;
//    soundTimerRegister = 0;
}

void Cpu::emulateCycle() {
    //TODO: change delay to actual processor clock rate
    SDL_Delay(1);
    updateTimers();
    uint16_t opcode = fetchOpCode();

    //note that not every instruction increments the program counter by 2
    //for example, a jump instruction avoids this, but since instructions are executed after this increment
    //an instruction such as the jump instruction will overwrite this increment.
    //other instructions may rely on the value of the program counter before this increment, so in some instruction
    //implementations, we have to "reverse" this increment by subtracting 2 from the program counter.
    //This happens on very few instructions, so it is better to increment here to avoid the excessive code duplication
    //of updating the program counter in every instruction implementation
    programCounter += DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;

    decodeAndExecuteOpcode(opcode);
    display.updateScreen();
}

uint16_t Cpu::fetchOpCode() {
    //use bit shifting to concatenate the contents of two 8-bit memory addresses to combine them into one 16-bit op-code
    //note that one opcode is two program instructions from memory
    return memory.getDataAtAddress(programCounter) << Constants::BITS_IN_BYTE | memory.getDataAtAddress(programCounter + 1);
}

void Cpu::decodeAndExecuteOpcode(uint16_t opcode) {
    //here we use bit shifting to interpret the first 4 bits of the op-code as an integer index into the opcode implementations array
    //forgive the "this->*" syntax; it's an unfortunate necessity to implement this with an array of member function pointers
    (this->*cpuOpcodeImplementations[getFirstNibbleFromOpcode(opcode)])(opcode);
}

void Cpu::delegateToArithmethicOpcodeImplementations(uint16_t opcode) {
    //for all arithmetic opcodes (opcodes beginning with first nibble == 8), the last nibble determines the specific arithmetic operation
    return (this->*arithmeticOpcodeImplementations[opcode & OpcodeBitmasks::LAST_NIBBLE])(opcode);
}

void Cpu::handleUnimplementedOpcode(uint16_t opcode) {
    std::stringstream errorMessage;
    errorMessage << "The specified opcode has not yet been implemented: " << opcode;
    throw InstructionUnimplementedException(errorMessage.str());
}

void Cpu::executeOpcodeBeginningWithZero(uint16_t opcode) {
    //there are multiple opcodes that start with zero, so we choose between them here
    //if there was only one opcode beginning with zero, we could have implemented a
    //function for that opcode with no switch statement similar to the below functions
    switch (opcode) {
        case Opcodes::CLEAR_DISPLAY:
            display.clearScreen();
            return;
        case Opcodes::RETURN_FROM_SUBROUTINE:
            executeReturnFromSubroutineOpcode();
            return;
        default:
            throw InstructionUnimplementedException("Opcode unimplemented. If you were trying to call the RCA 1802 program, this is intentionally unimplemented");
    }
}

void Cpu::executeReturnFromSubroutineOpcode() {
    currStackLevel--;
    programCounter = stack[currStackLevel] + DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;
}

void Cpu::executeAssignOpcode(uint16_t opcode) {
    indexRegister = opcode & OpcodeBitmasks::LAST_THREE_NIBBLES;
}

void Cpu::executeJumpOpcode(uint16_t opcode) {
    programCounter = opcode & OpcodeBitmasks::LAST_THREE_NIBBLES;
}

void Cpu::executeCallSubroutineOpcode(uint16_t opcode) {
    //Save the location of the programCounter before going to the specified subroutine, so we can return later.
    //if we didn't increment the program counter after fetching each instruction in emulateCycle(), we could just
    //set the stack to programCounter, but to "undo" this increment, we subtract programCounter by the amount added earlier
    stack[currStackLevel] = programCounter - DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;
    currStackLevel++;
    return executeJumpOpcode(opcode);
}

void Cpu::executeRegisterEqualsValueOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    int value = opcode & OpcodeBitmasks::LAST_BYTE;
    if (generalPurposeRegisters[registerNumber] == value) {
        skipInstruction();
    }
}

void Cpu::skipInstruction() {
    programCounter += DEFAULT_NUM_INSTRUCTIONS_PER_CYCLE;
}

//TODO: refactor this to reuse code in executeRegisterEqualsValueOpcode()
void Cpu::executeRegisterNotEqualsValueOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    int value = opcode & OpcodeBitmasks::LAST_BYTE;
    if (generalPurposeRegisters[registerNumber] != value) {
        skipInstruction();
    }
}

void Cpu::executeValueEqualsValueOpcode(uint16_t opcode) {
    int value1 = getSecondNibbleFromOpcode(opcode);
    int value2 = getThirdNibbleFromOpcode(opcode);
    if (value1 == value2) {
        skipInstruction();
    }
}

void Cpu::executeAssignRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumber] = (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE);
}

void Cpu::executeAddToRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumber] += (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE);
}

void Cpu::executeArithmeticSetOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberY];
}

//TODO: refactor bitwise operation instructions to reduce code duplication
void Cpu::executeArithmeticSetOrOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] | generalPurposeRegisters[registerNumberY];
}

void Cpu::executeArithmeticSetAndOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] & generalPurposeRegisters[registerNumberY];
}

void Cpu::executeArithmeticSetXOROpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] ^ generalPurposeRegisters[registerNumberY];
}

void Cpu::executeArithmeticAddOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    setAdditionOverflowRegister(registerNumberX, registerNumberY);
    //note that if this overflows, the overflowed result will start counting from zero again after the overflow occurs
    //this is c++'s default behaviour, so we don't have to do anything special to implement this
    generalPurposeRegisters[registerNumberX] += generalPurposeRegisters[registerNumberY];
}

//TODO: refactor SetSubtraction...() methods to reduce code duplication
void Cpu::setAdditionOverflowRegister(int registerNumberX, int registerNumberY) {
    if (generalPurposeRegisters[registerNumberX] + generalPurposeRegisters[registerNumberY] > Constants::MAX_BYTE_SIZE) {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 1;
    } else {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 0;
    }
}

void Cpu::executeArithmeticSubtractOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    setSubtractionXYOverflowRegisters(registerNumberX, registerNumberY);
    generalPurposeRegisters[registerNumberX] -= generalPurposeRegisters[registerNumberY];
}

void Cpu::setSubtractionXYOverflowRegisters(int registerNumberX, int registerNumberY) {
    if (generalPurposeRegisters[registerNumberX] - generalPurposeRegisters[registerNumberY] < 0) {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 0;
    } else {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 1;
    }
}

void Cpu::executeArithmeticShiftRightOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[INDEX_CARRY_REGISTER] = (uint8_t) (opcode & OpcodeBitmasks::LAST_BIT);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] >> 1;
}

void Cpu::executeArithmeticSubtractDifferenceOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    setSubtractionYXOverflowRegisters(registerNumberX, registerNumberY);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberY] - generalPurposeRegisters[registerNumberX];
}

void Cpu::setSubtractionYXOverflowRegisters(int registerNumberX, int registerNumberY) {
    if (generalPurposeRegisters[registerNumberY] - generalPurposeRegisters[registerNumberX] < 0) {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 1;
    } else {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 0;
    }
}

void Cpu::executeArithmeticShiftLeftOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[INDEX_CARRY_REGISTER] =
            (uint8_t) ((generalPurposeRegisters[registerNumberX] & OpcodeBitmasks::FIRST_BIT) >> OpcodeBitshifts::BIT_FIRST_TO_LAST);
    generalPurposeRegisters[registerNumberX] = generalPurposeRegisters[registerNumberX] << 1;
}

int Cpu::getFirstNibbleFromOpcode(uint16_t opcode) const {
    int registerNumber = (opcode & OpcodeBitmasks::FIRST_NIBBLE) >> OpcodeBitshifts::NIBBLE_FIRST_TO_LAST;
    return registerNumber;
}

unsigned int Cpu::getSecondNibbleFromOpcode(uint16_t opcode) const {
    unsigned int registerNumber = (opcode & OpcodeBitmasks::SECOND_NIBBLE) >> OpcodeBitshifts::BYTE_FIRST_TO_LAST;
    return registerNumber;
}

int Cpu::getThirdNibbleFromOpcode(uint16_t opcode) const {
    int value2 = (opcode & OpcodeBitmasks::THIRD_NIBBLE) >> OpcodeBitshifts::NIBBLE;
    return value2;
}

void Cpu::executeNotEqualsRegistersOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);
    if (generalPurposeRegisters[registerNumberX] != generalPurposeRegisters[registerNumberY]) {
        skipInstruction();
    }
}

void Cpu::executeJumpToAddressPlusRegisterOpcode(uint16_t opcode) {
    programCounter = (opcode & OpcodeBitmasks::LAST_THREE_NIBBLES) + generalPurposeRegisters[0];
}

void Cpu::executeRandomNumberOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = (uint8_t) (opcode & OpcodeBitmasks::LAST_BYTE) & RandomUtil::getRandomNumber();
}

void Cpu::executeDrawSpriteOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    int registerNumberY = getThirdNibbleFromOpcode(opcode);

    int coordinateX = generalPurposeRegisters[registerNumberX];
    int coordinateY = generalPurposeRegisters[registerNumberY];
    unsigned int spriteHeight = opcode & OpcodeBitmasks::LAST_NIBBLE;

    //default value for the carry register if no pixels are toggled off
    generalPurposeRegisters[INDEX_CARRY_REGISTER] = 0;

    for (unsigned int height = 0; height < spriteHeight; height++) {
        uint8_t pixelRow = memory.getDataAtAddress(indexRegister + height);
        //a bitmask that in binary looks like 10000000
        //we use this to read the values of the bits in the pixelRow from left to right
        uint8_t bitmask = 0x80;
        for (int width = 0; width < Display::SPRITE_WIDTH; width++) {
            //shift the bitmask over by one each iteration to evaluate the next bit of the pixelRow on the next loop iteration
            //ex: on the second iteration, (bitmask >> width) will be 01000000 and can be used to observe the value of the 2nd bit of the pixelRow
            //a non-zero value means the pixel must have been on
            bool isSpritePixelSet = (pixelRow & (bitmask >> width)) != 0;
            bool previousPixelValue = display.getPixel(coordinateX + width, coordinateY + height);
            //we set the pixel values in xor mode according to the specification of the chip-8
            bool newPixelValue = previousPixelValue ^ isSpritePixelSet;
            display.setPixel(coordinateX + width, coordinateY + height, newPixelValue);

            bool isPixelToggledOff = previousPixelValue && !newPixelValue;
            if (isPixelToggledOff) {
                generalPurposeRegisters[INDEX_CARRY_REGISTER] = 1;
            }
        }
    }
}

void Cpu::executeKeyPressedSkipOpcodes(uint16_t opcode) {
    //Note this method implements two similar but different opcodes
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    unsigned int keyNumber = generalPurposeRegisters[registerNumberX];

    switch(opcode & OpcodeBitmasks::LAST_BYTE) {
        case Opcodes::KEYPRESS_SKIP_IF_PRESSED:
            if (inputController.isKeyPressed(keyNumber)) {
                skipInstruction();
            }
            break;
        case Opcodes::KEYPRESS_SKIP_IF_NOT_PRESSED:
            if (!inputController.isKeyPressed(keyNumber)) {
                skipInstruction();
            }
            break;
        default:
            throw InstructionUnimplementedException("There is no such opcode");
    }
}

void Cpu::delegateToFOpcodeImplementations(uint16_t opcode) {
    switch (opcode & OpcodeBitmasks::LAST_BYTE) {
        case Opcodes::SET_REGISTER_TO_DELAY_TIMER:
            executeSetRegisterToDelayTimerOpcode(opcode);
            break;
        case Opcodes::BLOCK_KEY_PRESSES:
            executeBlockKeyPressesOpcode(opcode);
            break;
        case Opcodes::SET_DELAY_TIMER_TO_REGISTER:
            executeSetDelayTimerToRegisterOpcode(opcode);
            break;
        case Opcodes::SET_SOUND_TIMER_TO_REGISTER:
            executeSetSoundTimerToRegisterOpcode(opcode);
            break;
        case Opcodes::ADD_REGISTER_TO_INDEX_REGISTER:
            executeAddRegisterToIndexRegisterOpcode(opcode);
            break;
        case Opcodes::SET_SPRITE_LOCATION:
            executeSetSpriteLocationOpcode(opcode);
            break;
        case Opcodes::CONVERT_TO_BCD:
            executeConvertToBCDOpcode(opcode);
            break;
        case Opcodes::REGISTER_DUMP:
            executeRegisterDumpOpcode(opcode);
            break;
        case Opcodes::REGISTER_LOAD:
            executeRegisterLoadOpcode(opcode);
            break;
        default:
            throw InstructionUnimplementedException("Specified opcode beginning with F doesn't exist");
    }
}

void Cpu::executeSetRegisterToDelayTimerOpcode(uint16_t opcode) {
    int registerNumberX = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumberX] = delayTimerRegister;
}

void Cpu::executeBlockKeyPressesOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    generalPurposeRegisters[registerNumber] = inputController.waitForKeyPress();
}

void Cpu::executeSetDelayTimerToRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    delayTimerRegister = generalPurposeRegisters[registerNumber];
}

void Cpu::executeSetSoundTimerToRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    soundTimerRegister = generalPurposeRegisters[registerNumber];
}

void Cpu::executeAddRegisterToIndexRegisterOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    setIndexOverflowRegister(registerNumber);
    indexRegister += generalPurposeRegisters[registerNumber];
    //TODO: make sure this behaves correctly*************************************
    if (indexRegister > Constants::MAX_INDEX_REGISTER_VALUE) {
        indexRegister -= Constants::MAX_INDEX_REGISTER_VALUE;
    }
}

void Cpu::setIndexOverflowRegister(int registerNumber) {
    //note here that the indexRegister is a different size than the standard general purpose registers
    if (indexRegister + generalPurposeRegisters[registerNumber] > Constants::MAX_INDEX_REGISTER_VALUE) {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 1;
    } else {
        generalPurposeRegisters[INDEX_CARRY_REGISTER] = 0;
    }
}

void Cpu::executeSetSpriteLocationOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    uint8_t characterNumber = generalPurposeRegisters[registerNumber];
    indexRegister = Constants::MEMORY_FONT_START_LOCATION + (characterNumber * Constants::FONT_NUM_BYTES_PER_CHARACTER);
}

void Cpu::executeConvertToBCDOpcode(uint16_t opcode) {
    int registerNumber = getSecondNibbleFromOpcode(opcode);
    uint8_t numberToConvert = generalPurposeRegisters[registerNumber];

    //the least significant gets put in memory location (indexRegister + 2)
    unsigned int indexOfLeastSignificantDigit = 2;
    uint8_t digitShiftAmount = 10;

    //note: i < indexOfLeastSignificantDigit would normally be i >= 0, but the ints are unsigned, so instead of going to -1,
    //the integer will go to MAX_INT_VALUE upon decrementing at zero
    for (unsigned int i = indexOfLeastSignificantDigit; i <= indexOfLeastSignificantDigit; i--) {
        //by modding by 10, we get rid of every part of a decimal number except for the last (least significant) digit
        memory.setDataAtAddress(indexRegister + i, numberToConvert % digitShiftAmount);
        //this removes the last digit of the number, and shifts every other (decimal) digit to the right by one
        //this prepares the number for the next iteration by making the 2nd last digit the last digit,
        //so next iteration we can mod by 10 to reveal the 2nd last digit
        numberToConvert = numberToConvert / digitShiftAmount;
    }
}

void Cpu::executeRegisterDumpOpcode(uint16_t opcode) {
    unsigned int registerNumber = getSecondNibbleFromOpcode(opcode);
    for (unsigned int i = 0; i <= registerNumber; i++) {
        memory.setDataAtAddress(indexRegister + i, generalPurposeRegisters[i]);
    }
}

void Cpu::executeRegisterLoadOpcode(uint16_t opcode) {
    unsigned int registerNumber = getSecondNibbleFromOpcode(opcode);
    for (unsigned int i = 0; i <= registerNumber; i++) {
        generalPurposeRegisters[i] = memory.getDataAtAddress(indexRegister + i);
    }
}

void Cpu::updateTimers() {
    if (delayTimerRegister > 0) {
        delayTimerRegister--;
    }
    if (soundTimerRegister > 0) {
        soundTimerRegister--;
    }
}










