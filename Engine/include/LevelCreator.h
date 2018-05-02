#ifndef LEVELCREATOR_H
#define LEVELCREATOR_H

#include <GLScene.h>
#include <iostream>

class LevelCreator : public GLScene
{
    public:
        LevelCreator();
        virtual ~LevelCreator();

        // Initialize the openGL rendering for this scene
        GLint initGL();

        // Renders this scene to the window
        GLint drawGLScene();

        // Loads scenes from this scene
        void LoadScene(string sceneName);

        // Handles keyboard input
        int windowsMsg(HWND,UINT, WPARAM,LPARAM);

    protected:

    private:
        bool killGame;

        // used for minimizing and restoring the console window when input is needed
        HWND consoleWindow;

        bool mouseDown; // tracks if mouse is held down.
        double prevMouseX, prevMouseY; // used for tracking mouse motion
        double mouseMoveSpeed, mouseIncrement;

        vector<Model*> platforms;
        vector<Model*> enemies; // enemies will be models since we only need x, y positions
        Model* player; // Player only needs to be a model for the level creator. We'll grab the positions for the actual game scene
        Parallax* background; // background image for menu.
        Model* selectedModel; // user selects this with the mouse and can then manipulate the model
        Model* nextLevelTrigger; // allows user to move and scale the nextLevelTrigger

        double backgroundScaleX, backgroundScaleY;

        // will be used for moving the camera around the scene
        double cameraPosX, cameraPosY, cameraSpeed, cameraMoveIncrement;

        // these strings represent the relative file paths to the folders containing their textures
        string backgroundRelativeFilePath, platformRelativeFilePath, playerRelativeFilePath, enemyRelativeFilePath;

        // Screen width and height in pixels
        double screenWidth, screenHeight;

        // flag to lock user input while waiting for input from console
        bool waitingForConsoleInput;

        // Initializes the textures and vertices for the UI Elements in the scene
        void InitModels();

        float zPosButtonUI; // used for setting z position of all UI Button elements

        // draws UI buttons
        void DrawButtons();


        // Gets filename from input and instantiates a background model
        void CreateBackground();

        // Parses filename from input and instantiates a new platform model
        void CreatePlatform();

        // Moves platform to the position the user clicked on screen
        void MoveObject(double mouseX, double mouseY);

        // Places player
        void CreatePlayer();

        // Places an enemy
        void CreateEnemy();

        // Places a level trigger in the scene
        void CreateLevelTrigger();

        // gets filename from user input. relativePath will be passed by the CreateBackground, CreatePlatform, etc. functions
        string GetFilenameFromInput();

        // Modifies the scale of the currently selected model in the scene. Passing a negative value to either parameter will decrease the associated scale. Passing a positive value will increase the associated scale.
        void ModifyModelDimensions(double widthScale, double heightScale);

        // Moves the camera position.
        void MoveCamera(double xMove, double yMove);

        // Brings console window to front and activates it for user input
        void ShowConsoleWindow();

        // Sets colors on previous selected model and new selected model and updates the selectedModel pointer.
        void SetSelectedModel(Model* newModel);

        // Checks pointer collision against a model and selects it if collision happens
        void SelectModel(double mouseX, double mouseY);
};


#endif // LEVELCREATOR_H
