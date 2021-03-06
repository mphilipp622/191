#include "MainMenu.h"
#include <GLLight.h>
#include <LevelCreator.h>

MainMenu::MainMenu()
{
    //ctor
    screenHeight = GetSystemMetrics(SM_CYSCREEN); // get x size of screen
    screenWidth = GetSystemMetrics(SM_CXSCREEN); // get y size of screen

    sceneName = "MainMenu";

    // Add main menu to the Scene Manager
    SceneManager::scenes.insert( {"MainMenu", this} );
    SceneManager::activeScene = "MainMenu";

    // Initialize the audio engine. Will be used by other classes
    audioEngine = new AudioEngine();

    BGM = new AudioSource("Music", "Audio/Music/BGM/DarkSong.ogg",0, 0, .6, true);
    BGM->PlayMusic();

    killGame = false;
    showCredits = false;
    showHowToPlay = false;
    showLevelSelect = false;

    zPosButtonUI = 0;
}

MainMenu::~MainMenu()
{
    //dtor
}

GLint MainMenu::initGL()
{
    // Initialize openGL renderer for this scene

    glShadeModel(GL_SMOOTH); // Shading mode
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // set background color to black
    glClearDepth(1.0f); // depth buffer
    glEnable(GL_DEPTH_TEST); // test to see what's in front and what's in back
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_COLOR_MATERIAL); // allows texture to have color
    GLLight Light(GL_LIGHT0);

    InitModels(); // initialize UI Elements

    return 1;
}

void MainMenu::InitModels()
{
    // instantiate UI elements
//    background = new Parallax();
    splashImage = new Model(4.0, 5.0, -2.0, 0, "Splash", "Splash");
    startGame = new Model(2.0, 0.5, 1.5, 1.5, "NewGameButton", "Button");
    levelCreator = new Model(2.0, 0.5, 1.5, 0.8, "LevelCreatorButton", "Button");
    howToPlay = new Model(2.0, 0.5, 1.5, 0.1, "HowToPlayButton", "Button");
    credits = new Model(2.0, 0.5, 1.5, -0.6, "CreditsButton", "Button");
    exit = new Model(2.0, 0.5, 1.5, -1.3, "ExitButton", "Button");
    backArrow = new Model(0.5, 0.4, 4.0, -1.9, "BackButton", "Button");
    howToPlayScreen = new Model(4.5, 5.0, 2.2, 0, "HowToPlayScreen", "UI");
    creditsScreen = new Model(3.5, 3.5, 2.0, 0, "CreditsScreen", "UI");
    titleImage = new Model(2.0, 0.4, 1.5, 2.15, "TitleImage", "UI");

    for(int i = 0; i < 9; i++)
    {
        levelButtons.push_back(new Model(1.5, 0.4, 1.5, 2.0 - (i * 0.45), "Level" + to_string(i + 1), "LevelButton"));
        levelButtons.back()->InitModel("Images/UI/Level" + to_string(i + 1) + ".png", true);
    }


    // Bind textures for UI elements
//    background->ParallaxInit("Images/UI/Splashscreen.png");
    startGame->InitModel("Images/UI/NewGame.png", true);
    exit->InitModel("Images/UI/Exit.png", true);
    levelCreator->InitModel("Images/UI/LevelCreator.png", true);
    howToPlay->InitModel("Images/UI/HowToPlay.png", true);
    credits->InitModel("Images/UI/Credits.png", true);
    howToPlayScreen->InitModel("Images/UI/HowToPlayScreen.png", true);
    creditsScreen->InitModel("Images/UI/CreditsScreen.png", true);
    backArrow->InitModel("Images/UI/BackArrow.png", true);
    splashImage->InitModel("Images/UI/Splashscreen.png", true);
    titleImage->InitModel("Images/UI/RockBotTitle.png", true);
}

GLint MainMenu::drawGLScene()
{
    // Main loop. Render openGL elements to window every frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	gluLookAt(0, 0, 6,
            0, 0, 0,
            0.0f, 1.0f, 0.0f);

    if(killGame) // exit game
        return 0;

//    glPushMatrix();
//    glScaled(6.66, 6.66, 1.0); // Scale the background image
//    background->DrawSquare(screenWidth, screenHeight);
//    glPopMatrix();

    splashImage->DrawModel();

    if(!showCredits && !showHowToPlay && !showLevelSelect)
        DrawButtons();

    if(showHowToPlay)
    {
        howToPlayScreen->DrawModel();
        backArrow->DrawModel();
    }

//
    if(showCredits)
    {
        creditsScreen->DrawModel();
        backArrow->DrawModel();
    }

    if(showLevelSelect)
    {
        for(auto& button : levelButtons)
            button->DrawModel();
        backArrow->DrawModel();
    }


//    for(auto& button : maps)
//        // draw all the UI elements
//        button->DrawModel();

    return 1;
}

int MainMenu::windowsMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    if(uMsg == WM_LBUTTONDOWN)
    {
        double mouseX, mouseY;

        ConvertMouseToWorld(LOWORD(lParam), HIWORD(lParam), mouseX, mouseY);

        if(!showCredits && !showHowToPlay && !showLevelSelect)
        {
            if(CheckPointerCollision(exit, mouseX, mouseY))
                killGame = true;
            else if(CheckPointerCollision(startGame, mouseX, mouseY))
                showLevelSelect = true;
    //            SceneManager::LoadScene("Level1");
            else if(CheckPointerCollision(levelCreator, mouseX, mouseY))
                SceneManager::LoadScene("LevelCreator");
            else if(CheckPointerCollision(howToPlay, mouseX, mouseY))
                showHowToPlay = true;
            else if(CheckPointerCollision(credits, mouseX, mouseY))
                showCredits = true;
        }
        else
        {
             if(showLevelSelect)
            {
                // check for pointer collision on level buttons

                for(auto& button : levelButtons)
                {
                    if(CheckPointerCollision(button, mouseX, mouseY))
                        SceneManager::LoadScene(button->GetName());
                }
            }
            if(CheckPointerCollision(backArrow, mouseX, mouseY) && (showCredits || showHowToPlay || showLevelSelect))
            {
                showCredits = false;
                showHowToPlay = false;
                showLevelSelect = false;
            }
        }


    }
    if(uMsg == WM_KEYDOWN)
    {
        // Handle keyboard input. User can select options 0 - 9. Hex values represent numbers 0 - 9 at top of keyboard

        const int oneKey = 0x31, twoKey = 0x32, threeKey = 0x33, fourKey = 0x34, fiveKey = 0x35, sixKey = 0x36, sevenKey = 0x37, eightKey = 0x38, nineKey = 0x39;

        if(!showCredits && !showHowToPlay && !showLevelSelect)
        {
            if(wParam == oneKey || wParam == VK_NUMPAD1)
                showLevelSelect = true;
            else if(wParam == twoKey || wParam == VK_NUMPAD2)
                SceneManager::LoadScene("LevelCreator");
            else if(wParam == threeKey || wParam == VK_NUMPAD3)
                showHowToPlay = true;
            else if(wParam == fourKey || wParam == VK_NUMPAD4)
                showCredits = true;
            else if(wParam == fiveKey || wParam == VK_NUMPAD5)
                killGame = true;
        }
        else
        {
             if(showLevelSelect)
            {
                // check for pointer collision on level buttons

                if(wParam == oneKey || wParam == VK_NUMPAD1)
                    SceneManager::LoadScene("Level1");
                else if(wParam == twoKey || wParam == VK_NUMPAD2)
                    SceneManager::LoadScene("Level2");
                else if(wParam == threeKey || wParam == VK_NUMPAD3)
                    SceneManager::LoadScene("Level3");
                else if(wParam == fourKey || wParam == VK_NUMPAD4)
                    SceneManager::LoadScene("Level4");
                else if(wParam == fiveKey || wParam == VK_NUMPAD5)
                    SceneManager::LoadScene("Level5");
                else if(wParam == sixKey || wParam == VK_NUMPAD6)
                    SceneManager::LoadScene("Level6");
                else if(wParam == sevenKey || wParam == VK_NUMPAD7)
                    SceneManager::LoadScene("Level7");
                else if(wParam == eightKey || wParam == VK_NUMPAD8)
                    SceneManager::LoadScene("Level8");
                else if(wParam == nineKey || wParam == VK_NUMPAD9)
                    SceneManager::LoadScene("Level9");
            }
            if(wParam == VK_BACK)
            {
                // if user presses backspace, they can go back to the last menu
                showCredits = false;
                showHowToPlay = false;
                showLevelSelect = false;
            }
        }
    }

	return 1;
}

void MainMenu::DrawButtons()
{
    startGame->DrawModel();
    exit->DrawModel(); // render exit button
    levelCreator->DrawModel();
    credits->DrawModel();
    howToPlay->DrawModel();
    titleImage->DrawModel();
}
