# Brewery Searcher Application

## Overview

This application is designed to provide a graphical interface for searching brewery information.
It leverages the ImGui library for rendering the GUI and includes various components to handle data fetching, rendering, and user interaction.

## File Structure

- `CommonObject.h` and `CommonObject.cpp`: Define shared data structures and synchronization primitives used across threads.
- `DownloadThread.h` and `DownloadThread.cpp`: Handle downloading brewery data from a remote API.
- `DrawThread.h` and `DrawThread.cpp`: Handle rendering the GUI and updating it based on user interactions.
- `GuiMain.h`: Contains the declaration for the `GuiMain` function which initializes and runs the main GUI loop.
- `Bubble.h` and `Bubble.cpp`: Define the `Bubble` class which represents an animated bubble in the background.
- `BubbleManager.h` and `BubbleManager.cpp`: Manage multiple `Bubble` objects and handle their animation.
- `Favorites.h`: Manage favorite breweries.
- `ConnectedApp.cpp`: The main entry point of the application.

## Includes and Libraries

### Standard Libraries

- `#include <iostream>`: Provides input and output stream objects.
- `#include <vector>`: Provides the `std::vector` container.
- `#include <thread>`: Provides classes and functions for managing threads.
- `#include <mutex>`: Provides classes and functions for mutexes and lock guards.
- `#include <atomic>`: Provides atomic types for safe concurrent access.
- `#include <string>`: Provides the `std::string` class.

### Third-party Libraries

- `#include "httplib.h"`: A lightweight C++ HTTP client library.
- `#include "nlohmann/json.hpp"`: A JSON library for modern C++.

### Custom Headers

- `#include "GuiMain.h"`: Declares the `GuiMain` function which initializes and runs the GUI.
- `#include "CommonObject.h"`: Defines shared data structures used across threads.
- `#include "DownloadThread.h"`: Declares the `DownloadThread` class responsible for downloading data.
- `#include "DrawThread.h"`: Declares the `DrawThread` class responsible for rendering the GUI.
- `#include "Bubble.h"`: Defines the `Bubble` class used for rendering animated bubbles.
- `#include "BubbleManager.h"`: Manages multiple `Bubble` objects for the background animation.
- `#include "Favorites.h"`: Manages favorite breweries.
- `#include <Windows.h>`: Provides Windows-specific functions and types - PostQuitMessage(0); is a function provided by the Windows API; This function posts a WM_QUIT message to the message queue, signaling the application to terminate.

## Functions

### Common Functions

#### `DownloadThread::operator()(CommonObjects& common)`
This function runs in a separate thread to handle downloading brewery data. It performs the following steps:
1. Checks if the download URL is set.
2. Creates an HTTP client with the specified URL.
3. Enters an infinite loop to continuously check for new search parameters.
4. Uses a mutex to safely retrieve and reset the current search parameters.
5. Generates the request URL based on the current parameters.
6. Sends the HTTP request and parses the JSON response.
7. Stores the parsed brewery data in the shared `common` object.
8. Waits for further instructions using a condition variable.
9. Exits if the `exit_flag` is set.

#### `DrawThread::operator()(CommonObjects& common)`
This function runs in a separate thread to handle rendering the GUI. It performs the following steps:
1. Calls the `GuiMain` function with the `DrawAppWindow` function and the shared `common` object.
2. Sets the `exit_flag` to true when the GUI loop exits.

#### `Bubble::Bubble(ImVec2 position, float radius, float speed)`
Constructor for the `Bubble` class. Initializes the position, radius, and speed of the bubble.

#### `Bubble::Update(float deltaTime, ImVec2 windowSize)`
Updates the position of the bubble based on the elapsed time (`deltaTime`). If the bubble goes off the screen, it resets its position to the bottom of the window.

#### `Bubble::Draw(ImDrawList* drawList) const`
Draws the bubble using the provided ImGui draw list.

#### `BubbleManager::BubbleManager(int count, ImVec2 windowSize)`
Constructor for the `BubbleManager` class. Initializes the bubbles with random positions, sizes, and speeds.

#### `BubbleManager::InitializeBubbles(int count, ImVec2 windowSize)`
Initializes the bubbles with random positions, sizes, and speeds. Clears any existing bubbles.

#### `BubbleManager::Update(float deltaTime, ImVec2 windowSize)`
Updates all the bubbles managed by the `BubbleManager`.

#### `BubbleManager::Draw(ImDrawList* drawList) const`
Draws all the bubbles managed by the `BubbleManager`.

### GUI Functions

#### `SetupImGuiStyle()`
Sets up the ImGui style for the application, including colors and padding.

#### `DrawMenuBar(CommonObjects* common)`
Draws the menu bar at the top of the window, including the application title and a close button.

#### `DisplayWelcomeText()`
Displays a welcome message at the top of the window.

#### `DrawInputControls(CommonObjects* common, int& type_currentItem, int& country_currentItem)`
Draws input controls for the user to search for breweries by type or country. Handles user input and updates the shared `common` object with the new search parameters.

#### `DrawBreweryTable(CommonObjects* common)`
Draws a table of breweries based on the data in the shared `common` object. Allows the user to expand rows to see more details and mark breweries as favorites.

#### `DisplayNoDataMessage()`
Displays a message indicating that no data is available or the data is not ready.

#### `DrawAppWindow(void* common_ptr)`
The main drawing function for the application window. Sets up the ImGui window, updates and draws bubbles, and calls other drawing functions like `DrawMenuBar`, `DisplayWelcomeText`, `DrawInputControls`, and `DrawBreweryTable`.

## Main Function

The `main` function initializes the application and starts the main threads for downloading data and rendering the GUI.

### Steps in the Main Function

1. **Initialize Common Objects**: Creates a `CommonObjects` instance to store shared data.
2. **Initialize Threads**: Creates instances of `DrawThread` and `DownloadThread`.
3. **Set Download URL**: Sets the URL for the download thread to fetch brewery data.
4. **Start Threads**: Starts the drawing and downloading threads using `std::jthread`.
5. **Wait for Threads to Finish**: Waits for both threads to finish execution.
6. **Exit**: Returns 0 to indicate successful execution.

## Usage

1. Compile the application using a C++ compiler.
2. Run the executable to launch the GUI.
3. Use the search controls to query brewery data by type or country.
4. View and interact with the results in the brewery table.
5. Mark breweries as favorites and manage them through the GUI.

## Dependencies

- **ImGui**: Used for rendering the GUI.
- **httplib**: Used for making HTTP requests to fetch brewery data.
- **nlohmann/json**: Used for parsing JSON responses from the API.
- **Windows API**: Used for handling window-specific functions and types.


### Workflow Schema

1. **Initialization (Main Thread)**
    - Initialize `CommonObjects`
    - Initialize `DrawThread`
    - Initialize `DownloadThread`
    - Seed the random number generator
    - Set the URL for `DownloadThread`

2. **Start Threads**
    - Start `draw_th` thread for drawing
    - Start `down_th` thread for downloading

3. **Draw Thread (`draw_th`)**
    - Call `DrawThread::operator()(CommonObjects& common)`
        - Invoke `GuiMain` with `DrawAppWindow` and `common`
        - Set `common.exit_flag` to true upon completion

4. **Download Thread (`down_th`)**
    - Call `DownloadThread::operator()(CommonObjects& common)`
        - Download data from the specified URL
        - Process and update `common` with downloaded data
        - Notify any waiting threads via `common.cv`

5. **Synchronization and Completion**
    - Main thread waits (`join()`) for `down_th` to complete
    - Main thread waits (`join()`) for `draw_th` to complete
    - Both threads complete execution and join back to the main thread
    - Main thread terminates

### Workflow Diagram

'''
+--------------------------+
|       Main Thread        |
+--------------------------+
|  Initialize CommonObjects|
|  Initialize DrawThread   |
|  Initialize DownloadThread|
|  Seed Random Generator   |
|  Set URL for Download    |
|                          |
| Start draw_th Thread     | --------------------+
| Start down_th Thread     | ----------------+   |
|                          |                 |   |
| Wait for down_th to join |                 |   |
| Wait for draw_th to join |                 |   |
|                          |                 |   |
+--------------------------+                 |   |
                                             |   |
                    +------------------------|   |
                    |   draw_th Thread       |   |
                    +------------------------+   |
                    | DrawThread::operator() |   |
                    |  -> GuiMain            |   |
                    |  -> Set exit_flag true |   |
                    +------------------------+   |
                                                 |
                     +------------------------+--|
                     |   down_th Thread       |  |
                     +------------------------+--|
                     | DownloadThread::operator()|
                     |  -> Download Data         |
                     |  -> Process Data          |
                     |  -> Update CommonObjects  |
                     |  -> Notify via cv         |
                     +------------------------+--+
 '''

### Description of Workflow Steps

1. **Initialization**
   - Common objects and threads are initialized. The random generator is seeded for randomness, and the URL for downloading is set.

2. **Start Threads**
   - Two threads (`draw_th` and `down_th`) are started. `draw_th` handles drawing operations, while `down_th` handles downloading data from a specified URL.

3. **Draw Thread (`draw_th`)**
   - Executes `DrawThread::operator()`, which calls `GuiMain` with `DrawAppWindow` and `common` as parameters. The drawing operations are performed here, and the `exit_flag` is set to true upon completion.

4. **Download Thread (`down_th`)**
   - Executes `DownloadThread::operator()`, which downloads data from the URL, processes it, updates the `common` object, and notifies any waiting threads via the condition variable (`cv`).

5. **Synchronization and Completion**
   - The main thread waits for both `draw_th` and `down_th` to complete using `join()`. Once both threads have completed their operations and joined back to the main thread, the program terminates.
