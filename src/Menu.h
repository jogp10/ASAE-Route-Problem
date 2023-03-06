#ifndef ASAE_MENU_H
#define ASAE_MENU_H

#include "ASAE.h"

class Menu {
    ASAE asae;

    /**
     * Menu to choose between asking for stops or coordinates
     */
    void procedure();

public:
    /**
     * ASAE Menu
     * @param asae ASAE, bus service provider
     */
    Menu(ASAE asae);
};


#endif //ASAE_MENU_H
