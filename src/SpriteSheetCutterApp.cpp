#include "SpriteSheetCutterApp.h"

SpriteSheetCutterApp::SpriteSheetCutterApp()
{
	texturePath = "";
	exportFailed = false;
}

std::string SpriteSheetCutterApp::GetFileFromDialog()
{
	static const char *fileTypes[] = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};

	const char *result = tinyfd_openFileDialog(
		"Select an image",
		"",
		4,
		fileTypes,
		"Image files",
		0);

	if (result != nullptr)
	{
		texturePath = std::string(result);
		return texturePath;
	}
	return "";
}

void SpriteSheetCutterApp::run()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1200, 700, "Ultra-Accurate Sprite Sheet Splitter");
	SetTargetFPS(120);
	rlImGuiSetup(true);
	ImCustomTheme();

	while (!WindowShouldClose())
	{
		Update();
		Draw();
	}
}

void SpriteSheetCutterApp::Update()
{
	if (display.snapToPixels)
	{
		display.position.x = floorf(display.position.x);
		display.position.y = floorf(display.position.y);
	}
}

void SpriteSheetCutterApp::Draw()
{
	float frameW = spriteSheet.width / static_cast<float>(grid.columns);
	float frameH = spriteSheet.height / static_cast<float>(grid.rows);

	BeginDrawing();
	ClearBackground(GRAY);

	if (spriteSheet.id != 0)
	{
		SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
		DrawTexturePro(spriteSheet, {0, 0, static_cast<float>(spriteSheet.width), static_cast<float>(spriteSheet.height)},
					   {display.position.x, display.position.y, spriteSheet.width * display.scale, spriteSheet.height * display.scale},
					   {0, 0}, 0.0f, WHITE);

		DrawGridOverlay(frameW, frameH);
		DrawCellHighlight(frameW, frameH);
		DrawEnlargedPreview(frameW, frameH);
	}

	rlImGuiBegin();

	if (spriteSheet.id != 0)
	{
		RenderUI(frameW, frameH);
	}
	else
	{
		if (ImGui::Button("Load Sprite"))
		{
			std::string selectedPath = GetFileFromDialog();
			if (!selectedPath.empty())
			{
				spriteSheet = LoadTexture(texturePath.c_str());
			}
		}

		if (!texturePath.empty() && spriteSheet.id == 0)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load texture! Only Choose Image files");
			ImGui::Text("Path: %s", texturePath.c_str());
		}
	}

	rlImGuiEnd();
	EndDrawing();
}

Rectangle SpriteSheetCutterApp::GetFrameRect(int row, int col, float frameW, float frameH)
{
	return Rectangle{col * frameW, row * frameH, frameW, frameH};
}

void SpriteSheetCutterApp::DrawGridOverlay(float frameW, float frameH)
{
	if (!grid.showGrid || grid.rows < 1 || grid.columns < 1)
		return;

	float displayW = spriteSheet.width * display.scale;
	float displayH = spriteSheet.height * display.scale;
	float gridX = displayW / grid.columns;
	float gridY = displayH / grid.rows;

	for (uint8_t c = 0; c <= grid.columns; c++)
	{
		float x = display.position.x + c * gridX;
		if (display.snapToPixels)
			x = roundf(x);

		float thickness = (c == 0 || c == grid.columns) ? grid.lineThickness + 1 : grid.lineThickness;

		Color color = (c == 0 || c == grid.columns) ? YELLOW : WHITE;
		DrawLineEx({x, display.position.y}, {x, display.position.y + displayH}, thickness, color);
	}

	for (uint8_t r = 0; r <= grid.rows; r++)
	{
		float y = display.position.y + r * gridY;

		if (display.snapToPixels)
			y = roundf(y);

		float thickness = (r == 0 || r == grid.rows) ? grid.lineThickness + 1 : grid.lineThickness;
		Color color = (r == 0 || r == grid.rows) ? YELLOW : WHITE;
		DrawLineEx({display.position.x, y}, {display.position.x + displayW, y}, thickness, color);
	}

	if (display.showCellInfo)
	{
		for (uint8_t r = 0; r < grid.rows; r++)
		{
			for (uint8_t c = 0; c < grid.columns; c++)
			{
				int cellNumber = r * grid.columns + c;
				float x = display.position.x + c * gridX + 5;
				float y = display.position.y + r * gridY + 5;
				DrawText(TextFormat("%d", cellNumber), f2i(x), f2i(y), 12, LIGHTGRAY);
			}
		}
	}
}

void SpriteSheetCutterApp::DrawCellHighlight(float sheetW, float sheetH)
{
	if (grid.rows <= 0 || grid.columns <= 0)
		return;

	float gridX = (spriteSheet.width * display.scale) / grid.columns;
	float gridY = (spriteSheet.height * display.scale) / grid.rows;
	float x = display.position.x + selection.col * gridX;
	float y = display.position.y + selection.row * gridY;

	if (display.snapToPixels)
	{
		x = roundf(x);
		y = roundf(y);
		gridX = roundf(gridX);
		gridY = roundf(gridY);
	}

	Rectangle highlight = {x, y, gridX, gridY};

	DrawRectangleLinesEx(highlight, 3.0f, RED);
	
	float markerSize = 8.0f;
	DrawRectangle(f2i(x - markerSize / 2), f2i(y - markerSize / 2), f2i(markerSize), f2i(markerSize), RED);
	DrawRectangle(f2i(x + gridX - markerSize / 2), f2i(y - markerSize / 2), f2i(markerSize), f2i(markerSize), RED);
	DrawRectangle(f2i(x - markerSize / 2), f2i(y + gridY - markerSize / 2), f2i(markerSize), f2i(markerSize), RED);
	DrawRectangle(f2i(x + gridX - markerSize / 2), f2i(y + gridY - markerSize / 2), f2i(markerSize), f2i(markerSize), RED);
}

void SpriteSheetCutterApp::DrawEnlargedPreview(float frameW, float frameH)
{
	Rectangle src = GetFrameRect(selection.row, selection.col, frameW, frameH);
	float previewW = frameW * display.previewScale;
	float previewH = frameH * display.previewScale;
	float previewX = GetScreenWidth() - previewW - 50;
	float previewY = GetScreenHeight() - previewH - 50;

	if (display.snapToPixels)
	{
		previewX = roundf(previewX);
		previewY = roundf(previewY);
		previewW = roundf(previewW);
		previewH = roundf(previewH);
	}

	Rectangle dest = {previewX, previewY, previewW, previewH};
	DrawRectangleRec(dest, Color{0, 0, 0, 200});
	DrawTexturePro(spriteSheet, src, dest, {0, 0}, 0.0f, WHITE);
	DrawRectangleLinesEx(dest, 3.0f, GREEN);
	DrawText(TextFormat("Preview: Cell %d (%.1fx%.1f)", selection.index, frameW, frameH),
			 f2i(previewX), f2i(previewY - 25), 16, GREEN);
}

void SpriteSheetCutterApp::ExportAllFrames(char *destFileName)
{
	Image fullImage = LoadImage(texturePath.c_str());

	if (fullImage.data == nullptr)
	{
		exportFailed = true;
	}

	if (exportFailed)
		ImGui::OpenPopup("Export Failed");

	if (ImGui::BeginPopupModal("Export Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load image for export!");
		ImGui::Text("Check if the file exists at:\n%s", texturePath.c_str());
		ImGui::Separator();
		if (ImGui::Button("OK"))
		{
			exportFailed = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	int frameW = spriteSheet.width / grid.columns;
	int frameH = spriteSheet.height / grid.rows;

	const char *savePath = tinyfd_saveFileDialog(
		"Select folder by saving a dummy file",
		".png",
		0,
		NULL,
		NULL);

	if (!savePath)
		return;

	std::string fullPath = savePath;
	size_t lastSlash = fullPath.find_last_of("/\\");
	std::string folderPath = (lastSlash == std::string::npos) ? "." : fullPath.substr(0, lastSlash);

	uint8_t frameIdx = 0;

	for (uint8_t r = 0; r < grid.rows; r++)
	{
		for (uint8_t c = 0; c < grid.columns; c++)
		{
			Rectangle cropRect = GetFrameRect(r, c, static_cast<float>(frameW), static_cast<float>(frameH));
			Image frameImage = ImageFromImage(fullImage, cropRect);

			std::string filename = folderPath + "/" + destFileName + (frameIdx < 10 ? "0" : "") + std::to_string(frameIdx) + ".png";

			ExportImage(frameImage, filename.c_str());
			UnloadImage(frameImage);
			frameIdx++;
		}
	}

	UnloadImage(fullImage);
}

void SpriteSheetCutterApp::RenderUI(float frameW, float frameH)
{
	static bool save = false;
	static bool showInputBox = false;
	static char destFileName[256] = "";
	ImGui::Begin("Ultra-Accurate Sprite Sheet Splitter", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Sprite Sheet: %dx%d pixels", spriteSheet.width, spriteSheet.height);
	ImGui::SameLine(0.0f, 80.0f);
	if (ImGui::Button("Load new SpriteSheet"))
	{
		std::string selectedPath = GetFileFromDialog();
		if (!selectedPath.empty())
		{
			// Unload previous
			if (spriteSheet.id != 0)
				UnloadTexture(spriteSheet);

			// Load new
			spriteSheet = LoadTexture(texturePath.c_str());
		}
	}

	ImGui::Checkbox("Snap to Pixels", &display.snapToPixels);
	ImGui::SameLine();
	ImGui::Checkbox("Show Cell Info", &display.showCellInfo);
	ImGui::SliderFloat("Grid Line Thickness", &grid.lineThickness, 0.5f, 5.0f);

	if (ImGui::InputInt("Rows", &grid.rows))
		grid.rows = std::max(1, grid.rows);

	if (ImGui::InputInt("Columns", &grid.columns))
		grid.columns = std::max(1, grid.columns);

	ImGui::Checkbox("Show Grid", &grid.showGrid);
	ImGui::SliderFloat("Main Scale", &display.scale, 1.0f, 3.0f);
	ImGui::SliderFloat("Preview Scale", &display.previewScale, 1.0f, 10.0f);
	ImGui::DragFloat2("Position", (float *)&display.position, 1.0f, 0.0f, 0.0f, "%.1f");

	int totalCells = grid.rows * grid.columns;
	selection.index = std::clamp(selection.index, 0, totalCells - 1);
	selection.row = selection.index / grid.columns;
	selection.col = selection.index % grid.columns;

	if (ImGui::SliderInt("Select Cell", &selection.index, 0, totalCells - 1))
	{
		selection.row = selection.index / grid.columns;
		selection.col = selection.index % grid.columns;
	}
	if (ImGui::SliderInt("Row", &selection.row, 0, grid.rows - 1))
		selection.index = selection.row * grid.columns + selection.col;

	if (ImGui::SliderInt("Column", &selection.col, 0, grid.columns - 1))
		selection.index = selection.row * grid.columns + selection.col;

	ImGui::Text("Cell: %d (Row %d, Col %d)", selection.index, selection.row, selection.col);
	ImGui::Text("Frame: %.1f x %.1f", frameW, frameH);
	ImGui::Text("Display: %.1f x %.1f", frameW * display.scale, frameH * display.scale);
	ImGui::Text("Tex Coords: (%.1f, %.1f)", selection.col * frameW, selection.row * frameH);

	if (ImGui::Button("Reset Position"))
	{
		display.position = {50, 50};
		display.scale = 1.0f;
	}
	ImGui::SameLine(0.0f, 60.0f);
	if (ImGui::Button("Fit to Window"))
	{
		float maxW = static_cast<float>(GetScreenWidth()) - 400.0f;
		float maxH = static_cast<float>(GetScreenHeight()) - 150.0f;

		display.scale = std::min(maxW / spriteSheet.width, maxH / spriteSheet.height);
		display.position = {50, 50};
	}
	ImGui::SameLine(0.0f, 60.0f);
	if (ImGui::Button("Save All Frames"))
	{
		showInputBox = true;
	}

	if (showInputBox)
	{
		ImGui::InputText("Name of the file:", destFileName, IM_ARRAYSIZE(destFileName));

		if (ImGui::Button("Confirm Save"))
		{
			if (strlen(destFileName) > 0)
			{
				save = true;
				showInputBox = false;
			}
		}
	}

	if (save)
	{
		ExportAllFrames(destFileName);
		save = false;
	}

	ImGui::End();
}

SpriteSheetCutterApp::~SpriteSheetCutterApp()
{
	rlImGuiShutdown();

	if (spriteSheet.id != 0)
		UnloadTexture(spriteSheet);
		
	CloseWindow();
}