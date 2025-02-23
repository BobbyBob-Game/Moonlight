#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED
#include "defs.h"
#include <string>
//widget is a double-linked-list -> can go back and forth 
class Widget {
    private:
    std::string name; //identify
    int x, y; //coordinates
    char label[MAX_NAME_LENGTH]; //text displaying
    Widget* prev;
    Widget* next;

    public:
    //Constructor:
    Widget(std::string a_name){
        name = a_name;
        prev = next = nullptr;
    } //each time a new 'button' is added, it's get inserted at the end of the linked list
    void addButton(Widget button); //add button to the list of buttons

};


#endif