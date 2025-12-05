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

struct t_GridSettings
{
	float line_thickness = 1.0f;
	int rows = 1;
	int columns = 1;
	bool b_ShowGrid = true;
};

struct t_CellSelection
{
	int row = 0;
	int col = 0;
	int index = 0;
	int total_cells;
};

struct t_DisplaySettings
{
	Vector2 position = {50, 50};
	float scale = 1.0f;
	float preview_scale = 1.0f;
	bool b_ShowCellInfo = true;
};

struct FrameSize
{
	float width;
	float height;
	float display_w;
	float display_h;
	float grid_x;
	float grid_y;
};

class SpriteSheetCutterApp
{
public:
	SpriteSheetCutterApp();
	~SpriteSheetCutterApp();
	void run();

private:
	Texture2D m_SpriteSheet{};
	std::string m_TexturePath;
	t_GridSettings m_Grid;
	t_DisplaySettings m_Display;
	t_CellSelection m_Selection;
	FrameSize m_Frame;
	bool m_bExportFailed;

	Rectangle GetFrameRect(int row, int col, float frame_w, float frame_h);
	std::string GetFileFromDialog();
	void Draw();
	void RenderUI(float frame_w, float frame_h);
	void ExportAllFrames(const char *dest_file_name);
	void DrawGridOverlay(float frame_w, float frame_h);
	void DrawCellHighlight(float sheet_w, float sheet_h);
	void DrawEnlargedPreview(float frameW, float frame_h);
};