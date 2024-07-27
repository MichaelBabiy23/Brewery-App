#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <iostream>

void DrawAppWindow(void* common_ptr)
{
    auto common = (CommonObjects*)common_ptr;

    // Set custom colors and style for a brewery theme
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.93f, 0.85f, 1.0f)); // Light beige background
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.35f, 0.20f, 0.10f, 1.0f));      // Dark brown text
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.40f, 0.20f, 1.0f));  // Warm brown buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.50f, 0.30f, 1.0f)); // Lighter brown on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.30f, 0.10f, 1.0f));  // Darker brown on click
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.60f, 0.40f, 0.20f, 1.0f));  // Warm brown header
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.70f, 0.50f, 0.30f, 1.0f)); // Lighter brown header on hover
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.50f, 0.30f, 0.10f, 1.0f));  // Darker brown header on click

    // Set padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f); // Rounded frames for a cozy feel

    // Start a new ImGui window
    ImGui::Begin("Brewery Information");

    // Display a text label
    ImGui::Text("This is our brewery display callback");

    // URL input field with a buffer
    static char buff[200];
    ImGui::InputText("URL", buff, sizeof(buff));
    ImGui::SameLine();
    if (ImGui::Button("Set"))
        common->url = buff;

    // Debugging
    if (common->data_ready)
    {
        ImGui::Text("Data is ready. Displaying table...");

        if (ImGui::BeginTable("Breweries", 7))
        {
            // Setting up table columns
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Street");
            ImGui::TableSetupColumn("City");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("Country");
            ImGui::TableSetupColumn("Details"); // Button column

            ImGui::TableHeadersRow();

            // Loop through breweries and display each one in a row
            for (size_t i = 0; i < common->breweries.size(); ++i)
            {
                const auto& brewery = common->breweries[i];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", brewery.name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", brewery.brewery_type.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", brewery.street.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", brewery.city.c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%s", brewery.state.c_str());
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", brewery.country.c_str());

                ImGui::TableSetColumnIndex(6);
                if (ImGui::Button(("Details##" + std::to_string(i)).c_str()))
                {
                    common->current_brewery = &common->breweries[i];
                }
            }
            ImGui::EndTable();
        }
        else
        {
            ImGui::Text("Failed to create table.");
        }

        // If a brewery is selected, display its details
        if (common->current_brewery)
        {
            ImGui::Separator(); // Add a separator line
            ImGui::Text("Brewery Details:");
            ImGui::Text("Name: %s", common->current_brewery->name.c_str());
            ImGui::Text("Type: %s", common->current_brewery->brewery_type.c_str());
            ImGui::Text("Street: %s", common->current_brewery->street.c_str());
            ImGui::Text("City: %s", common->current_brewery->city.c_str());
            ImGui::Text("State: %s", common->current_brewery->state.c_str());
            ImGui::Text("Country: %s", common->current_brewery->country.c_str());

            // Display additional information if needed
            ImGui::Text("Postal Code: %s", common->current_brewery->postal_code.c_str());
            ImGui::Text("Phone: %s", common->current_brewery->phone.c_str());
            ImGui::Text("Website: %s", common->current_brewery->website_url.c_str());
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
