Sample code
Example based on LIS3DH digital accelerometer from STMicroelectronics[2].

// Initialize accelerometer in the host processor. It only needs to be executed one time after power up in initialization routine.

void init_ACC(void)
{
    // configurations for control registers
    Write 27h into CTRL_REG1;      // Turn on the sensor with ODR = 10Hz normal mode.
    Write 01h into CTRL_REG2;      // High-pass filter (HPF) enabled with 0.2Hz cut-off                         frequency for INT1 (AOI1) interrupt generation only.
    Write 40h into CTRL_REG3;      // ACC AOI1 interrupt signal is routed to INT1 pin.
    Write 88h into CTRL_REG4;      // Full Scale = +/-2 g with BDU and HR bits enabled.
    Write 00h into CTRL_REG5;      // Default value. Interrupt signals on INT1 pin is not                         latched. Users don’t need to read the INT1_SRC                         register     to clear the interrupt signal.
   
    // configurations for wakeup and motionless detection
    Write 08h into INT1_THS;          // Threshold (THS) = 8LSBs * 15.625mg/LSB = 125mg.
    Write 32h into INT1_DURATION;      // Duration = 50LSBs * (1/10Hz) = 5s.
    Write 95h into INT1_CFG;          // Enable XLIE, YLIE and ZLIE interrupt generation,                         AND logic. It means that the interrupt will be generated                         when X and Y and Z axis acceleration is within the                         ±THS threshold simultaneously. 
}

INT1_CFG configuration is the same as freefall event detection. The differences are (1) Freefall interrupt doesn’t need to enable the HPF; (2) Freefall duration is in milliseconds range. 5 seconds duration means the freefall distance is about 122.5 meters which rarely happens to a handheld device.
