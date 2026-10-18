#define ENCODER_A_PINS { GP29, GP0 }
#define ENCODER_B_PINS { GP28, GP1 }
#define ENCODER_RESOLUTIONS {4, 4}
#define ENCODER_DEFAULT_POS 0x3


// Lighting profile: base effect is black, ichor_lighting.c paints every LED on top.
#define ENABLE_RGB_MATRIX_SOLID_COLOR
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
#define RGB_MATRIX_DEFAULT_VAL 0
// #define PROFILE_DEBUG_ORDER  // uncomment to show each LED's wiring index as a hue


/*
I2C initialization

#pragma once
#define I2C_DRIVER I2CD0
#define I2C1_SDA_PIN GP4
#define I2C1_SCL_PIN GP5

#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP14
#define I2C1_SCL_PIN GP15

*/
