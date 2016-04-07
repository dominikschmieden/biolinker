#define BAUD 115200
#define MAX_BUF 64 // serial input buffer size

// distance sensors
#define TRIGGER_X 33
#define ECHO_X 32
#define TRIGGER_Y 37
#define ECHO_Y 36
#define TRIGGER_Z 26
#define ECHO_Z 27
#define MAX_SENSING_DISTANCE 300 // mm, read values from sensors above this will be ignored
#define MEDIAN_READS 7 // number of measurements a distance sensor uses to calculate a distance (median of those values)

// pumps
#define PUMP1_ADDRESS 8 // for inter-arduino communication

// motors
#define XMOTOR_PORT 2
#define YMOTOR_PORT 1
#define ZMOTOR_PORT 4

// addresses in persistent memory of arduino
// values stored must be int between 0 and 255
#define ADDR_ZEROX 0
#define ADDR_ZEROY 1
#define ADDR_ZEROZ 2
#define ADDR_MAXX 3
#define ADDR_MAXY 4
#define ADDR_MAXZ 5
#define ADDR_PUMP1_SPEED 6
#define ADDR_PUMP1_DIRECTION 7
#define ADDR_PUMP2_SPEED 8
#define ADDR_PUMP2_DIRECTION 9
#define ADDR_XMOTOR_SPEED 10
#define ADDR_YMOTOR_SPEED 11
#define ADDR_ZMOTOR_SPEED 12
