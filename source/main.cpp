#include <fcntl.h>
#include <semaphore.h>

#include <exception>
#include <iostream>

#include "levels/gamelevel.h"
#include "levels/terrain2deditor.h"
#include "levels/testlevel.h"

constexpr char semaphore_identifier[] = "znakeq";

using namespace std;

int main() {
    using namespace RoninEngine;
    sem_t* sem;
    setlocale(LC_ALL, "");
    if ((sem = sem_open(semaphore_identifier, 0)) == SEM_FAILED) {
        sem = sem_open(semaphore_identifier, O_CREAT | O_EXCL, 0644, 1);
        if (sem == SEM_FAILED) {
            Application::fail(strerror(errno));
            return EXIT_FAILURE;
        }
    } else {
        Application::show_message("The application has already been launched.");
        return EXIT_FAILURE;
    }

    // goto err500;
    Application::Init(1366, 768);

    GameLevel* level = new GameLevel;
    Application::LoadLevel(level);

    Application::Simulate();

    delete level;

    Application::Quit();

    sem_close(sem);
    sem_unlink(semaphore_identifier);

    return EXIT_SUCCESS;
}
