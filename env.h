#ifndef envs

#define envs

// #define env1

#ifdef env1
namespace ENV {
    const float VOLT_MAX = 3.3;
    const int ANALOG_WIDTH = 11;

    const int RESET = 32;
    const int LEVEL = 34;
    const int BTN = 18;
}
#endif

#ifndef env1

#define env2
namespace ENV {
    const int RST = 12;
    const int TG = 13;
    const int S1 = 27;
    const int S2 = 26;
    const int PC = 14;
    // const int RS = 6;

    const int RESET = RST;
    const int LEVEL = 34;
    const int BTN = 18;

    const float VOLT_MAX = 3.3;
    const int ANALOG_WIDTH = 11;
}

#endif

#endif