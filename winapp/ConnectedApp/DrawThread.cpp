#include "DrawThread.h"
#include "GuiMain.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include "Favorites.h"
#include "BubbleManager.h"

#define input_length 50

void SetupImGuiStyle();
void DrawMenuBar(CommonObjects* common);
void DisplayWelcomeText();
void DrawInputControls(CommonObjects* common, int& type_currentItem, int& country_currentItem);
void DrawBreweryTable(CommonObjects* common);
void DisplayNoDataMessage();

BubbleManager bubbleManager(100, ImVec2(1920, 1080));

// Drawing the main application window
void DrawAppWindow(void* common_ptr) {
    auto common = (CommonObjects*)common_ptr;

    if (common->exit_flag) return;

    static int type_currentItem = -1;
    static int country_currentItem = -1;

    SetupImGuiStyle();
    ImGui::Begin("Brewery Information", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowPos(ImVec2(0, 0));

    bubbleManager.Update(ImGui::GetIO().DeltaTime, ImGui::GetIO().DisplaySize);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    bubbleManager.Draw(draw_list);

    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

    DrawMenuBar(common);
    DisplayWelcomeText();
    DrawInputControls(common, type_currentItem, country_currentItem);
    DrawBreweryTable(common);

    ImGui::End();
    // Pop style changes
    ImGui::PopStyleVar(4);
    ImGui::PopStyleColor(8); // Popping 8 colors

}

// Set up ImGui styles
void SetupImGuiStyle() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.93f, 0.85f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.35f, 0.20f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.40f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.50f, 0.30f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.30f, 0.10f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.80f, 0.40f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.90f, 0.50f, 0.30f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.70f, 0.30f, 0.10f, 1.0f));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    
}

// Draw the menu bar including the title and close button
void DrawMenuBar(CommonObjects* common) {
    if (ImGui::BeginMenuBar()) {
        float windowWidth = ImGui::GetWindowWidth();
        float textWidth = ImGui::CalcTextSize("Brewery Searcher").x;
        float centerPos = (windowWidth - textWidth) * 0.5f;

        ImGui::SetCursorPosX(centerPos);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.75f, 0.76f, 0.76f));
        ImGui::Text("Brewery Searcher");
        ImGui::PopStyleColor();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 67);
        if (ImGui::Button("X")) {
            common->exit_flag = true;
            PostQuitMessage(0);
            common->cv.notify_one();
        }

        ImGui::EndMenuBar();
    }
}

// Display welcome text
void DisplayWelcomeText() {
    ImGui::Text("Welcome to our brewery data base :)");
}

// Draw input controls for search
void DrawInputControls(CommonObjects * common, int& type_currentItem, int& country_currentItem) {
    const char* type_options[] = {
        "micro", "nano", "regional", "brewpub", "large", "planning",
        "bar", "contract", "proprietor", "closed"
    };

    const char* country_options[] = {
        "Austria", "England", "France", "Isle of Man", "Ireland",
        "Poland", "Portugal", "Scotland", "Singapore", "South Korea", "United States"
    };

    static char buff[input_length];
    ImGui::SetNextItemWidth(input_length * 8);
    ImGui::InputText(" ", buff, sizeof(buff));
    ImGui::SameLine();

    if (ImGui::Button("Search")) {
        std::lock_guard<std::mutex> lock_gurd(common->mutex);
        common->current_serach = buff;
        common->current_countries = "";
        common->current_type = "";
        common->breweries.clear();
        common->cv.notify_one();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(170);
    if (ImGui::BeginCombo("##Select Type", type_currentItem >= 0 ? type_options[type_currentItem] : "Types")) {
        for (int i = 0; i < IM_ARRAYSIZE(type_options); i++) {
            bool isSelected = (type_currentItem == i);
            if (ImGui::Selectable(type_options[i], isSelected)) {
                std::lock_guard<std::mutex> lock_gurd(common->mutex);
                type_currentItem = i;
                common->current_type = type_options[type_currentItem];
                common->current_countries = "";
                common->current_serach = "";
                country_currentItem = -1;
                common->breweries.clear();
                common->cv.notify_one();
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(170);
    if (ImGui::BeginCombo("##Select ", country_currentItem >= 0 ? country_options[country_currentItem] : "Countries")) {
        for (int i = 0; i < IM_ARRAYSIZE(country_options); i++) {
            bool isSelected = (country_currentItem == i);
            if (ImGui::Selectable(country_options[i], isSelected)) {
                std::lock_guard<std::mutex> lock_gurd(common->mutex);
                country_currentItem = i;
                common->current_countries = country_options[country_currentItem];
                common->current_serach = "";
                common->current_type = "";
                type_currentItem = -1;
                common->breweries.clear();
                common->cv.notify_one();
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    if (ImGui::Button("Favorites")) {
        auto favorites = getFavorites();
        common->breweries = favorites;
        common->data_ready = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        std::lock_guard<std::mutex> lock_gurd(common->mutex);
        buff[0] = '\0';
        common->current_countries = "";
        common->current_serach = "";
        common->current_type = "";
        common->breweries.clear();
        common->reset = true;
        common->cv.notify_one();
    }
}

// Draw the breweries table
void DrawBreweryTable(CommonObjects* common) {
    if (common->data_ready && !common->breweries.empty()) {
        ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.7f, 0.7f, 0.2f, 1.00f));
        if (ImGui::BeginTable("Breweries", 5, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Country", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Favorite", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableHeadersRow();

            static std::vector<bool> expanded(common->breweries.size(), false);
            expanded.resize(common->breweries.size(), false);

            for (size_t i = 0; i < common->breweries.size(); ++i) {
                const auto& brewery = common->breweries[i];

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", brewery.name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", brewery.brewery_type.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", brewery.country.c_str());

                ImGui::TableSetColumnIndex(3);
                if (ImGui::Button(("Show##" + std::to_string(i)).c_str())) {
                    expanded[i] = !expanded[i];
                }
                if (expanded[i]) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    ImGui::Text("State: %s", brewery.state.c_str());
                    ImGui::Text("City: %s", brewery.city.c_str());
                    ImGui::Text("Street: %s", brewery.street.c_str());
                    ImGui::Text("Postal Code: %s", brewery.postal_code.c_str());
                    ImGui::Text("Phone: %s", brewery.phone.c_str());
                    ImGui::Text("Website: %s", brewery.website_url.c_str());

                    DrawThread::black_line();
                }

                ImGui::TableSetColumnIndex(4);
                bool favorite = isFavorite(brewery.id);
                if (ImGui::Checkbox(("##favorite" + std::to_string(i)).c_str(), &favorite)) {
                    toggleFavorite(brewery);
                }
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleColor();
    }
    else {
        DisplayNoDataMessage();
    }
}

// Display message if no data is available
void DisplayNoDataMessage() {
    ImGui::Text("No data available or data is not ready.");
}

void DrawThread::operator()(CommonObjects& common) {
    GuiMain(&DrawAppWindow, &common);
    common.exit_flag = true;
}

void DrawThread::black_line() {
    ImGui::Dummy(ImVec2(0, 10)); // Dummy space for padding
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 table_pos = ImGui::GetCursorScreenPos();
    float table_width = ImGui::GetContentRegionAvail().x;
    float lineHeight = 4.0f;

    draw_list->AddRectFilled(ImVec2(table_pos.x, ImGui::GetItemRectMax().y + 2),
        ImVec2(table_pos.x + table_width, ImGui::GetItemRectMax().y + 2 + lineHeight),
        IM_COL32(0, 0, 0, 255));

    ImGui::Dummy(ImVec2(0, 10)); // Dummy space for padding
}
