// updated 2/7/18

#ifndef GLSCENE_H
#define GLSCENE_H

#include <windows.h>
#include <glut.h>
#include <iostream>
#include <vector>
#include <Model.h>
<<<<<<< HEAD
=======
#include <Inputs.h>
<<<<<<< HEAD
>>>>>>> master
=======
#include <unordered_map>
#include <AudioSource.h>
#include <AudioEngine.h>
#include <Enemy.h>
#include <MeleeEnemy.h>
>>>>>>> master

using namespace std;

class GLScene
{
    public:
        GLScene();
        virtual ~GLScene();
        GLint initGL();
        GLint drawGLScene();
        GLvoid resizeGLScene(GLsizei, GLsizei);

        int windowsMsg(HWND, UINT, WPARAM, LPARAM);

        WPARAM wParam;
        float screenHeight, screenWidth;

        static vector<Model*> movableObjects; // only moving objects will check for collision
        static vector<Model*> staticObjects; // environmental, non-moving objects don't need to check for collision
<<<<<<< HEAD
<<<<<<< HEAD

=======
        static Inputs *keyboardAndMouse;

        static void UpdateModelPositions();
>>>>>>> master
=======
        static vector<Enemy*> enemies;
        static Inputs *keyboardAndMouse;

        // Sets level state to loaded, which will set GLScene loaded boolean.
        void SetLoaded(bool newState);

>>>>>>> master
    protected:
        unordered_map<string, AudioSource*> audioSources;
        Player* player;
        AudioEngine* audioEngine;
        bool isLoaded;
        AudioSource* BGM;


    private:
        DeltaTime* dTime;
        Enemy* testEnemy;
};

#endif // GLSCENE_H
