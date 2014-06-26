#ifndef WINDOW_H
#define WINDOW_H


class Window
{
    public:
        Window();
        virtual ~Window();
        int getWidth();
        int getHeight();
        void setWidth(int x);
        void setHeight(int y);


    protected:

    private:

};

#endif // WINDOW_H
