#include "DrawThread.h"
#include "GuiMain.h"
#include <Windows.h>
#include "../../shared/ImGuiSrc/imgui.h"
#include <iostream>

#define input_length 50
void DrawAppWindow(void* common_ptr)
{
    auto common = (CommonObjects*)common_ptr;

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
    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

    // Close button at top right
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 50, 30)); // Adjust according to your needs
    ImVec4 closeButtonColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color
    ImGui::PushStyleColor(ImGuiCol_Button, closeButtonColor);
    if (ImGui::Button("X", ImVec2(20, 20))) {
        PostQuitMessage(0); // This will trigger the WM_DESTROY message
    }
    ImGui::PopStyleColor();

    // Display a welcome text
    ImGui::Text("Welcome to our brewery data base :)");

    // URL input field with a buffer
    static char buff[input_length];
    ImGui::SetNextItemWidth(input_length * 8);
    ImGui::InputText(" ", buff, sizeof(buff));
    ImGui::SameLine();
    if (ImGui::Button("Search"))
        common->url = buff;

    if (common->data_ready)
    {
        if (ImGui::BeginTable("Breweries", 4, ImGuiTableFlags_SizingStretchProp))
        {
            // Setting up table columns
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Country", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_WidthStretch); // Button column

            ImGui::TableHeadersRow();

            // Loop through breweries and display each one
            for (size_t i = 0; i < common->breweries.size(); ++i)
            {
                const auto& brewery = common->breweries[i];
                static std::vector<bool> expanded(common->breweries.size(), false);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", brewery.name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", brewery.brewery_type.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", brewery.country.c_str());

                ImGui::TableSetColumnIndex(3);
                if (ImGui::Button(("Show##" + std::to_string(i)).c_str()))
                {
                    expanded[i] = !expanded[i]; // Toggle the expanded state
                }
                if (expanded[i])
                {
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
            }
            ImGui::EndTable();
        }
        else
        {
            ImGui::Text("Failed to create table.");
        }
    }
    else
    {
        ImGui::Text("No data available or data is not ready.");
    }

    // End the ImGui window
    ImGui::End();

    // Pop style changes
    ImGui::PopStyleVar(4);
    ImGui::PopStyleColor(8);
}

void DrawThread::operator()(CommonObjects& common)
{
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