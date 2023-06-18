#define USE_SINGLE_RUN 0
#if USE_SINGLE_RUN
#include <cstring>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#endif

#include <exception>
#include <iostream>

#include "levels/gamelevel.h"
// #include "levels/terrain2deditor.h"
// #include "levels/testlevel.h"

using namespace std;

#if USE_SINGLE_RUN
constexpr char semaphore_identifier[] = "roninengine.znakeq";
sem_t* sem;
void signal_out(int)
{
    if (sem)
        sem_unlink(semaphore_identifier);q
    exit(EXIT_FAILURE);
}
#endif
#if WIN32
typedef void* HINSTANCE;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
    using namespace RoninEngine;
    setlocale(LC_ALL, "");
#if USE_SINGLE_RUN
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
#endif

    RoninSimulator::init();

    Resolution res { 1024, 600 };
    RoninSimulator::show(res, false);

    auto world = new AppleEatGameLevel;

    RoninSimulator::load_world(world);

    RoninSimulator::simulate();

    delete world;

    RoninSimulator::utilize();

#if USE_SINGLE_RUN
    sem_close(sem);
    sem_unlink(semaphore_identifier);
#endif

    return EXIT_SUCCESS;
}
