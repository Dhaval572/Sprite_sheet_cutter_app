#include "SpriteSheetCutterApp.h"

SpriteSheetCutterApp::SpriteSheetCutterApp()
{
	texturePath = "";
	exportFailed = false;
}

std::string SpriteSheetCutterApp::GetFileFromDialog()
{
	static const char *fileTypes[] = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};

	const char *result = tinyfd_openFileDialog
	(
		"Select an image",
		"",
		4,
		fileTypes,
		"Image files",
		0
	);

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
	InitWindow(1200, 700, "Sprite Sheet Splitter");
	SetTargetFPS(120);
	rlImGuiSetup(true);
	ImCustomTheme();

	while (!WindowShouldClose())
	{
		Draw();
	}
}

void SpriteSheetCutterApp::Draw()
{
	frame.width = spriteSheet.width / static_cast<float>(grid.columns);
	frame.height = spriteSheet.height / static_cast<float>(grid.rows);

	BeginDrawing();
	ClearBackground(GRAY);

	if (spriteSheet.id != 0)
	{
		SetTextureFilter(spriteSheet, TEXTURE_FILTER_POINT);
		DrawTexturePro
		(
			spriteSheet, {0, 0, static_cast<float>(spriteSheet.width), static_cast<float>(spriteSheet.height)},
			{display.position.x, display.position.y, spriteSheet.width * display.scale, spriteSheet.height * display.scale},
			{0, 0}, 0.0f, WHITE
		);

		DrawGridOverlay(frame.width, frame.height);
		DrawCellHighlight(frame.width, frame.height);
		DrawEnlargedPreview(frame.width, frame.height);
	}

	rlImGuiBegin();

	if (spriteSheet.id != 0)
	{
		RenderUI(frame.width, frame.height);
	}
	else
	{
		ImGuiIO &io = ImGui::GetIO();
		ImVec2 windowSize(300, 220);
		ImVec2 centerPos = ImVec2
		(
			(io.DisplaySize.x - windowSize.x) * 0.5f,
			(io.DisplaySize.y - windowSize.y) * 0.5f
		);

		ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

		ImGui::Begin("Startup Menu", nullptr, ImGuiWindowFlags_NoResize);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
		ImGui::TextWrapped("Select image from your pc");
		ImGui::NewLine();
		ImGui::TextWrapped("Choose only (.png, .jpg, .jpeg, .bmp )");
		ImGui::PopStyleColor();

		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::NewLine();

		ImGui::SameLine(0.0f, 90.0f);
		if (ImGui::Button("Load Sprite"))
		{
			std::string selectedPath = GetFileFromDialog();
			if (!selectedPath.empty())
			{
				spriteSheet = LoadTexture(selectedPath.c_str());
			}
		}

		ImGui::End();

		if (!texturePath.empty() && spriteSheet.id == 0)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load texture!\n Please select an image file only.");
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

	float x, y, thickness;
	frame.displayW = spriteSheet.width * display.scale;
	frame.displayH = spriteSheet.height * display.scale;
	frame.gridX = frame.displayW / grid.columns;
	frame.gridY = frame.displayH / grid.rows;

	for (int c = 0; c <= grid.columns; c++)
	{
		x = display.position.x + c * frame.gridX;

		thickness = (c == 0 || c == grid.columns) ? grid.lineThickness + 1 : grid.lineThickness;

		Color color = (c == 0 || c == grid.columns) ? YELLOW : WHITE;
		DrawLineEx
		(
			{x, display.position.y},
			{x, display.position.y + frame.displayH},
			thickness, color
		);
	}

	for (int r = 0; r <= grid.rows; r++)
	{
		y = display.position.y + r * frame.gridY;

		thickness = (r == 0 || r == grid.rows) ? grid.lineThickness + 1 : grid.lineThickness;

		Color color = (r == 0 || r == grid.rows) ? YELLOW : WHITE;
		DrawLineEx
		(
			{display.position.x, y}, 
			{display.position.x + frame.displayW, y},
			thickness, color
		);
	}

	int cellNumber;
	if (display.showCellInfo)
	{
		for (int r = 0; r < grid.rows; r++)
		{
			for (int c = 0; c < grid.columns; c++)
			{
				cellNumber = r * grid.columns + c;
				x = display.position.x + c * frame.gridX + 5;
				y = display.position.y + r * frame.gridY + 5;
				DrawText
				(
					TextFormat("%d", cellNumber), f2i(x), f2i(y), 12, LIGHTGRAY
				);
			}
		}
	}
}

void SpriteSheetCutterApp::DrawCellHighlight(float sheetW, float sheetH)
{
	float x, y, markerSize = 8.0f;
	if (grid.rows <= 0 || grid.columns <= 0)
		return;

	frame.gridX = (spriteSheet.width * display.scale) / grid.columns;
	frame.gridY = (spriteSheet.height * display.scale) / grid.rows;
	x = display.position.x + selection.col * frame.gridX;
	y = display.position.y + selection.row * frame.gridY;

	Rectangle highlight = {x, y, frame.gridX, frame.gridY};

	DrawRectangleLinesEx(highlight, 3.0f, RED);
	DrawRectangle
	(
		f2i(x - markerSize / 2), f2i(y - markerSize / 2), f2i(markerSize), f2i(markerSize), RED
	);

	DrawRectangle
	(
		f2i(x + frame.gridX - markerSize / 2), f2i(y - markerSize / 2), f2i(markerSize), f2i(markerSize), RED
	);

	DrawRectangle
	(
		f2i(x - markerSize / 2), f2i(y + frame.gridY - markerSize / 2), f2i(markerSize), f2i(markerSize), RED
	);

	DrawRectangle
	(
		f2i(x + frame.gridX - markerSize / 2), f2i(y + frame.gridY - markerSize / 2), f2i(markerSize), f2i(markerSize), RED
	);
}

void SpriteSheetCutterApp::DrawEnlargedPreview(float frameW, float frameH)
{
	float previewW, previewH, previewX, previewY;
	Rectangle src = GetFrameRect(selection.row, selection.col, frameW, frameH);
	previewW = frameW * display.previewScale;
	previewH = frameH * display.previewScale;
	previewX = GetScreenWidth() - previewW - 50;  // 50 padding from right side
	previewY = GetScreenHeight() - previewH - 50; // 50 padding from bottom side

	Rectangle dest = {previewX, previewY, previewW, previewH};
	DrawRectangleRec(dest, Color{0, 0, 0, 200});
	DrawTexturePro(spriteSheet, src, dest, {0, 0}, 0.0f, WHITE);
	DrawRectangleLinesEx(dest, 3.0f, GREEN);
	DrawText
	(
		TextFormat("Preview: Cell %d (%.1fx%.1f)", selection.index, frameW, frameH), f2i(previewX), f2i(previewY - 25), 16, GREEN
	);
}

void SpriteSheetCutterApp::ExportAllFrames(const char *destFileName)
{
	Image fullImage = LoadImage(texturePath.c_str());

	if (fullImage.data == nullptr)
	{
		exportFailed = true;
		ImGui::OpenPopup("Export Failed");
		return;
	}

	if (ImGui::BeginPopupModal("Export Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "	Failed to load image for export!");
		ImGui::Text("Check if the file exists at:\n%s", texturePath.c_str());

		ImGui::Separator();
		if (ImGui::Button("OK"))
		{
			exportFailed = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	frame.width = spriteSheet.width / grid.columns;
	frame.height = spriteSheet.height / grid.rows;

	const char *savePath = tinyfd_saveFileDialog
	(
		"Select folder",
		".png",
		0,
		NULL,
		NULL
	);

	if (!savePath)
		return;

	std::string folderPath = fs::path(savePath).parent_path().string();
	int frameIdx = 0;
	for (int r = 0; r < grid.rows; r++)
	{
		for (int c = 0; c < grid.columns; c++)
		{
			Rectangle cropRect = GetFrameRect(r, c, static_cast<float>(frame.width), static_cast<float>(frame.height));
			Image frameImage = ImageFromImage(fullImage, cropRect);

			std::string filename = folderPath + "/" + destFileName + "_(" + std::to_string(frameIdx) + ").png";

			ExportImage(frameImage, filename.c_str());
			UnloadImage(frameImage);
			frameIdx++;
		}
	}

	UnloadImage(fullImage);
}

void SpriteSheetCutterApp::RenderUI(float frameW, float frameH)
{
	float maxW, maxH;
	static bool save = false;
	static bool showInputBox = false;
	static char destFileName[256] = "";
	ImGui::Begin("Sprite Sheet Splitter", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Sprite Sheet: %dx%d pixels", spriteSheet.width, spriteSheet.height);
	ImGui::SameLine(0.0f, 120.0f);
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
	ImGui::Checkbox("Show Cell Info", &display.showCellInfo);
	ImGui::SliderFloat("Grid Line Thickness", &grid.lineThickness, 0.5f, 5.0f);

	if (ImGui::InputInt("Rows", &grid.rows))
		grid.rows = std::clamp(grid.rows, 1, 50);

	if (ImGui::InputInt("Columns", &grid.columns))
		grid.columns = std::clamp(grid.columns, 1, 50);

	ImGui::Checkbox("Show Grid", &grid.showGrid);
	ImGui::SliderFloat("Main Scale", &display.scale, 1.0f, 3.0f);
	ImGui::SliderFloat("Preview Scale", &display.previewScale, 1.0f, 10.0f);
	ImGui::InputFloat2("Position", (float *)&display.position, "%.1f");

	selection.totalCells = grid.rows * grid.columns;
	selection.index = std::clamp(selection.index, 0, selection.totalCells - 1);
	selection.row = selection.index / grid.columns;
	selection.col = selection.index % grid.columns;

	if (ImGui::SliderInt("Select Cell", &selection.index, 0, selection.totalCells - 1))
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

	ImGui::SameLine(0.0f, 70.0f);
	if (ImGui::Button("Fit to Window"))
	{
		maxW = static_cast<float>(GetScreenWidth()) - 400.0f;
		maxH = static_cast<float>(GetScreenHeight()) - 150.0f;

		display.scale = std::min(maxW / spriteSheet.width, maxH / spriteSheet.height);
		display.position = {50, 50};
	}

	ImGui::SameLine(0.0f, 70.0f);
	if (ImGui::Button("Save All Frames"))
	{
		showInputBox = true;
	}

	if (showInputBox)
	{
		ImGui::InputText("Name of the file: ", destFileName, IM_ARRAYSIZE(destFileName));

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