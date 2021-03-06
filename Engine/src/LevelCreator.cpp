#include "LevelCreator.h"

#include <fstream>
#include <GLLight.h>

LevelCreator::LevelCreator()
{
    //ctor
    screenHeight = GetSystemMetrics(SM_CYSCREEN); // get x size of screen
    screenWidth = GetSystemMetrics(SM_CXSCREEN); // get y size of screen

    sceneName = "LevelCreator";

    // Add main menu to the Scene Manager
    SceneManager::scenes.insert( {"LevelCreator", this} );
    SceneManager::activeScene = "LevelCreator";

    zPosButtonUI = 0;
    killGame = false;

    backgroundRelativeFilePath = "Images/Backgrounds/";
    platformRelativeFilePath = "Images/Platforms/";
    playerRelativeFilePath = "Images/Player/";
    enemyRelativeFilePath = "Images/Enemies/";
    playerTexturePath = "Images/Player/Idle_0000.png";

    waitingForConsoleInput = false;
    mouseDown = false;

    backgroundScaleX = 12.0;
    backgroundScaleY = 12.0;

    cameraPosX = 0;
    cameraPosY = 0;
    cameraPosZ = 6;
    cameraSpeed = 4.0;
    cameraMoveIncrement = 0.4; // this is the base unit that the camera will move when user presses WASD

    consoleWindow = GetConsoleWindow();

    background = nullptr;
    player = nullptr;
    selectedModel = nullptr;
    nextLevelTrigger = nullptr;
}

LevelCreator::~LevelCreator()
{
    //dtor
}



//////////////////////
// OPENGL FUNCTIONS //
//////////////////////

GLint LevelCreator::initGL()
{

    glShadeModel(GL_SMOOTH); // Shading mode
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // set background color to black
    glClearDepth(1.0f); // depth buffer
    glEnable(GL_DEPTH_TEST); // test to see what's in front and what's in back
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_COLOR_MATERIAL); // allows texture to have color
    GLLight Light(GL_LIGHT0);

    dTime = new DeltaTime();
}

void LevelCreator::InitModels()
{

}

void LevelCreator::DrawButtons()
{

}

GLint LevelCreator::drawGLScene()
{
    // Main loop. Render openGL elements to window every frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
            cameraPosX, cameraPosY, 0,
            0.0f, 1.0f, 0.0f);

    if(killGame) // exit game
        return 0;

    if(background)
    {
        glEnable(GL_TEXTURE_2D);
        glPushMatrix();
        glScaled(backgroundScaleX, backgroundScaleY, 1.0); // Scale the background image
        background->DrawSquare(screenWidth, screenHeight);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    }

    for(auto& platform : platforms)
        platform->DrawModel();

    if(enemies.size() > 0)
    {
        for(auto& enemy : enemies)
            enemy->DrawModel();
    }

    for(auto& trigger : triggers)
        trigger->DrawModel();

    for(auto& healthPack : healthPacks)
        healthPack->DrawModel();

    if(player)
    {
        player->DrawModel();
    }

    if(nextLevelTrigger)
        nextLevelTrigger->DrawModel();

    dTime->UpdateDeltaTime();

    return 1;
}



void LevelCreator::LoadScene(string sceneName)
{


}




//////////////////////////////////////////////
//         KEYBOARD AND MOUSE INPUT         //
//////////////////////////////////////////////

int LevelCreator::windowsMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const int bKey = 0x42, wKey = 0x57, sKey = 0x53, aKey = 0x41, dKey = 0x44, mKey = 0x4D, pKey = 0x50, eKey = 0x45, tKey = 0x54, hKey = 0x48;

    if(uMsg == WM_KEYDOWN)
    {
        if(waitingForConsoleInput)
            return 1; // prevent input while we're waiting for console input


        ////////////////////////
        //  BACKGROUND CREATION
        ////////////////////////
        if(wParam == bKey)
        {
            ShowConsoleWindow(); // pop up the console window for input.

            CreateBackground();

            SetForegroundWindow(hWnd); // set the game window back to front
        }


        ////////////////////////////////
        // Model Dimension Modification
        ////////////////////////////////

        else if(wParam == VK_ADD)
            // '+' key will increase model's x scale
            ModifyModelDimensions(1.0, 0);
        else if(wParam == VK_SUBTRACT)
            // '-' key will decrease model's x scale
            ModifyModelDimensions(-1.0, 0);
        else if(wParam == VK_UP)
            // Up Arrow Key will increase model's Y scale
            ModifyModelDimensions(0, 1.0);
        else if(wParam == VK_DOWN)
            // Down Arrow key will decrease model's Y scale
            ModifyModelDimensions(0, -1.0);


        ////////////////////
        //  CAMERA MOVEMENT
        ////////////////////

        else if(wParam == wKey)
            // Move camera up
            MoveCamera(0, 1.0);
        else if(wParam == sKey)
            // Move camera down
            MoveCamera(0, -1.0);
        else if(wParam == aKey)
            // Move camera left
            MoveCamera(-1.0, 0);
        else if(wParam == dKey)
            // Move camera right
            MoveCamera(1.0, 0);


        ////////////////////
        // PLATFORM CREATION
        ////////////////////
        else if(wParam == mKey)
        {
            ShowConsoleWindow();

            CreatePlatform();

            SetForegroundWindow(hWnd); // set the game window back to front
        }


        //////////////////
        // PLAYER CREATION
        //////////////////
        else if(wParam == pKey && !player)
            CreatePlayer(); // if player doesn't exist, create him


        /////////////////
        // ENEMY CREATION
        /////////////////
        else if(wParam == eKey && !selectedModel)
        {
            ShowConsoleWindow();

            CreateEnemy();

            SetForegroundWindow(hWnd); // set the game window back to front
        }

        /////////////////////////
        // LEVEL TRIGGER CREATION
        /////////////////////////

        else if(wParam == 0x4C)
        {
            ShowConsoleWindow();
            CreateLevelTrigger();
            SetForegroundWindow(hWnd); // set the game window back to front
        }


        /////////////////////////
        // TEXT TRIGGER CREATION
        /////////////////////////

        else if(wParam == tKey)
        {
            ShowConsoleWindow();

            CreateTextTrigger();

            SetForegroundWindow(hWnd); // set the game window back to front
        }

        ////////////////////////
        // TEXT TRIGGER EDITING
        ////////////////////////

        if(wParam == eKey && selectedModel)
        {
            if(selectedModel->GetTag() == "TextTrigger")
            {
                // Edit text
                ShowConsoleWindow();

                EditTextTrigger();

                SetForegroundWindow(hWnd);
            }
        }

        ////////////////////////
        // HEALTH PACK CREATION
        ////////////////////////

        if(wParam == hKey)
            CreateHealthPack();

        ////////////////////////
        // MODEL DELETION
        ////////////////////////
        else if(wParam == VK_DELETE)
            DeleteObject();

        ////////////////////////
        // SAVE / LOAD
        ////////////////////////

        else if(wParam == VK_RETURN)
        {
            SaveLevelToXML();
            SetForegroundWindow(hWnd); // set the game window back to front
        }
        else if(wParam == VK_SHIFT)
        {
            LoadLevelFromXML();
            SetForegroundWindow(hWnd);
        }

        ////////////////////////
        // RETURN TO MAIN MENU
        ////////////////////////

        else if(wParam == VK_BACK)
        {
            SceneManager::LoadScene("MainMenu");
            SceneManager::DeleteScene(sceneName);
            delete this;
        }

    }

    ////////////////////////////////////
    //  MODEL MOVEMENT WITH LEFT MOUSE
    ////////////////////////////////////

    if(uMsg == WM_LBUTTONDOWN)
    {
        mouseDown = true;
        SelectModel(LOWORD(lParam), HIWORD(lParam));
    }
    if(uMsg == WM_LBUTTONUP)
        mouseDown = false;

    if(uMsg == WM_RBUTTONDOWN)
        // If user right-clicks, deselect models.
        SetSelectedModel(nullptr);

    if(uMsg == WM_MOUSEMOVE && mouseDown)
        MoveObject(LOWORD(lParam), HIWORD(lParam));


    ////////////////////////////////
    // CAMERA ZOOM WITH MOUSE WHEEL
    ////////////////////////////////

    if(uMsg == WM_MOUSEWHEEL)
        ZoomCamera(GET_WHEEL_DELTA_WPARAM(wParam));
}




////////////////////////////////////////////////////
// MODEL CREATION FUNCTIONS
////////////////////////////////////////////////////

void LevelCreator::CreateBackground()
{
    waitingForConsoleInput = true; // lock further input until we're done

    // get filename input first
    cout << "Making Background..." << endl;

    if(!background)
        background = new Parallax();

    string filename = GetFilenameFromInput();
    backgroundTexture = filename;
    background->ParallaxInit(backgroundRelativeFilePath + filename);

    waitingForConsoleInput = false; // unlock input
}

void LevelCreator::CreatePlatform()
{
    waitingForConsoleInput = true;

    cout << "Making Platform..." << endl;

    string filepath = platformRelativeFilePath + GetFilenameFromInput();

    // instantiate model with default Width and Height of 1 and spawned at camera's center
    Model* temp = new Model(1.0, 1.0, cameraPosX, cameraPosY, "Platform" + to_string(platforms.size()), "Platform");
    temp->InitModel(filepath, true);

    platforms.push_back(temp);

    SetSelectedModel(platforms.back());

    waitingForConsoleInput = false;
}

void LevelCreator::CreatePlayer()
{
    cout << "Making Player..." << endl;

    player = new Model(1.0, 1.0, cameraPosX, cameraPosY, "Player", "Player"); // spawn player in middle of screen
    player->InitModel(playerTexturePath, true);

    SetSelectedModel(player);
}

void LevelCreator::CreateEnemy()
{
    waitingForConsoleInput = true;

    cout << "Making Enemy..." << endl;
    cout << "Choose The Type of Enemy To Make\n\t1 = Melee Enemy\n\t2 = Ranged Enemy\n\t3 = Ranged Boss\n\t4 = Melee Boss" << endl;

    char type;
    cin >> type;
    cin.clear();

    cin.ignore(10000,'\n');

    type = type - '0'; // convert to integer from ascii

    string filepath;
    Model* temp;

    // Enemy tags will be used in the XML file to determine which class to spawn
    if(type == 1)
    {
        filepath = enemyRelativeFilePath + "Melee_0000.png";
        temp = new Model(1.0, 1.0, cameraPosX, cameraPosY, "Enemy" + to_string(enemies.size()), "MeleeEnemy");
    }
    else if(type == 2)
    {
        filepath = enemyRelativeFilePath + "Ranged_0000.png";
        temp = new Model(1.0, 1.0, cameraPosX, cameraPosY, "Enemy" + to_string(enemies.size()), "RangedEnemy");
    }
    else if(type == 3){
        filepath = enemyRelativeFilePath + "Ranged_0000.png";
        temp = new Model(4.0, 4.0, cameraPosX, cameraPosY, "Enemy" + to_string(enemies.size()), "Boss");
    }
    else if(type == 4){
        filepath = enemyRelativeFilePath + "Melee_0000.png";
        temp = new Model(4.0, 4.0, cameraPosX, cameraPosY, "Enemy" + to_string(enemies.size()), "MeleeBoss");
    }

    // instantiate model with default Width and Height of 1 and spawned at camera's center
    temp->InitModel(filepath, true);

    enemies.push_back(temp);

    SetSelectedModel(enemies.back()); // set new selected model

    waitingForConsoleInput = false;
}

void LevelCreator::CreateLevelTrigger()
{

    if(nextLevelTrigger)
        return; // don't allow user to put more than 1 level trigger

    cout << "Insert the filename for the level trigger texture: " << endl;

    string newPath;

    getline(cin, newPath);

    // instantiate model with default Width and Height of 1 and spawned at camera's center
    nextLevelTrigger = new Model(1.0, 1.0, cameraPosX, cameraPosY, "Images/Misc/" + newPath, "LevelTrigger");
    nextLevelTrigger->InitModel("Images/Misc/" + newPath, true);

    SetSelectedModel(nextLevelTrigger);
}

void LevelCreator::CreateTextTrigger()
{
    cout << "Input the text for this trigger: " << endl;
    string newText;

    getline(cin, newText);

    // We'll use the model's name for storing the text in the level creator. This avoids making a hashtable or something.
    triggers.push_back(new Model(1.0, 1.0, cameraPosX, cameraPosY, newText, "TextTrigger"));
    triggers.back()->SetZoom(-0.1);
    triggers.back()->InitModel("Images/Misc/TextTrigger.png", true);

    SetSelectedModel(triggers.back());
}

void LevelCreator::CreateHealthPack()
{
    cout << "Making Health Pack..." << endl;

    healthPacks.push_back(new Model(0.3, 0.3, cameraPosX, cameraPosY, "HealthPack", "HealthPack")); // spawn player in middle of screen
    healthPacks.back()->InitModel("Images/Misc/HealthPack.png", true);

    SetSelectedModel(healthPacks.back());
}




////////////////////////////////////////////////////////////
//  OBJECT MANIPULATION FUNCTIONS
////////////////////////////////////////////////////////////

void LevelCreator::ModifyModelDimensions(double widthScale, double heightScale)
{
    // When user presses + or -, they can modify the width of the model they have currently selected.
    // When user presses up or down arrow, they can modify the height of the model they have selected.
    // If the user has no model selected and they press any of these keys, then they will modify the scale for the background

    if(!selectedModel)
    {
        // If no model is selected, then we assume the background is selected and scale it
        backgroundScaleX += 0.1 * widthScale; // if scale is - then this will subtract. Otherwise it will add
        backgroundScaleY += 0.1 * heightScale;
    }
    else if(selectedModel == player || selectedModel->GetTag() == "MeleeEnemy" || selectedModel->GetTag() == "RangedEnemy")
        return; // don't allow user to modify the dimensions of the player or enemies
    else
    {
        double newWidth = selectedModel->GetWidth() + (0.1 * widthScale);
        double newHeight = selectedModel->GetHeight() + (0.1 * heightScale);

        selectedModel->SetWidth(newWidth);
        selectedModel->SetHeight(newHeight);
    }
}

void LevelCreator::MoveCamera(double xMove, double yMove)
{
    // Pass 0 for xMove or yMove to keep that axis from being moved
    // Pass + 1 for xMove or yMove to increase the position of the axis.
    // Pass -1 for xMove or yMove to decrease the position of the axis.

    cout << "MOVING" << endl;
    cameraPosX += cameraMoveIncrement * xMove * cameraSpeed;
    cameraPosY += cameraMoveIncrement * yMove * cameraSpeed;
}

void LevelCreator::MoveObject(double mouseX, double mouseY)
{
    if(!selectedModel)
        return; // if no model is selected, leave the function

    double convertedX, convertedY;

    // convert mouse coordinates
    ConvertMouseToWorld(mouseX, mouseY, convertedX, convertedY);

    selectedModel->SetPosition(convertedX, convertedY);
}

void LevelCreator::SelectModel(double mouseX, double mouseY)
{
    double convertedX, convertedY;

    // convert mouse coordinates and put them into convertedX and convertedY
    ConvertMouseToWorld(mouseX, mouseY, convertedX, convertedY);

    // This will deselect the selectedModel. Allowing user to right-click empty space to deselect.
    SetSelectedModel(nullptr);

    // Iterate through all the models in the scene and check for mouse pointer collision.
    for(auto& platform : platforms)
    {
        if(CheckPointerCollision(platform, convertedX, convertedY))
        {
            // Set the new selected model
            SetSelectedModel(platform);
            return;
        }
    }

    for(auto& enemy : enemies)
    {
        if(CheckPointerCollision(enemy, convertedX, convertedY))
        {
            // Set the new selected model
            SetSelectedModel(enemy);
            return;
        }
    }

    for(auto& trigger : triggers)
    {
        if(CheckPointerCollision(trigger, convertedX, convertedY))
        {
            SetSelectedModel(trigger);
            return;
        }
    }

    for(auto& healthPack : healthPacks)
    {
        if(CheckPointerCollision(healthPack, convertedX, convertedY))
        {
            SetSelectedModel(healthPack);
            return;
        }
    }

    if(player)
    {
        if(CheckPointerCollision(player, convertedX, convertedY))
            SetSelectedModel(player);
    }

    if(nextLevelTrigger)
    {
        if(CheckPointerCollision(nextLevelTrigger, convertedX, convertedY))
            SetSelectedModel(nextLevelTrigger);
    }
}

void LevelCreator::DeleteObject()
{
    if(!selectedModel)
    {
        delete background;
        background = nullptr;
        return; // delete background if nothing is selected
    }

    if(selectedModel == player)
    {
        cout << "DELETE PLAYER" << endl;
         // If we're deleting the player, then just remove the player pointer
        delete player;
        player = nullptr;
    }

    else if(selectedModel == nextLevelTrigger)
    {
        // Remove next level trigger since there's only one of them
        delete nextLevelTrigger;
        nextLevelTrigger = nullptr;
    }
    else
    {
        // otherwise, we need to look through platforms and enemies, find the model, and remove it
        auto finderPlatform = find(platforms.begin(), platforms.end(), selectedModel);
        auto finderEnemy = find(enemies.begin(), enemies.end(), selectedModel);
        auto finderTrigger = find(triggers.begin(), triggers.end(), selectedModel);
        auto finderHealthPack = find(healthPacks.begin(), healthPacks.end(), selectedModel);

        if(finderPlatform != platforms.end())
            // we found the selected model in platforms. Now remove it
            platforms.erase(std::remove(platforms.begin(), platforms.end(), selectedModel), platforms.end());
        else if(finderEnemy != enemies.end())
             // we found selected model in enemies. Remove it
            enemies.erase(std::remove(enemies.begin(), enemies.end(), selectedModel), enemies.end());
        else if(finderTrigger != triggers.end())
            // found trigger, erase it
            triggers.erase(std::remove(triggers.begin(), triggers.end(), selectedModel), triggers.end());
        else if(finderHealthPack != healthPacks.end())
            // found the health pack, erase it
            healthPacks.erase(std::remove(healthPacks.begin(), healthPacks.end(), selectedModel), healthPacks.end());

    }

    SetSelectedModel(nullptr);
}

void LevelCreator::EditTextTrigger()
{
    if(!selectedModel)
        return; // this should never be called but just in case

    cout << "This Trigger's Current Text is:" << endl << "\"" + selectedModel->GetName() + "\"" << endl << "Do you wish to change it? (y/n): ";

    char input;

    cin >> input;

    cin.clear();
    cin.ignore(10000,'\n');

    if(input == 'y' || input == 'Y')
    {
        cout << "Input new Text:" << endl;
        string newText;

        // Need to do this twice to flush the buffer from previous character input.
        getline(cin, newText);
        selectedModel->SetName(newText);
    }
}




/////////////////////////////////////////////////////
//       HELPER FUNCTIONS                          //
/////////////////////////////////////////////////////

string LevelCreator::GetFilenameFromInput()
{
    string path;

    cout << "Input texture file name (don't input full path): ";
    getline(cin, path);

    return path;
}

void LevelCreator::ShowConsoleWindow()
{
    ShowWindow(consoleWindow, SW_RESTORE);
    SetForegroundWindow(consoleWindow);
}

void LevelCreator::SetSelectedModel(Model* newModel)
{
    if(selectedModel)
        selectedModel->SetColor(1.0, 1.0, 1.0); // if selected model exists, set its color back to normal

    selectedModel = newModel;

    if(selectedModel)
        selectedModel->SetColor(1.0, 0, 0);
}

void LevelCreator::ClearScene()
{
    // iterate through vectors and delete pointers then clear the vectors
    for(auto& platform : platforms)
        delete platform;

    platforms.clear();

    for(auto& enemy : enemies)
        delete enemy;

    enemies.clear();

    for(auto& trigger : triggers)
        delete trigger;

    triggers.clear();

    for(auto& healthPack : healthPacks)
        delete healthPack;

    healthPacks.clear();

    delete player;
    player = nullptr;

    delete background;
    background = nullptr;

    delete nextLevelTrigger;
    nextLevelTrigger = nullptr;

    selectedModel = nullptr;
}

void LevelCreator::ZoomCamera(int delta)
{
    cameraPosZ -= 0.5 * (delta / 120);
}


//////////////////////////////
// XML SAVING AND LOADING
/////////////////////////////

void LevelCreator::SaveLevelToXML()
{
    /*
    XML Structure is as follows

    <LevelNum>
        <Platforms>
            <Platform>
                <data/>
            </Platform>
            ...
        </Platforms>

        <Enemies>
            <Enemy>
                <data/>
            </Enemy>
            ...
        </Enemies>

        <Player>
            <data>
            ...
        </Player>

        <Background>
            <data/>
        </Background>

        <LevelTrigger>
            <data/>
        </LevelTrigger>
    </LevelNum>
    */

    ShowConsoleWindow();

    if(!player || !nextLevelTrigger || platforms.size() <= 0)
    {
        cout << "ERROR SAVING FILE: Scene must have a player, a next level trigger, and at least one platform" << endl;
        return;
    }

    cout << "Input a number for the level (1 - 9): ";

    char num;

    cin >> num;
    cin.clear();
    cin.ignore(10000,'\n');

    int convertNum = num - 48;

    if(convertNum > 9 || convertNum < 1)
    {
        cout << "ERROR: only numbers 1 - 9 are valid" << endl;
        return;
    }

    string filepath = "LevelData/Level" + to_string(convertNum) + ".xml";

    XMLDocument xmlDoc;

    // establish root node for the document
    XMLNode * pRoot = xmlDoc.NewElement(("Level" + to_string(convertNum)).c_str());
    xmlDoc.InsertFirstChild(pRoot);
    XMLElement* endElement; // tracks the ending element in the doc


    ///////////////////
    // PLATFORMS
    ///////////////////

    // Create child Element Platforms, which will contain all the platforms
    XMLElement * platformsElement = xmlDoc.NewElement("Platforms");
    XMLElement * pChild;

    for(auto& platform : platforms)
    {
        XMLElement* newPlatform = xmlDoc.NewElement("Platform");
        newPlatform->SetAttribute("Width", platform->GetWidth());
        newPlatform->SetAttribute("Height", platform->GetHeight());
        newPlatform->SetAttribute("xPos", platform->GetX());
        newPlatform->SetAttribute("yPos", platform->GetY());

        // Put texture name as a child of this platform element
        XMLElement* platformTexture = xmlDoc.NewElement("Texture");
        platformTexture->SetText(platform->GetTextureName().c_str());

        newPlatform->InsertEndChild(platformTexture);

        if(!platformsElement->FirstChild())
            platformsElement->InsertFirstChild(newPlatform);
        else
            platformsElement->InsertAfterChild(pChild, newPlatform);

        pChild = newPlatform;
    }

    pRoot->InsertFirstChild(platformsElement);
    platformsElement->InsertEndChild(pChild);



    ////////////////////////
    // PLAYER
    ////////////////////////
    XMLElement* playerElement = xmlDoc.NewElement("Player");
    playerElement->SetAttribute("xPos", player->GetX());
    playerElement->SetAttribute("yPos", player->GetY());

    pRoot->InsertAfterChild(platformsElement, playerElement); // insert player after platforms


    ////////////////////////
    // LEVEL TRIGGER
    ////////////////////////

    XMLElement* trigger = xmlDoc.NewElement("LevelTrigger");
    trigger->SetAttribute("Width", nextLevelTrigger->GetWidth());
    trigger->SetAttribute("Height", nextLevelTrigger->GetHeight());
    trigger->SetAttribute("xPos", nextLevelTrigger->GetX());
    trigger->SetAttribute("yPos", nextLevelTrigger->GetY());

    XMLElement* levelTriggerTexture = xmlDoc.NewElement("Texture");
    levelTriggerTexture->SetText(nextLevelTrigger->GetName().c_str());
    trigger->InsertEndChild(levelTriggerTexture);

    endElement = trigger;
    pRoot->InsertAfterChild(playerElement, trigger); // insert player after platforms


    XMLElement* triggerElement;

    if(triggers.size() > 0)
    {
        triggerElement = xmlDoc.NewElement("TextTriggers");
        XMLElement* tChild;

        for(auto& trigger : triggers)
        {
            XMLElement* newTrigger = xmlDoc.NewElement("Trigger");
            newTrigger->SetAttribute("Width", trigger->GetWidth());
            newTrigger->SetAttribute("Height", trigger->GetHeight());
            newTrigger->SetAttribute("xPos", trigger->GetX());
            newTrigger->SetAttribute("yPos", trigger->GetY());

            XMLElement* triggerChild = xmlDoc.NewElement("Text");
            triggerChild->SetText(trigger->GetName().c_str());
            newTrigger->InsertEndChild(triggerChild);

            if(!triggerElement->FirstChild())
                triggerElement->InsertFirstChild(newTrigger);
            else
                triggerElement->InsertAfterChild(tChild, newTrigger);

            tChild = newTrigger;
        }

        pRoot->InsertAfterChild(trigger, triggerElement);
        triggerElement->InsertEndChild(tChild);
        endElement = triggerElement;
    }


    ////////////////////////
    // HEALTH PACKS
    ////////////////////////

    // Create child Element Platforms, which will contain all the platforms
    XMLElement * healthElement = xmlDoc.NewElement("HealthPacks");
    XMLElement * hChild;

    if(healthPacks.size() > 0)
    {
        for(auto& healthPack : healthPacks)
        {
            XMLElement* newHealthPack = xmlDoc.NewElement("HealthPack");
            newHealthPack->SetAttribute("Width", healthPack->GetWidth());
            newHealthPack->SetAttribute("Height", healthPack->GetHeight());
            newHealthPack->SetAttribute("xPos", healthPack->GetX());
            newHealthPack->SetAttribute("yPos", healthPack->GetY());

            if(!healthElement->FirstChild())
                healthElement->InsertFirstChild(newHealthPack);
            else
                healthElement->InsertAfterChild(hChild, newHealthPack);

            hChild = newHealthPack;
        }

        pRoot->InsertAfterChild(triggerElement, healthElement);
        healthElement->InsertEndChild(hChild);

        endElement = healthElement;
    }



    ////////////////////////
    //  BACKGROUND
    ////////////////////////

    if(background)
    {
        XMLElement* backgroundElement = xmlDoc.NewElement("Background");
        backgroundElement->SetAttribute("Width", backgroundScaleX);
        backgroundElement->SetAttribute("Height", backgroundScaleY);

        XMLElement* bgTexture = xmlDoc.NewElement("Texture");
        bgTexture->SetText((backgroundRelativeFilePath + backgroundTexture).c_str());

        backgroundElement->InsertEndChild(bgTexture);

        pRoot->InsertAfterChild(endElement, backgroundElement);

        endElement = backgroundElement;
    }

    ////////////////////
    // ENEMIES
    ////////////////////

    if(enemies.size() > 0)
    {
        XMLElement* enemiesElement = xmlDoc.NewElement("Enemies");

        for(auto& enemy : enemies)
        {
            // enemies always have width and height of 1.0 each. Don't need attributes for them.
            XMLElement* newEnemy = xmlDoc.NewElement("Enemy");
            newEnemy->SetAttribute("xPos", enemy->GetX());
            newEnemy->SetAttribute("yPos", enemy->GetY());

            XMLElement* enemyName = xmlDoc.NewElement("Name");
            enemyName->SetText(enemy->GetName().c_str());
            XMLElement* enemyTag = xmlDoc.NewElement("Tag");
            enemyTag->SetText(enemy->GetTag().c_str());

            newEnemy->InsertFirstChild(enemyName);
            newEnemy->InsertEndChild(enemyTag);

            if(!enemiesElement->FirstChild())
                enemiesElement->InsertFirstChild(newEnemy);
            else
                enemiesElement->InsertAfterChild(pChild, newEnemy);

            pChild = newEnemy;
        }

        enemiesElement->InsertEndChild(pChild);

        endElement = enemiesElement; // update end element pointer

    }

    // Determine what the end child should be for the doc.
    // If there are no enemies but a backgroudn exists, set to background
    // If there are no enemies and no background, set to trigger
    // Otherwise, we set to enemies
    pRoot->InsertEndChild(endElement);

    xmlDoc.SaveFile(filepath.c_str()); // save the xml file
}


void LevelCreator::LoadLevelFromXML()
{
    ShowConsoleWindow();

    cout << "Input a level numebr to load (0 - 9): ";

    char num;

    cin >> num;
    cin.clear();
    cin.ignore(10000,'\n');

    int convertNum = num - 48;

    string filepath = "LevelData/Level" + to_string(convertNum) + ".xml";
    ifstream checkFile(filepath);

    if(!checkFile)
    {
        cout << "ERROR: " << filepath << " DOES NOT EXIST" << endl;
        return;
    }

    ClearScene();

    XMLDocument xmlDoc;
    xmlDoc.LoadFile(filepath.c_str());

    const XMLElement* root = xmlDoc.FirstChildElement(); // gets root node



    /////////////////
    // LOAD PLATFORMS
    /////////////////

    const XMLElement* mainElements = root->FirstChildElement();
    for (const XMLElement* child = mainElements->FirstChildElement(); child != 0; child=child->NextSiblingElement())
    {
        double newWidth, newHeight, newX, newY;
        child->QueryAttribute("Width", &newWidth);
        child->QueryAttribute("Height", &newHeight);
        child->QueryAttribute("xPos", &newX);
        child->QueryAttribute("yPos", &newY);

        string newTexture = child->FirstChildElement()->GetText();

        platforms.push_back(new Model(newWidth, newHeight, newX, newY, "Platform", "Platform"));
        platforms.back()->InitModel(newTexture, true);
    }

    //////////////
    // LOAD PLAYER
    //////////////

    mainElements = mainElements->NextSiblingElement();
    double playerX, playerY;
    mainElements->QueryAttribute("xPos", &playerX);
    mainElements->QueryAttribute("yPos", &playerY);

    player = new Model(1.0, 1.0, playerX, playerY, "Player", "Player");
    player->InitModel(playerTexturePath, true);

    /////////////////////
    // LOAD LEVEL TRIGGER
    /////////////////////

    mainElements = mainElements->NextSiblingElement();
    double newWidth, newHeight, newX, newY;
    mainElements->QueryAttribute("Width", &newWidth);
    mainElements->QueryAttribute("Height", &newHeight);
    mainElements->QueryAttribute("xPos", &newX);
    mainElements->QueryAttribute("yPos", &newY);

    string texturePath = mainElements->FirstChildElement()->GetText();

    nextLevelTrigger = new Model(newWidth, newHeight, newX, newY, texturePath , "Trigger");
    nextLevelTrigger->InitModel(texturePath, true);

    //////////////////////
    // LOAD TEXT TRIGGERS
    //////////////////////

    mainElements = mainElements->NextSiblingElement();

    // Need to store element name into a string due to the return type being const char*
    string checkName = mainElements->Name();

    if(checkName == "TextTriggers")
    {
        for (const XMLElement* child = mainElements->FirstChildElement(); child != 0; child=child->NextSiblingElement())
        {
            double newX, newY, newWidth, newHeight;
            child->QueryAttribute("xPos", &newX);
            child->QueryAttribute("yPos", &newY);
            child->QueryAttribute("Width", &newWidth);
            child->QueryAttribute("Height", &newHeight);

            string newText;

            newText = child->FirstChildElement()->GetText(); // get the name of the enemy

            triggers.push_back(new Model(newWidth, newHeight, newX, newY, newText, "TextTrigger"));
            triggers.back()->SetZoom(-0.1);
            triggers.back()->InitModel("Images/Misc/TextTrigger.png", true);
        }


        mainElements = mainElements->NextSiblingElement();
        checkName = mainElements->Name();
    }

    ////////////////////
    // LOAD HEALTH PACKS
    ////////////////////

    if(checkName == "HealthPacks")
    {
        for (const XMLElement* child = mainElements->FirstChildElement(); child != 0; child=child->NextSiblingElement())
        {
            double newX, newY, newWidth, newHeight;
            child->QueryAttribute("xPos", &newX);
            child->QueryAttribute("yPos", &newY);
            child->QueryAttribute("Width", &newWidth);
            child->QueryAttribute("Height", &newHeight);

            healthPacks.push_back(new Model(newWidth, newHeight, newX, newY, "HealthPack", "HealthPack"));

            healthPacks.back()->InitModel("Images/Misc/HealthPack.png", true);
        }

        mainElements = mainElements->NextSiblingElement();
        checkName = mainElements->Name();
    }

    //////////////////
    // LOAD BACKGROUND
    //////////////////

    if(checkName == "Background")
    {
        // LevelCreator class has variables for background scale x and y, which is where we'll dump width and height.
        mainElements->QueryAttribute("Width", &backgroundScaleX);
        mainElements->QueryAttribute("Height", &backgroundScaleY);
        string texturePath = mainElements->FirstChildElement()->GetText();

        background = new Parallax();
        background->ParallaxInit(texturePath);

        string::size_type findRelativePath = texturePath.find(backgroundRelativeFilePath);

        if (findRelativePath != string::npos)
            texturePath.erase(findRelativePath, backgroundRelativeFilePath.length());

        backgroundTexture = texturePath;
        cout << texturePath << endl;

        mainElements = mainElements->NextSiblingElement();
        checkName = mainElements->Name();
    }

    //////////////////////
    // LOAD ENEMIES
    //////////////////////

    if(checkName == "Enemies")
    {
        for (const XMLElement* child = mainElements->FirstChildElement(); child != 0; child=child->NextSiblingElement())
        {
            double newX, newY;
            child->QueryAttribute("xPos", &newX);
            child->QueryAttribute("yPos", &newY);

            string newName, newTag;

            newName = child->FirstChildElement()->GetText(); // get the name of the enemy
            newTag = child->LastChildElement()->GetText(); // get the tag of the enemy

            enemies.push_back(new Model(1.0, 1.0, newX, newY, newName, newTag));

            string texturePath;

            if(newTag == "MeleeEnemy")
                texturePath = "Images/Enemies/Melee_0000.png";
            else if(newTag == "RangedEnemy")
                texturePath = "Images/Enemies/Ranged_0000.png";
            else if(newTag == "Boss")
                texturePath = "Images/Enemies/Ranged_0000.png";
            else if(newTag == "MeleeBoss")
                texturePath = "Images/Enemies/Melee_0000.png";

            enemies.back()->InitModel(texturePath, true);
        }
    }
}
