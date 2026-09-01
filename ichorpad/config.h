#define ENCODER_A_PINS { GP29, GP0 }
#define ENCODER_B_PINS { GP28, GP1 }
#define ENCODER_RESOLUTIONS { 4}
#define ENCODER_DEFAULT_POS 0x3

// I2C driver + pins (confirmed working via Arduino test)
#pragma once
#define I2C_DRIVER I2CD0
#define I2C1_SDA_PIN GP4
#define I2C1_SCL_PIN GP5

// OLED-specific config

