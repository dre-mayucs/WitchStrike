#define RIGHT 1
#define NOMAL 0
#define LEFT -1

//Color
#define BLACK GetColor(0, 0, 0)
#define WHITE GetColor(255, 255, 255)
#define RED GetColor(255, 0, 0)
#define MAGENTA GetColor(255, 0, 255)
#define PURPLE GetColor(190, 0, 255)
#define GREEN Getcolor(0, 255, 0)
#define YELLO GetColor(255, 255, 0)
#define BLUE GetColor(0, 0, 255)
#define SKY_BLUE GetColor(0, 255, 255)

//key config
#define BULLET_ONE_SHOT keys[KEY_INPUT_SPACE] == true && oldkeys[KEY_INPUT_SPACE] != true
#define BULL_MULTI_SHOT keys[KEY_INPUT_SPACE] == true
#define BULLET_MULTI_SHOT keys[KEY_INPUT_SPACE] == true
#define WASD_ALL_KEY_OFF keys[KEY_INPUT_W] == false && keys[KEY_INPUT_S] == false && keys[KEY_INPUT_A] == false && keys[KEY_INPUT_D] == false