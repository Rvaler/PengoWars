#include "headers/Window.h"

Window::Window()
{
    int width = 600;
    int height = 250;
}

int getWidth() {
    return this->width;
}

int getHeight() {
    return this->height;
}

void setWidth(int x) {
    this->width = x;
}

void setHeight(int y) {
    this->height = y;
}

void onReshape(int x, int y) {
    this->width = x;
    this->height = y;
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);
}

Window::~Window()
{
    delete(this);
}
