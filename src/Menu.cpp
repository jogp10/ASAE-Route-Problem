#include "Menu.h"

#include <iostream>
#include <utility>
#include <string>

using namespace std;

Menu::Menu(ASAE asae) {
    this->asae = move(asae);

    int choice = 0;

    cout << "1) Simulated Annealing" << endl;
    cout << "2) Hill Climbing" << endl;
    cout << "Choice: "; cin >> choice;

    switch(choice) {
        case 1:
            procedure();
            break;
        case 2:
            procedure();
            break;
        default:
            return;
    }
}


void Menu::procedure() {
    int choice;

    asae.toRead();

    cout << "1) " << endl;
    cout << "2) " << endl;
    cout << "Option: "; cin >> choice;

    switch(choice) {
        case 1:
            break;
        case 2:
            break;
        default:
            return;
    }
}
