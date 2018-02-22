#ifndef MODEL_H
#define MODEL_H

#include <windows.h>
#include "glut.h"

class Model
{
    public:
        Model();
        virtual ~Model();
        void DrawModel();
        void InitModel(char* fileName);
        void Jump();
        void Update();
        void SetJump(bool);

        double rotateX, rotateY, rotateZ;

        double zoom;

        double xPos, yPos;

    protected:

    private:
        bool jump;
        float jumpY;
        float initialY;
};

#endif // MODEL_H
