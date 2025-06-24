#pragma once

#include <rlImGui.h>
#include <imgui.h>
#include <string>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <tinyfiledialogs.h>
#include "ImGuiCustomTheme.h"
namespace fs = std::filesystem;

// For make typecasting easy
constexpr auto f2i = [](float f)
	{ return static_cast<int>(f); };

struct GridSettings
{
	float lineThickness = 1.0f;
	int rows = 1;
	int columns = 1;
	bool showGrid = true;
};

struct CellSelection
{
	int row = 0;
	int col = 0;
	int index = 0;
};

struct DisplaySettings
{
	Vector2 position = { 50, 50 };
	float scale = 1.0f;
	float previewScale = 1.0f;
	bool showCellInfo = true;
};

class SpriteSheetCutterApp
{
public:
	SpriteSheetCutterApp();
	~SpriteSheetCutterApp();
	void run();

private:
	Texture2D spriteSheet{};
	std::string texturePath;
	GridSettings grid;
	DisplaySettings display;
	CellSelection selection;
	bool exportFailed;

	Rectangle GetFrameRect(int row, int col, float frameW, float frameH);
	std::string GetFileFromDialog();
	void Draw();
	void RenderUI(float frameW, float frameH);
	void ExportAllFrames(char* destFileName);
	void DrawGridOverlay(float frameW, float frameH);
	void DrawCellHighlight(float sheetW, float sheetH);
	void DrawEnlargedPreview(float frameW, float frameH);
};