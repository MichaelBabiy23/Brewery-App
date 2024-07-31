#include "DrawThread.h"
#include "GuiMain.h"
#include <Windows.h>
#include "../../shared/ImGuiSrc/imgui.h"
#include <iostream>
#include <vector>
#include "Favorites.h"
#include "Bubble.h"


#define input_length 50


void DrawAppWindow(void* common_ptr) {
    auto common = (CommonObjects*)common_ptr;

    if (common->exit_flag)
        return;

    static int type_currentItem = -1;
    static int country_currentItem = -1;

    const char* type_options[] = {
        "micro", "nano", "regional", "brewpub", "large", "planning",
        "bar", "contract", "proprietor", "closed"
    };

    const char* country_options[] = {
        "Austria", "England", "France", "Isle of Man", "Ireland",
        "Poland", "Portugal", "Scotland", "Singapore", "South Korea", "United States"
    };

    // Set custom colors and style for a brewery theme
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.93f, 0.85f, 1.0f)); // Light beige background
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.35f, 0.20f, 0.10f, 1.0f));      // Dark brown text
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.40f, 0.20f, 1.0f));  // Warm brown buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.50f, 0.30f, 1.0f)); // Lighter brown on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.30f, 0.10f, 1.0f));  // Darker brown on click

    // Change header color
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.80f, 0.40f, 0.20f, 1.0f));  // New color for header
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.90f, 0.50f, 0.30f, 1.0f)); // Lighter brown header on hover
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.70f, 0.30f, 0.10f, 1.0f));  // Darker brown header on click

    // Set padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f); // Rounded frames for a cozy feel

    // Begin full screen window
    ImGui::Begin("Brewery Information", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

    // Update and draw bubbles in the background
    bubbleManager.Update(ImGui::GetIO().DeltaTime, ImGui::GetIO().DisplaySize);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    bubbleManager.Draw(draw_list);

    if (ImGui::BeginMenuBar()) {
        // Center title
        float windowWidth = ImGui::GetWindowWidth();
        float textWidth = ImGui::CalcTextSize("Brewery Searcher").x;
        float centerPos = (windowWidth - textWidth) * 0.5f;

        ImGui::SetCursorPosX(centerPos);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.75f, 0.76f, 0.76f));
        ImGui::Text("Brewery Searcher");
        ImGui::PopStyleColor();

        // Right-side position
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 67);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));

        // Add the button to the menu bar
        if (ImGui::Button("X")) {
            common->exit_flag = true;
            PostQuitMessage(0);
            common->cv.notify_one();
        }

        ImGui::PopStyleColor(2);

        ImGui::EndMenuBar();
    }

    // Display a welcome text
    ImGui::Text("Welcome to our brewery data base :)");

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.60f, 0.40f, 0.20f, 1.0f));          // Background of combo box
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.70f, 0.50f, 0.30f, 1.0f));   // Background when hovered
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.9f, 0.0f, 0.0f, 1.0f));    // Background when active
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));          // Background of the popup

    // URL input field with a buffer
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
                {
                    std::lock_guard<std::mutex> lock_gurd(common->mutex);
                    type_currentItem = i;
                    common->current_type = type_options[type_currentItem];
                    common->current_countries = "";
                    common->current_serach = "";
                    country_currentItem = -1;
                    common->breweries.clear();
                    common->cv.notify_one();
                }
            }
            // Set the initial focus when opening the combo (e.g. first item)
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
                {
                    std::lock_guard<std::mutex> lock_gurd(common->mutex);
                    country_currentItem = i;
                    common->current_countries = country_options[country_currentItem];
                    common->current_serach = "";
                    common->current_type = "";
                    type_currentItem = -1;
                    common->breweries.clear();
                    common->cv.notify_one();
                }
            }
            // Set the initial focus when opening the combo (e.g. first item)
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

    ImGui::PopStyleColor(4);

    if (common->data_ready and common->breweries.size() > 0) {
        ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.7f, 0.7f, 0.2f, 1.00f));
        if (ImGui::BeginTable("Breweries", 5, ImGuiTableFlags_SizingStretchProp)) {
            // Setting up table columns
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Country", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Favorite", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableHeadersRow();

            // Loop through breweries and display each one
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
                    expanded[i] = !expanded[i]; // Toggle the expanded state
                }
                if (expanded[i]) {
                    ImGui::TableNextRow(); // Move to next row for details
                    ImGui::TableSetColumnIndex(0); // Choose the first column for your details

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
        ImGui::PopStyleColor(1);
    }
    else {
        ImGui::Text("No data available or data is not ready.");
    }

    // End the ImGui window
    ImGui::End();

    // Pop style changes
    ImGui::PopStyleVar(4);
    ImGui::PopStyleColor(8);
}

void DrawThread::operator()(CommonObjects& common) {
    GuiMain(DrawAppWindow, &common);
    common.exit_flag = true;
}

void DrawThread::black_line() {
    // Adding Padding
    ImGui::Dummy(ImVec2(0, 10)); // Dummy space for padding

    // Draw a black line above the details
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Retrieve table's position
    ImVec2 table_pos = ImGui::GetCursorScreenPos();
    float table_width = ImGui::GetContentRegionAvail().x; // Total width allocated for the table

    // Height of the thick line
    float lineHeight = 4.0f; // Adjust this value for desired thickness

    // Draw a line as a rectangle for thickness
    draw_list->AddRectFilled(ImVec2(table_pos.x, ImGui::GetItemRectMax().y + 2),
        ImVec2(table_pos.x + table_width, ImGui::GetItemRectMax().y + 2 + lineHeight),
        IM_COL32(0, 0, 0, 255));

    // Adding Padding
    ImGui::Dummy(ImVec2(0, 10)); // Dummy space for padding
}
