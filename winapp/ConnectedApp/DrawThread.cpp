#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <iostream>

void DrawAppWindow(void* common_ptr)
{
    auto common = (CommonObjects*)common_ptr;

    // Set custom colors and style for a classic look
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.22f, 0.27f, 1.0f));  // Dark grey background
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));      // Light grey text
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.45f, 0.55f, 1.0f));  // Subdued blue-grey buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.55f, 0.65f, 1.0f)); // Lighter blue-grey on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.35f, 0.45f, 1.0f));  // Darker blue-grey on click
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.45f, 0.55f, 1.0f));  // Blue-grey header
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.5f, 0.55f, 0.65f, 1.0f)); // Lighter blue-grey header on hover
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.3f, 0.35f, 0.45f, 1.0f));  // Darker blue-grey header on click

    // Set padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded frames for a classic touch

    // Start a new ImGui window
    ImGui::Begin("Connected!");

    // Display a text label
    ImGui::Text("This is our draw callback");

    // URL input field with a buffer
    static char buff[200];
    ImGui::InputText("URL", buff, sizeof(buff));
    ImGui::SameLine();
    if (ImGui::Button("Set"))
        common->url = buff;

    // Debugging
    if (common->data_ready)
    {
        ImGui::Text("Data is ready. Displaying table..."); // Debug message

        if (ImGui::BeginTable("Breweries", 10)) // Updated table with 10 columns
        {
            // Setting up table columns
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Street");
            ImGui::TableSetupColumn("City");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("Postal Code");
            ImGui::TableSetupColumn("Country");
            ImGui::TableSetupColumn("Phone");
            ImGui::TableSetupColumn("Website");

            ImGui::TableHeadersRow();

            // Loop through breweries and display each one in a row
            for (const auto& brewery : common->breweries)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", brewery.id.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", brewery.name.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", brewery.brewery_type.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", brewery.street.c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%s", brewery.city.c_str());
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", brewery.state.c_str());
                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%s", brewery.postal_code.c_str());
                ImGui::TableSetColumnIndex(7);
                ImGui::Text("%s", brewery.country.c_str());
                ImGui::TableSetColumnIndex(8);
                ImGui::Text("%s", brewery.phone.c_str());
                ImGui::TableSetColumnIndex(9);
                ImGui::Text("%s", brewery.website_url.c_str());
            }
            ImGui::EndTable();
        }
        else
        {
            ImGui::Text("Failed to create table."); // Debug message
        }

        // If a brewery is selected, display its details
        if (common->current_brewery)
        {
            ImGui::Separator(); // Add a separator line
            ImGui::Text("Brewery Details:");
            ImGui::Text("ID: %s", common->current_brewery->id.c_str());
            ImGui::Text("Name: %s", common->current_brewery->name.c_str());
            ImGui::Text("Type: %s", common->current_brewery->brewery_type.c_str());
            ImGui::Text("Street: %s", common->current_brewery->street.c_str());
            ImGui::Text("City: %s", common->current_brewery->city.c_str());
            ImGui::Text("State: %s", common->current_brewery->state.c_str());
            ImGui::Text("Postal Code: %s", common->current_brewery->postal_code.c_str());
            ImGui::Text("Country: %s", common->current_brewery->country.c_str());
            ImGui::Text("Phone: %s", common->current_brewery->phone.c_str());
            ImGui::Text("Website: %s", common->current_brewery->website_url.c_str());
        }
    }
    else
    {
        ImGui::Text("No data available or data is not ready."); // Debug message
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
