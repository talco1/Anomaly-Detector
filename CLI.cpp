#include "CLI.h"

CLI::CLI(DefaultIO* dio):dio(dio) {
    LinkingClass* linker = new LinkingClass();
    //LinkingClass linker;
    commands[0] = new UploadCommand(dio, linker);
    commands[1] = new Command1(dio, linker);
    commands[2] = new Command2(dio, linker);
    commands[3] = new Command3(dio, linker);
    commands[4] = new Command4(dio, linker);
    commands[5] = new Command5(dio, linker);
    commands[6] = new Command6(dio, linker);
}

void CLI::start(){
    for (int i = 0; i < 7; i++) {
        dio->write(commands[i]->description);
    }
    string input = dio->read();
    while (input != "6") {
        commands[stoi(input)]->execute();
        for (int i = 0; i < 7; i++) {
            dio->write(commands[i]->description);
        }
        input = dio->read();
    }
}

CLI::~CLI() {
    for (int i = 0; i < 7; i++) {
        delete commands[i];
    }
}