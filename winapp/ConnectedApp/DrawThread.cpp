// Include the header for the DrawThread class
#include "DrawThread.h"
// Include the header for the GuiMain function
#include "GuiMain.h"
// Include Windows-specific functions and types
#include <Windows.h>
// Include the input and output stream
#include <iostream>
// Include vector for dynamic array
#include <vector>
// Include the header for the Favorites functionality
#include "Favorites.h"
// Include the header for the BubbleManager class
#include "BubbleManager.h"

// Define the length of the input buffer
#define input_length 50

// Drawing the main application window
void DrawAppWindow(void* common_ptr) {

    auto common = (CommonObjects*)common_ptr;

    // If the exit flag is set, return immediately
    if (common->exit_flag) return;

    static int type_currentItem = -1;
    static int country_currentItem = -1;

    // Set up the ImGui styles
    SetupImGuiStyle();
    ImGui::Begin("Brewery Information", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowPos(ImVec2(0, 0));

    // Get the current screen size
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    ImGui::SetWindowSize(screenSize);

    // Instantiate the BubbleManager with the dynamic screen size
    static BubbleManager bubbleManager(100, screenSize);

    // Update and draw bubbles
    bubbleManager.Update(ImGui::GetIO().DeltaTime, screenSize);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    bubbleManager.Draw(draw_list);

    // Draw the menu bar, welcome text, input controls, and brewery table
    DrawMenuBar(common);
    DisplayWelcomeText();

    // Create vector to see who needs to show
    static std::vector<bool> expanded(common->breweries.size(), false);

    DrawInputControls(common, type_currentItem, country_currentItem, expanded);
    DrawBreweryTable(common, expanded);

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

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 67);
        if (ImGui::Button("X")) {
            // Set the exit flag to true and notify the condition variable
            common->exit_flag = true;
            PostQuitMessage(0);
            common->cv.notify_one(); // Notify one waiting thread
        }

        ImGui::PopStyleColor(2);

        ImGui::EndMenuBar();
    }
}

// Display welcome text
void DisplayWelcomeText() {
    ImGui::Text("Welcome to our brewery database :)");
}

// Draw input controls for search
void DrawInputControls(CommonObjects* common, int& type_currentItem, int& country_currentItem, std::vector<bool>& expanded)
{
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
        ResetExpanded(expanded);
        // Lock the mutex to protect shared data
        std::lock_guard<std::mutex> lock_guard(common->mutex);
        // Update search parameters and notify the condition variable
        common->current_search = buff;
        common->current_countries = "";
        common->current_type = "";
        country_currentItem = -1;
        type_currentItem = -1;
        common->breweries.clear();
        common->cv.notify_one(); // Notify one waiting thread
    }

    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));  // Hovered background color
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));            // Text color

    ImGui::SameLine();
    ImGui::SetNextItemWidth(170);
    if (ImGui::BeginCombo("##Select Type", type_currentItem >= 0 ? type_options[type_currentItem] : "Types")) {
        for (int i = 0; i < IM_ARRAYSIZE(type_options); i++) {
            bool isSelected = (type_currentItem == i);
            if (ImGui::Selectable(type_options[i], isSelected)) {
                ResetExpanded(expanded);
                // Lock the mutex to protect shared data
                std::lock_guard<std::mutex> lock_guard(common->mutex);
                // Update type filter and notify the condition variable
                type_currentItem = i;
                common->current_type = type_options[type_currentItem];
                common->current_countries = "";
                common->current_search = "";
                country_currentItem = -1;
                buff[0] = '\0';
                common->breweries.clear();
                common->cv.notify_one(); // Notify one waiting thread
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
                ResetExpanded(expanded);
                // Lock the mutex to protect shared data
                std::lock_guard<std::mutex> lock_guard(common->mutex);
                // Update country filter and notify the condition variable
                country_currentItem = i;
                common->current_countries = country_options[country_currentItem];
                common->current_search = "";
                common->current_type = "";               
                type_currentItem = -1;
                buff[0] = '\0';
                common->breweries.clear();
                common->cv.notify_one(); // Notify one waiting thread
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    if (ImGui::Button("Favorites")) {
        ResetExpanded(expanded);
        auto favorites = getFavorites();
        common->breweries = favorites;
        common->data_ready = true;
        country_currentItem = -1;
        type_currentItem = -1;
        common->current_search = "";
        common->current_type = "";
        buff[0] = '\0';
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        ResetExpanded(expanded);
        // Lock the mutex to protect shared data
        std::lock_guard<std::mutex> lock_guard(common->mutex);
        // Reset search parameters and notify the condition variable
        common->current_countries = "";
        common->current_type = "";
        common->current_search = "";
        type_currentItem = -1;
        country_currentItem = -1;
        buff[0] = '\0';
        common->breweries.clear();
        common->reset = true;
        common->cv.notify_one(); // Notify one waiting thread
    }
}

// Draw the table of breweries
void DrawBreweryTable(CommonObjects* common, std::vector<bool>& expanded) {
    if (common->data_ready) {
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TableHeaderBg, ImVec4(0.9f, 0.6f, 0.3f, 1.0f));
        if (ImGui::BeginTable("Breweries", 5, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Country", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Favorite", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableHeadersRow();

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

// Reset epanded states to false 
void ResetExpanded(std::vector<bool>& expanded)
{
    std::fill(expanded.begin(), expanded.end(), false);
}

// Display message if no data is available
void DisplayNoDataMessage() {
    ImGui::Text("No data available or data is not ready.");
}

// Function to draw a black line as a separator
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

// Function to handle drawing in a separate thread
void DrawThread::operator()(CommonObjects& common) {
    // Start the GUI main loop with the DrawAppWindow function and the common object
    GuiMain(&DrawAppWindow, &common);
    // Set the exit flag to true when the main loop exits
    common.exit_flag = true;
}