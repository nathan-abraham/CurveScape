#pragma once

class App {
private:
    int argc;
    char** argv;

public:
    App(int, char*[]);

    void run();
};
