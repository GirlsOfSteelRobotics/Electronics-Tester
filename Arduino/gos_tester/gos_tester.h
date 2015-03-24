/* Define some constants to represent the various screens that can be
 * shown in the interface. Used when recording the current or previous
 * state.
 */
enum ScreenState {
    SCR_NONE,
    SCR_START,
    SCR_MAINSCR,
      SCR_CABLESEL,
        SCR_CABLETEST,
      SCR_SENSORS,
	SCR_GYRO,
	SCR_ACCEL,
	SCR_LIMITSW,
	SCR_HALLEFFECT,
	SCR_ENCODER,
        SCR_LIDAR,
      SCR_MOTOR,
        SCR_JAGUAR,
        SCR_SPIKE,
        SCR_VICTOR,
};

/* For each button we draw on the screen, define a constant to serve
 * as its ID.  Note that the buttonReleased() function returns 0 to
 * indicate no button was pressed, so skip over that value.
 */
enum ButtonIds {
  BTN_None_Pressed = 0,
  BTN_BACK,
  BTN_HELP,
  BTN_CABLES,
  BTN_SENSORS,
  BTN_MOTOR,
  BTN_CABLE1,
  BTN_CABLE2,
  BTN_CABLE3,
  BTN_CABLE4,
  BTN_CABLE5,
  BTN_GYRO,
  BTN_ACCEL,
  BTN_LIMITSW,
  BTN_HALLEFFECT,
  BTN_ENCODER,
  BTN_LIDAR,
  BTN_JAGUAR,
  BTN_SPIKE,
  BTN_VICTOR,
  BTN_RESET,
  BTN_CYCLE,
  BTN_UP,
  BTN_DOWN,
  BTN_FORWARD,
  BTN_REVERSE,
  BTN_OFF,
  BTN_BOTH,
  BTN_RATETEMP,
};

void draw_start();
enum ScreenState loop_start();
void draw_mainscr();
enum ScreenState loop_mainscr();
void draw_cablesel();
enum ScreenState loop_cablesel();
void draw_cabletest();
enum ScreenState loop_cabletest();
void draw_sensors();
enum ScreenState loop_sensors();
void draw_gyro();
enum ScreenState loop_gyro();
void draw_accel();
enum ScreenState loop_accel();
void draw_limitsw();
enum ScreenState loop_limitsw();
void draw_halleffect();
enum ScreenState loop_halleffect();
void draw_encoder();
enum ScreenState loop_encoder();
void draw_lidar();
enum ScreenState loop_lidar();
void draw_motor();
enum ScreenState loop_motor();
void draw_jaguar();
enum ScreenState loop_jaguar();
void draw_spike();
enum ScreenState loop_spike();
void draw_victor();
enum ScreenState loop_victor();
