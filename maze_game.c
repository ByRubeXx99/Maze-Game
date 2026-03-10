/*******************************************************************************************
*
*   raylib maze game
*
*   Procedural maze generator using Maze Grid Algorithm
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2024-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdlib.h>     // Required for: malloc(), free()

#define MAZE_WIDTH          64
#define MAZE_HEIGHT         64
#define MAZE_SCALE          10.0f

#define MAX_MAZE_ITEMS      16

// Declare new data type: Point
typedef struct Point {
    int x;
    int y;
} Point;

// Generate procedural maze image, using grid-based algorithm
// NOTE: Functions defined as static are internal to the module
static Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Delivery04 - maze game");

    // Current application mode
    int currentMode = 1;    // 0-Game, 1-Editor

    // Random seed defines the random numbers generation,
    // always the same if using the same seed
    SetRandomSeed(67218);

    // Generate maze image using the grid-based generator
    // TODO-1: [1p] Implement GenImageMaze() function with required parameters
    Image imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.75f);

    // Load a texture to be drawn on screen from our image data
    // WARNING: If imMaze pixel data is modified, texMaze needs to be re-loaded
    Texture texMaze = LoadTextureFromImage(imMaze);

    // Player start-position and end-position initialization
    Point startCell = { 1, 1 };
    Point endCell = { imMaze.width - 2, imMaze.height - 2 };

    // Maze drawing position (editor mode)
    Vector2 mazePosition = {
        GetScreenWidth()/2 - texMaze.width*MAZE_SCALE/2,
        GetScreenHeight()/2 - texMaze.height*MAZE_SCALE/2
    };

    // Define player position and size
    Rectangle player = { mazePosition.x + 1*MAZE_SCALE + 2, mazePosition.y + 1*MAZE_SCALE + 2, 4, 4 };

    // Camera 2D for 2d gameplay mode
    // TODO-2: [2p] Initialize camera parameters as required
    Camera2D camera2d = { 0 };

    camera2d.target = (Vector2){ player.x, player.y };
	camera2d.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
	camera2d.rotation = 0.0f;
	camera2d.zoom = 1.0f;
    // END TODO-2

    // Mouse selected cell for maze editing
    Point selectedCell = { 0 };

    // Maze items position and state
    Point mazeItems[MAX_MAZE_ITEMS] = { 0 };
    bool mazeItemPicked[MAX_MAZE_ITEMS] = { 0 };
    
    // Define textures to be used as our "biomes"
    Texture texBiomes[4] = { 0 };
    texBiomes[0] = LoadTexture("resources/maze_atlas01.png");

    // TODO-3: Load additional textures for different biomes
	texBiomes[1] = LoadTexture("resources/maze_atlas02.png");
	texBiomes[2] = LoadTexture("resources/maze_atlas03.png");
	texBiomes[3] = LoadTexture("resources/maze_atlas04.png");
	// END TODO-3

    int currentBiome = 0;

    // TODO-4: Define all variables required for game UI elements (sprites, fonts...)
    int socre = 0;
	float gameTime = 0.0f;
    Font font = GetFontDefault();
	// END TODO-4

    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Select current mode as desired
        if (IsKeyPressed(KEY_SPACE)) currentMode = !currentMode; // Toggle mode: 0-Game, 1-Editor

        if (currentMode == 0) // Game mode
        {
            // TODO-5: [2p] Player 2D movement from predefined Start-point to End-point
            Rectangle newPlayer = player;

            // Implement maze 2D player movement logic (cursors || WASD)
            if (IsKeyDown(KEY_D)) player.x += 8.0f;
            if (IsKeyDown(KEY_A)) player.x -= 8.0f;
            if (IsKeyDown(KEY_W)) player.y -= 8.0f;
            if (IsKeyDown(KEY_S)) player.y += 8.0f;

            // Use imMaze pixel information to check collisions
			int cellX = (newPlayer.x - mazePosition.x) / MAZE_SCALE;
			int cellY = (newPlayer.y - mazePosition.y) / MAZE_SCALE;

            Color *pixels = LoadImageColors(imMaze);
			if (pixels[cellY*imMaze.width + cellX].r == 0) player = newPlayer; // Check if the cell is walkable (BLACK)

            UnloadImageColors(pixels);

            // Detect if current playerCell == endCell to finish game
			if (cellX == endCell.x && cellY == endCell.y) DrawText("YOU WIN!", 500, 200, 40, GREEN);
			// END TODO-5
           
            // TODO-6: [1p] Camera 2D system following player movement around the map
            // Update Camera2D parameters as required to follow player and zoom control
			camera2d.target = (Vector2){ player.x, player.y }; // Camera target follows player
            camera2d.zoom = expf(logf(camera2d.zoom) + ((flaot)GetMouseWheelMove() * 0.1f)); // Camera zoom controls
			// END TODO-6
            
            // TODO-7: [2p] Maze items pickup logic
            for (int i = 0; i < MAX_MAZE_ITEMS; i++)
            {
                if (!mazeItemPicked[i])
                {
                    Rectangle itemRect = { mazePosition.x + mazeItems[i].x * MAZE_SCALE, mazePosition.y + mazeItems[i].y * MAZE_SCALE, MAZE_SCALE, MAZE_SCALE }; // MAZE_SCALE --> 4

                    if (CheckCollisionRecs(player, itemRect))
                    {
                        mazeItemPicked[i] = true; // Mark item as picked
                        score += 10; // Increase player score
                    }
				}
            }
			// END TODO-7
        }
        else if (currentMode == 1) // Editor mode
        {
            // TODO-8: [2p] Maze editor mode, edit image pixels with mouse.
            // Implement logic to selecte image cell from mouse position -> TIP: GetMousePosition()
            // NOTE: Mouse position is returned in screen coordinates and it has to 
            // transformed into image coordinates
            // Once the cell is selected, if mouse button pressed add/remove image pixels
            
            // WARNING: Remember that when imMaze changes, texMaze must be also updated!

			Vector2 mouse = GetMousePosition();

			selectedCell.x = (mouse.x - mazePosition.x) / MAZE_SCALE;
			selectedCell.y = (mouse.y - mazePosition.y) / MAZE_SCALE;

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, RAYWHITE); // Add wall
			if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, BLACK); // Remove wall

            UnloadTexture(texMaze);
			texMaze = LoadTextureFromImage(imMaze);
			// END TODO-8
            
            // TODO-9: [2p] Collectible map items: player score
            // Using same mechanism than maze editor, implement an items editor, registering
            // points in the map where items should be added for player pickup -> TIP: Use mazeItems[]
        }

        // TODO-10: [1p] Multiple maze biomes supported
        // Implement changing between the different textures to be used as biomes
        // NOTE: For the 3d model, the current selected texture must be applied to the model material  

        if (IsKeyPressed(KEY_B))
        {
            currentBiome++;
            if (currentBiome > 3) currentBiome = 0;
		}
		// END TODO-10
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (currentMode == 0) // Game mode
            {
                // Draw maze using camera2d (for automatic positioning and scale)
                BeginMode2D(camera2d);

                // TODO-11: Draw maze walls and floor using current texture biome

                for (int y = 0; y < imMaze.height; y++)
                {
                    for (int x = 0; x < imMaze.width; x++)
                    {
                        Color pixelColor = GetImageColor(imMaze, x, y);
                        if (pixelColor.r == 255) // Wall
                        {
                            DrawTexture(texBiomes[currentBiome], mazePosition.x + x * MAZE_SCALE, mazePosition.y + y * MAZE_SCALE, WHITE);
                        }
                        else if (pixelColor.r == 0) // Floor
                        {
                            DrawTexture(texBiomes[currentBiome], mazePosition.x + x * MAZE_SCALE, mazePosition.y + y * MAZE_SCALE, WHITE);
                        }
                    }
				}
				// END TODO-11
                
                // TODO-12: Draw player rectangle or sprite at player position
                DrawRectangleRec(player, BLUE);
				// END TODO-12
                
                // TODO-13: Draw maze items 2d (using sprite texture?)
                for (int i = 0; i < MAX_MAZE_ITEMS; i++)
                {
                    if (!mazeItemPicked[i])
                    {
                        DrawCircle(mazePosition.x + mazeItems[i].x*MAZE_SCALE + MAZE_SCALE/2, mazePosition.y + mazeItems[i].y*MAZE_SCALE + MAZE_SCALE/2, 4, YELLOW);
                    }
				}
				// END TODO-13
                EndMode2D();

                // TODO-14: Draw game UI (score, time...) using custom sprites/fonts
                // NOTE: Game UI does not receive the camera2d transformations,
                // it is drawn in screen space coordinates directly

				DrawText(TextFormat("Score: %i", score), 20, 40, 20, BLACK);
				DrawText(TextFormat("Time: %.2f", gameTime), 20, 70, 20, BLACK);
				// END TODO-14
            }
            else if (currentMode == 1) // Editor mode
            {
                // Draw generated maze texture, scaled and centered on screen 
                DrawTextureEx(texMaze, mazePosition, 0.0f, MAZE_SCALE, WHITE);

                // Draw lines rectangle over texture, scaled and centered on screen 
                DrawRectangleLines(mazePosition.x, mazePosition.y, MAZE_WIDTH*MAZE_SCALE, MAZE_HEIGHT*MAZE_SCALE, RED);

                // TODO-15: Draw player using a rectangle, consider maze screen coordinates!
                DrawRectangleRec(player, BLUE);
				// END TODO-15
                
                // TODO-16: Draw editor UI required elements
				DrawText("EDITOR MODE", 20, 100, 20, RED);
                DrawText("Left click: wall", 20, 130, 20, BLACK);
                DrawText("Right click: path", 20, 160, 20, BLACK);
                DrawText("Press I to place item", 20, 190, 20, BLACK);
				// END TODO-16
            }

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texMaze);     // Unload maze texture from VRAM (GPU)
    UnloadImage(imMaze);        // Unload maze image from RAM (CPU)

    // TODO-17: Unload all loaded resources
	UnloadTexture(texBiomes[0]);
	UnloadTexture(texBiomes[1]);
	UnloadTexture(texBiomes[2]);
	UnloadTexture(texBiomes[3]);
	// END TODO-17

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Generate procedural maze image, using grid-based algorithm
// NOTE: Color scheme used: WHITE = Wall, BLACK = Walkable, RED = Item
static Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance)
{
    Image imMaze = { 0 };
    
    // TODO-1: [1p] Implement maze image generation algorithm
    Color* pixels = LoadImageColors(imMaze);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x % spacingCols == 0 || y % spacingRows == 0)
            {
                pixels[y * width + x] = RAYWHITE;

                if ((GetRandomValue(0, 100) / 100.0f) < pointChance) pixels[y * width + x] = BLACK;
            }
        }
    }

    for (int x = 0; x < width; x++)
    {
        pixels[x] = RAYWHITE;
        pixels[(height - 1) * width + x] = RAYWHITE;
    }

    for (int y = 0; y < height; y++)
    {
        pixels[y * width] = RAYWHITE;
        pixels[y * width + width - 1] = RAYWHITE;
    }

    UpdateImageColors(&imMaze, pixels);
    UnloadImageColors(pixels);
	// END TODO-1
    return imMaze;
}
