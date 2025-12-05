#include "SpriteSheetCutterApp.h"

SpriteSheetCutterApp::SpriteSheetCutterApp()
{
	m_TexturePath = "";
	m_bExportFailed = false;
}

std::string SpriteSheetCutterApp::GetFileFromDialog()
{
	static const char *FILE_TYPES[] = 
	{
		"*.png", "*.jpg", "*.jpeg", "*.bmp"
	};

	const char *RESULT = tinyfd_openFileDialog
	(
		"Select an image",
		"",
		4,
		FILE_TYPES,
		"Image files",
		0
	);

	if (RESULT != nullptr)
	{
		m_TexturePath = std::string(RESULT);
		return m_TexturePath;
	}
	return "";
}

void SpriteSheetCutterApp::run()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1200, 700, "Sprite Sheet Splitter");
	Image icon = LoadImage("assets/Screenshots/App_icon.png");
	SetWindowIcon(icon);
	UnloadImage(icon);
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
	m_Frame.width = m_SpriteSheet.width / static_cast<float>(m_Grid.columns);
	m_Frame.height = m_SpriteSheet.height / static_cast<float>(m_Grid.rows);

	BeginDrawing();
	ClearBackground(GRAY);

	if (m_SpriteSheet.id != 0)
	{
		SetTextureFilter(m_SpriteSheet, TEXTURE_FILTER_POINT);
		DrawTexturePro
		(
			m_SpriteSheet, 
			// Source rect
			{
				0, 0, 
				static_cast<float>(m_SpriteSheet.width), 
				static_cast<float>(m_SpriteSheet.height)
			},
			// Original rect
			{
				m_Display.position.x, 
				m_Display.position.y, 
				m_SpriteSheet.width * m_Display.scale, 
				m_SpriteSheet.height * m_Display.scale
			},
			{0, 0}, 0.0f, WHITE
		);

		DrawGridOverlay(m_Frame.width, m_Frame.height);
		DrawCellHighlight(m_Frame.width, m_Frame.height);
		DrawEnlargedPreview(m_Frame.width, m_Frame.height);
	}

	rlImGuiBegin();

	if (m_SpriteSheet.id != 0)
	{
		RenderUI(m_Frame.width, m_Frame.height);
	}
	else
	{
		ImGuiIO &io = ImGui::GetIO();
		ImVec2 window_size(300, 220);
		ImVec2 center_pos = ImVec2
		(
			(io.DisplaySize.x - window_size.x) * 0.5f,
			(io.DisplaySize.y - window_size.y) * 0.5f
		);

		ImGui::SetNextWindowPos(center_pos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

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
			std::string selected_path = GetFileFromDialog();
			if (!selected_path.empty())
			{
				m_SpriteSheet = LoadTexture(selected_path.c_str());
			}
		}

		ImGui::End();

		if (!m_TexturePath.empty() && m_SpriteSheet.id == 0)
		{
			ImGui::TextColored
			(
				ImVec4(1, 0, 0, 1), 
				"Failed to load texture!\n Please select an image file only."
			);
			ImGui::Text("Path: %s", m_TexturePath.c_str());
		}
	}

	rlImGuiEnd();
	EndDrawing();
}

Rectangle SpriteSheetCutterApp::GetFrameRect
(
	int row, int col, float frame_w, float frame_h
)
{
	return Rectangle
	{
		col * frame_w, 
		row * frame_h, 
		frame_w, 
		frame_h
	};
}

void SpriteSheetCutterApp::DrawGridOverlay(float frame_w, float frame_h)
{
	if (!m_Grid.b_ShowGrid || m_Grid.rows < 1 || m_Grid.columns < 1)
		return;

	float x, y, thickness;
	m_Frame.display_w = m_SpriteSheet.width * m_Display.scale;
	m_Frame.display_h = m_SpriteSheet.height * m_Display.scale;
	m_Frame.grid_x = m_Frame.display_w / m_Grid.columns;
	m_Frame.grid_y = m_Frame.display_h / m_Grid.rows;

	for (int c = 0; c <= m_Grid.columns; c++)
	{
		x = m_Display.position.x + c * m_Frame.grid_x;

		thickness = (c == 0 || c == m_Grid.columns) ? 
			m_Grid.line_thickness + 1 : m_Grid.line_thickness;

		Color color = (c == 0 || c == m_Grid.columns) ? YELLOW : WHITE;
		DrawLineEx
		(
			{x, m_Display.position.y},
			{x, m_Display.position.y + m_Frame.display_h},
			thickness, 
			color
		);
	}

	for (int r = 0; r <= m_Grid.rows; r++)
	{
		y = m_Display.position.y + r * m_Frame.grid_y;

		thickness = (r == 0 || r == m_Grid.rows) ? 
			m_Grid.line_thickness + 1 : m_Grid.line_thickness;

		Color color = (r == 0 || r == m_Grid.rows) ? YELLOW : WHITE;
		DrawLineEx
		(
			{m_Display.position.x, y}, 
			{m_Display.position.x + m_Frame.display_w, y},
			thickness, color
		);
	}

	int cell_number;
	if (m_Display.b_ShowCellInfo)
	{
		for (int r = 0; r < m_Grid.rows; r++)
		{
			for (int c = 0; c < m_Grid.columns; c++)
			{
				cell_number = r * m_Grid.columns + c;
				x = m_Display.position.x + c * m_Frame.grid_x + 5;
				y = m_Display.position.y + r * m_Frame.grid_y + 5;
				DrawText
				(
					TextFormat("%d", cell_number), 
					f2i(x), 
					f2i(y), 
					12, 
					LIGHTGRAY
				);
			}
		}
	}
}

void SpriteSheetCutterApp::DrawCellHighlight(float sheetW, float sheetH)
{
	float x, y, marker_size = 8.0f;
	if (m_Grid.rows <= 0 || m_Grid.columns <= 0) return;

	m_Frame.grid_x = (m_SpriteSheet.width * m_Display.scale) / m_Grid.columns;
	m_Frame.grid_y = (m_SpriteSheet.height * m_Display.scale) / m_Grid.rows;
	x = m_Display.position.x + m_Selection.col * m_Frame.grid_x;
	y = m_Display.position.y + m_Selection.row * m_Frame.grid_y;

	Rectangle highlight = {x, y, m_Frame.grid_x, m_Frame.grid_y};

	DrawRectangleLinesEx(highlight, 3.0f, RED);
	DrawRectangle
	(
		f2i(x - marker_size / 2), 
		f2i(y - marker_size / 2), 
		f2i(marker_size), 
		f2i(marker_size), 
		RED
	);

	DrawRectangle
	(
		f2i(x + m_Frame.grid_x - marker_size / 2), 
		f2i(y - marker_size / 2), 
		f2i(marker_size), 
		f2i(marker_size), 
		RED	
	);

	DrawRectangle
	(
		f2i(x - marker_size / 2), 
		f2i(y + m_Frame.grid_y - marker_size / 2), 
		f2i(marker_size), 
		f2i(marker_size), 
		RED
	);

	DrawRectangle
	(
		f2i(x + m_Frame.grid_x - marker_size / 2), 
		f2i(y + m_Frame.grid_y - marker_size / 2), 
		f2i(marker_size), 
		f2i(marker_size), 
		RED
	);
}

void SpriteSheetCutterApp::DrawEnlargedPreview(float frame_w, float frame_h)
{
	float preview_w, preview_h, preview_x, preview_y;
	Rectangle src = GetFrameRect
	(
		m_Selection.row, 
		m_Selection.col, 
		frame_w, 
		frame_h
	);
	preview_w = frame_w * m_Display.preview_scale;
	preview_h = frame_h * m_Display.preview_scale;

	// 50 padding from right and bottom side
	preview_x = GetScreenWidth() - preview_w - 50;  
	preview_y = GetScreenHeight() - preview_h - 50; 

	Rectangle dest = {preview_x, preview_y, preview_w, preview_h};
	DrawRectangleRec(dest, Color{0, 0, 0, 200});
	DrawTexturePro(m_SpriteSheet, src, dest, {0, 0}, 0.0f, WHITE);
	DrawRectangleLinesEx(dest, 3.0f, GREEN);
	DrawText
	(
		TextFormat
		(
			"Preview: Cell %d (%.1fx%.1f)", 
			m_Selection.index, 
			frame_w, 
			frame_h
		), 
		f2i(preview_x), f2i(preview_y - 25), 16, GREEN
	);
}

void SpriteSheetCutterApp::ExportAllFrames(const char *dest_file_name)
{
	Image full_image = LoadImage(m_TexturePath.c_str());

	if (full_image.data == nullptr)
	{
		m_bExportFailed = true;
		ImGui::OpenPopup("Export Failed");
		return;
	}

	if 
	(
		ImGui::BeginPopupModal
		(
			"Export Failed", 
			NULL,
			ImGuiWindowFlags_AlwaysAutoResize
		)
	)
	{
		ImGui::TextColored
		(
			ImVec4(1, 0, 0, 1), 
			"	Failed to load image for export!"
		);
		ImGui::Text("Check if the file exists at:\n%s", m_TexturePath.c_str());

		ImGui::Separator();
		if (ImGui::Button("OK"))
		{
			m_bExportFailed = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	m_Frame.width = m_SpriteSheet.width / m_Grid.columns;
	m_Frame.height = m_SpriteSheet.height / m_Grid.rows;

	const char *save_path = tinyfd_saveFileDialog
	(
		"Select folder",
		".png",
		0,
		NULL,
		NULL
	);

	if (!save_path) return;

	std::string folder_path = fs::path(save_path).parent_path().string();
	int frame_idx = 0;
	for (int r = 0; r < m_Grid.rows; r++)
	{
		for (int c = 0; c < m_Grid.columns; c++)
		{
			Rectangle crop_rect = GetFrameRect
			(
				r, 
				c, 
				static_cast<float>(m_Frame.width), 
				static_cast<float>(m_Frame.height)
			);
			Image frame_image = ImageFromImage(full_image, crop_rect);

			std::string filename = folder_path + "/" + dest_file_name + "_(" + std::to_string(frame_idx) + ").png";

			ExportImage(frame_image, filename.c_str());
			UnloadImage(frame_image);
			frame_idx++;
		}
	}

	UnloadImage(full_image);
}

void SpriteSheetCutterApp::RenderUI(float frame_w, float frame_h)
{
	float max_w, max_h;
	static bool s_bSave = false;
	static bool s_bShowInputBox = false;
	static char s_bDestFileName[256] = "";
	ImGui::Begin
	(
		"Sprite Sheet Splitter", 
		nullptr, 
		ImGuiWindowFlags_AlwaysAutoResize
	);
	ImGui::Text
	(
		"Sprite Sheet: %dx%d pixels", 
		m_SpriteSheet.width, 
		m_SpriteSheet.height
	);
	ImGui::SameLine(0.0f, 120.0f);
	if (ImGui::Button("Load new SpriteSheet"))
	{
		std::string selected_path = GetFileFromDialog();
		if (!selected_path.empty())
		{
			// Unload previous
			if (m_SpriteSheet.id != 0)
			{
				UnloadTexture(m_SpriteSheet);
			}

			// Load new
			m_SpriteSheet = LoadTexture(m_TexturePath.c_str());
		}
	}
	ImGui::Checkbox("Show Cell Info", &m_Display.b_ShowCellInfo);
	ImGui::SliderFloat
	(
		"Grid Line Thickness", 
		&m_Grid.line_thickness, 
		0.5f, 
		5.0f
	);

	if (ImGui::InputInt("Rows", &m_Grid.rows))
	{
		m_Grid.rows = std::clamp(m_Grid.rows, 1, 50);
	}

	if (ImGui::InputInt("Columns", &m_Grid.columns))
	{
		m_Grid.columns = std::clamp(m_Grid.columns, 1, 50);
	}

	ImGui::Checkbox("Show Grid", &m_Grid.b_ShowGrid);
	ImGui::SliderFloat("Main Scale", &m_Display.scale, 1.0f, 3.0f);
	ImGui::SliderFloat("Preview Scale", &m_Display.preview_scale, 1.0f, 10.0f);
	ImGui::InputFloat2("Position", (float *)&m_Display.position, "%.1f");

	m_Selection.total_cells = m_Grid.rows * m_Grid.columns;
	m_Selection.index = std::clamp
	(
		m_Selection.index, 
		0, 
		m_Selection.total_cells - 1
	);
	m_Selection.row = m_Selection.index / m_Grid.columns;
	m_Selection.col = m_Selection.index % m_Grid.columns;

	if 
	(
		ImGui::SliderInt
		(
			"Select Cell", 
			&m_Selection.index, 
			0, 
			m_Selection.total_cells - 1
		)
	)
	{
		m_Selection.row = m_Selection.index / m_Grid.columns;
		m_Selection.col = m_Selection.index % m_Grid.columns;
	}
	if (ImGui::SliderInt("Row", &m_Selection.row, 0, m_Grid.rows - 1))
	{
		m_Selection.index = m_Selection.row * m_Grid.columns + m_Selection.col;
	}

	if (ImGui::SliderInt("Column", &m_Selection.col, 0, m_Grid.columns - 1))
	{
		m_Selection.index = m_Selection.row * m_Grid.columns + m_Selection.col;
	}

	ImGui::Text
	(
		"Cell: %d (Row %d, Col %d)", 
		m_Selection.index, 
		m_Selection.row, 
		m_Selection.col
	);
	ImGui::Text("Frame: %.1f x %.1f", frame_w, frame_h);
	ImGui::Text
	(
		"Display: %.1f x %.1f", 
		frame_w * m_Display.scale, 
		frame_h * m_Display.scale
	);
	ImGui::Text
	(
		"Tex Coords: (%.1f, %.1f)", 
		m_Selection.col * frame_w, 
		m_Selection.row * frame_h
	);

	if (ImGui::Button("Reset Position"))
	{
		m_Display.position = {50, 50};
		m_Display.scale = 1.0f;
	}

	ImGui::SameLine(0.0f, 70.0f);
	if (ImGui::Button("Fit to Window"))
	{
		max_w = static_cast<float>(GetScreenWidth()) - 400.0f;
		max_h = static_cast<float>(GetScreenHeight()) - 150.0f;

		m_Display.scale = std::min
		(
			max_w / m_SpriteSheet.width, 
			max_h / m_SpriteSheet.height
		);
		m_Display.position = {50, 50};
	}

	ImGui::SameLine(0.0f, 70.0f);
	if (ImGui::Button("Save All Frames"))
	{
		s_bShowInputBox = true;
	}

	if (s_bShowInputBox)
	{
		ImGui::InputText
		(
			"Name of the file: ", 
			s_bDestFileName, 
			IM_ARRAYSIZE(s_bDestFileName)
		);

		if (ImGui::Button("Confirm Save"))
		{
			if (strlen(s_bDestFileName) > 0)
			{
				s_bSave = true;
				s_bShowInputBox = false;
			}
		}
	}

	if (s_bSave)
	{
		ExportAllFrames(s_bDestFileName);
		s_bSave = false;
	}

	ImGui::End();
}

SpriteSheetCutterApp::~SpriteSheetCutterApp()
{
	rlImGuiShutdown();

	if (m_SpriteSheet.id != 0)
	{
		UnloadTexture(m_SpriteSheet);
	}

	CloseWindow();
}