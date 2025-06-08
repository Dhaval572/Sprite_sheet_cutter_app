#pragma once

#include <rlImGui.h>
#include <imgui.h>
#include <string>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <format>
#include <tinyfiledialogs.h>
#include "ImGuiCustomTheme.h"
namespace fs = std::filesystem;

// For make typecasting easy
inline constexpr auto f2i = [](float f)
{ return static_cast<int>(f); };

struct GridSettings
{
    int rows = 1;
    int columns = 1;
    bool showGrid = true;
    float lineThickness = 1.0f;
};

struct CellSelection
{
    int row = 0;
    int col = 0;
    int index = 0;
};

struct DisplaySettings
{
    float scale = 1.0f;
    float previewScale = 1.0f;
    Vector2 position = {50, 50};
    bool snapToPixels = true;
    bool showCellInfo = true;
};

class SpriteSheetCutterApp
{
public:
    SpriteSheetCutterApp();
    ~SpriteSheetCutterApp();
    void run();

private:
    std::string texturePath;
    Texture2D spriteSheet{};
    GridSettings grid;
    DisplaySettings display;
    CellSelection selection;
    bool exportFailed;

    void Update();
    void Draw();
    void RenderUI(float frameW, float frameH);
    void ExportAllFrames(char *destFileName);
    Rectangle GetFrameRect(int row, int col, float frameW, float frameH);
    std::string GetFileFromDialog();
    void DrawGridOverlay(float frameW, float frameH);
    void DrawCellHighlight(float sheetW, float sheetH);
    void DrawEnlargedPreview(float frameW, float frameH);
};