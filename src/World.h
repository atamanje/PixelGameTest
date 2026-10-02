#pragma once
#include "Vec2.h"
#include "Entity.h"
#include "Texture2D.h"
#include <vector>
#include <memory>

enum class TileType {
    Grass,
    Dirt,
    Water,
    WaterCoastN,
    WaterCoastS,
    WaterCoastE,
    WaterCoastW,
    WaterCoastNW,
    WaterCoastNE,
    WaterCoastSW,
    WaterCoastSE,
    Wall,
    Path,
    WoodFloor
};

struct Tile {
    TileType type = TileType::Grass;
    bool isSolid = false;
    uint32_t color = 0xFF44AA44; // ABGR format
};

class World {
public:
    int mapWidth = 30;
    int mapHeight = 20;
    float tileSize = 32.0f;

    World(int width = 30, int height = 20, float tileSize = 32.0f);

    bool LoadTileset(const std::string& path);
    void GenerateDefaultMap();

    Tile GetTile(int x, int y) const;
    void SetTile(int x, int y, TileType type, bool isSolid, uint32_t color);

    bool CheckCollision(Vec2 targetPos, Vec2 targetSize, Entity* ignoreEntity = nullptr) const;
    Entity* FindEntityInRect(Vec2 rectPos, Vec2 rectSize, Entity* ignoreEntity = nullptr) const;

    template<typename T, typename... Args>
    T* AddEntity(Args&&... args) {
        auto entity = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = entity.get();
        entity->id = m_nextEntityId++;
        m_entities.push_back(std::move(entity));
        return ptr;
    }

    void Update(float deltaTime);
    void Render(ImDrawList* drawList, const Camera2D& camera, Vec2 viewportSize, Vec2 viewportOrigin);

    const std::vector<std::unique_ptr<Entity>>& GetEntities() const { return m_entities; }

private:
    std::vector<Tile> m_tiles;
    std::vector<std::unique_ptr<Entity>> m_entities;
    std::shared_ptr<Texture2D> m_tilesetTex;
    int m_nextEntityId = 1;

    float m_waterAnimTimer = 0.0f;
    int m_waterFrame = 0;

    void GetTileUVs(TileType type, float& u0, float& v0, float& u1, float& v1) const;
};
