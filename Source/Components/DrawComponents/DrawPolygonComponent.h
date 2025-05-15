#pragma once

#include "DrawComponent.h"
#include <vector>
#include "../../Utils/Math.h"

class DrawPolygonComponent : public DrawComponent {
    public:
        // (Lower draw order corresponds with further back)
        DrawPolygonComponent(Actor *owner, std::vector<Vector2> &vertices, int drawOrder = 100);

        void Draw(SDL_Renderer *renderer) override;
        std::vector<Vector2>& GetVertices() { return mVertices; }
    protected:
        int mDrawOrder;
        std::vector<Vector2> mVertices;
};