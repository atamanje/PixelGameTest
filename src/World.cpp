#include "pch.h"
#include "World.h"

World::World(int width, int height, float tileSize)
    : mapWidth(width), mapHeight(height), tileSize(tileSize) {
    m_tiles.resize(mapWidth * mapHeight);
    m_tilesetTex = std::make_shared<Texture2D>();
    GenerateDefaultMap();
}

bool World::LoadTileset(const std::string& path) {
    return m_tilesetTex->LoadFromFile(path);
}

void World::GenerateDefaultMap() {
    uint32_t cGrass = IM_COL32(60, 160, 70, 255);
    uint32_t cWall  = IM_COL32(100, 100, 110, 255);
    uint32_t cWater = IM_COL32(40, 100, 200, 255);
    uint32_t cPath  = IM_COL32(180, 150, 100, 255);

    int lakeMinX = 18, lakeMaxX = 25;
    int lakeMinY = 3,  lakeMaxY = 8;

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            // Map borders are solid walls
            if (x == 0 || x == mapWidth - 1 || y == 0 || y == mapHeight - 1) {
                SetTile(x, y, TileType::Wall, true, cWall);
            }
            // Lake with 4-sided animated lapping coast & corners
            else if (x >= lakeMinX && x <= lakeMaxX && y >= lakeMinY && y <= lakeMaxY) {
                if (x == lakeMinX && y == lakeMinY) {
                    SetTile(x, y, TileType::WaterCoastNW, true, cWater);
                } else if (x == lakeMaxX && y == lakeMinY) {
                    SetTile(x, y, TileType::WaterCoastNE, true, cWater);
                } else if (x == lakeMinX && y == lakeMaxY) {
                    SetTile(x, y, TileType::WaterCoastSW, true, cWater);
                } else if (x == lakeMaxX && y == lakeMaxY) {
                    SetTile(x, y, TileType::WaterCoastSE, true, cWater);
                } else if (y == lakeMinY) {
                    SetTile(x, y, TileType::WaterCoastN, true, cWater);
                } else if (y == lakeMaxY) {
                    SetTile(x, y, TileType::WaterCoastS, true, cWater);
                } else if (x == lakeMinX) {
                    SetTile(x, y, TileType::WaterCoastW, true, cWater);
                } else if (x == lakeMaxX) {
                    SetTile(x, y, TileType::WaterCoastE, true, cWater);
                } else {
                    SetTile(x, y, TileType::Water, true, cWater);
                }
            }
            // Cobblestone path down the middle
            else if (x == 12 || x == 13) {
                SetTile(x, y, TileType::Path, false, cPath);
            }
            // Default grass
            else {
                SetTile(x, y, TileType::Grass, false, cGrass);
            }
        }
    }
}

Tile World::GetTile(int x, int y) const {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) {
        return Tile{ TileType::Wall, true, IM_COL32(80, 80, 80, 255) };
    }
    return m_tiles[y * mapWidth + x];
}

void World::SetTile(int x, int y, TileType type, bool isSolid, uint32_t color) {
    if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
        m_tiles[y * mapWidth + x] = Tile{ type, isSolid, color };
    }
}

bool World::CheckCollision(Vec2 targetPos, Vec2 targetSize, Entity* ignoreEntity) const {
    // 1. Check Tile map collisions
    int minTileX = (int)(targetPos.x / tileSize);
    int maxTileX = (int)((targetPos.x + targetSize.x) / tileSize);
    int minTileY = (int)(targetPos.y / tileSize);
    int maxTileY = (int)((targetPos.y + targetSize.y) / tileSize);

    for (int ty = minTileY; ty <= maxTileY; ++ty) {
        for (int tx = minTileX; tx <= maxTileX; ++tx) {
            Tile tile = GetTile(tx, ty);
            if (tile.isSolid) {
                return true;
            }
        }
    }

    // 2. Check Entity collisions
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive || !entity->isSolid) continue;
        if (entity.get() == ignoreEntity) continue;

        if (entity->CollidesWithRect(targetPos, targetSize)) {
            return true;
        }
    }

    return false;
}

Entity* World::FindEntityInRect(Vec2 rectPos, Vec2 rectSize, Entity* ignoreEntity) const {
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive) continue;
        if (entity.get() == ignoreEntity) continue;

        if (entity->CollidesWithRect(rectPos, rectSize)) {
            return entity.get();
        }
    }
    return nullptr;
}

void World::GetTileUVs(TileType type, float& u0, float& v0, float& u1, float& v1) const {
    // Tileset layout (4 cols, 10 rows, 32x32 per tile = 128x320 total)
    // Row 0: Open Water Shimmer (cols 0..3)
    // Row 1: Coast North (cols 0..3)
    // Row 2: Coast South (cols 0..3)
    // Row 3: Coast West (cols 0..3)
    // Row 4: Coast East (cols 0..3)
    // Row 5: Corner NW (cols 0..3)
    // Row 6: Corner NE (cols 0..3)
    // Row 7: Corner SW (cols 0..3)
    // Row 8: Corner SE (cols 0..3)
    // Row 9: Grass(0,9), Path(1,9), Wall(2,9), Dirt(3,9)
    int col = 0, row = 0;
    switch (type) {
        case TileType::Water:        col = m_waterFrame; row = 0; break;
        case TileType::WaterCoastN:  col = m_waterFrame; row = 1; break;
        case TileType::WaterCoastS:  col = m_waterFrame; row = 2; break;
        case TileType::WaterCoastW:  col = m_waterFrame; row = 3; break;
        case TileType::WaterCoastE:  col = m_waterFrame; row = 4; break;
        case TileType::WaterCoastNW: col = m_waterFrame; row = 5; break;
        case TileType::WaterCoastNE: col = m_waterFrame; row = 6; break;
        case TileType::WaterCoastSW: col = m_waterFrame; row = 7; break;
        case TileType::WaterCoastSE: col = m_waterFrame; row = 8; break;
        case TileType::Grass:        col = 0; row = 9; break;
        case TileType::Path:         col = 1; row = 9; break;
        case TileType::Wall:         col = 2; row = 9; break;
        case TileType::Dirt:
        case TileType::WoodFloor:    col = 3; row = 9; break;
    }

    float texWidth = 128.0f;
    float texHeight = 320.0f;
    float tileSizePx = 32.0f;
    float inset = 0.05f;

    u0 = (col * tileSizePx + inset) / texWidth;
    v0 = (row * tileSizePx + inset) / texHeight;
    u1 = ((col + 1) * tileSizePx - inset) / texWidth;
    v1 = ((row + 1) * tileSizePx - inset) / texHeight;
}

void World::Update(float deltaTime) {
    // Advance water animation frame
    m_waterAnimTimer += deltaTime;
    if (m_waterAnimTimer >= 0.20f) { // 5 FPS water animation speed
        m_waterAnimTimer -= 0.20f;
        m_waterFrame = (m_waterFrame + 1) % 4;
    }

    for (auto& entity : m_entities) {
        if (entity && entity->isActive) {
            entity->Update(deltaTime, *this);
        }
    }
}

void World::Render(ImDrawList* drawList, const Camera2D& camera, Vec2 viewportSize, Vec2 viewportOrigin) {
    if (!drawList) return;

    bool hasTileset = (m_tilesetTex && m_tilesetTex->IsLoaded());

    // Render TileMap
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            Tile tile = GetTile(x, y);
            Vec2 tileWorldMin(x * tileSize, y * tileSize);
            Vec2 tileWorldMax = tileWorldMin + Vec2(tileSize, tileSize);

            Vec2 screenMin = camera.WorldToScreen(tileWorldMin, viewportSize, viewportOrigin);
            Vec2 screenMax = camera.WorldToScreen(tileWorldMax, viewportSize, viewportOrigin);

            // Culling: only draw tiles visible inside viewport bounds
            if (screenMax.x < viewportOrigin.x || screenMin.x > viewportOrigin.x + viewportSize.x ||
                screenMax.y < viewportOrigin.y || screenMin.y > viewportOrigin.y + viewportSize.y) {
                continue;
            }

            if (hasTileset) {
                float u0, v0, u1, v1;
                GetTileUVs(tile.type, u0, v0, u1, v1);
                drawList->AddImage(
                    (ImTextureID)(intptr_t)m_tilesetTex->GetID(),
                    ImVec2(screenMin.x, screenMin.y),
                    ImVec2(screenMax.x, screenMax.y),
                    ImVec2(u0, v0),
                    ImVec2(u1, v1)
                );
            } else {
                drawList->AddRectFilled(ImVec2(screenMin.x, screenMin.y), ImVec2(screenMax.x, screenMax.y), tile.color);
                drawList->AddRect(ImVec2(screenMin.x, screenMin.y), ImVec2(screenMax.x, screenMax.y), IM_COL32(0, 0, 0, 40));
            }
        }
    }

    // Render Entities
    for (auto& entity : m_entities) {
        if (entity && entity->isActive) {
            entity->Render(drawList, camera, viewportSize, viewportOrigin);
        }
    }
}
