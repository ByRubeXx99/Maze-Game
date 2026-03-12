/*******************************************************************************************
*
*   raylib maze game
*
*   Procedural maze generator using Maze Grid Algorithm
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2026 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdlib.h>

#define MAZE_WIDTH 64
#define MAZE_HEIGHT 64
#define MAZE_SCALE 10.0f

#define MAX_MAZE_ITEMS 16

typedef struct Point {
    int x;
    int y;
} Point;


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

    camera2d.target = (Vector2){ player.x + player.width/2 , player.y + player.height/2 };
	camera2d.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	camera2d.rotation = 0.0f;
	camera2d.zoom = 5.0f;
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
    int score = 0;
	float gameTime = 0.0f;
    // Font font = GetFontDefault();
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
			gameTime += GetFrameTime(); // Update game time

            // Implement maze 2D player movement logic (cursors || WASD)

            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) newPlayer.x += 2.0f;
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) newPlayer.x -= 2.0f;
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) newPlayer.y -= 2.0f;
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) newPlayer.y += 2.0f;

			int cellX = (newPlayer.x - mazePosition.x) / MAZE_SCALE;
			int cellY = (newPlayer.y - mazePosition.y) / MAZE_SCALE;

            if (cellX >= 0 && cellX < imMaze.width && cellY >= 0 && cellY < imMaze.height)
            {
				Color pixel = GetImageColor(imMaze, cellX, cellY);

                if (ColorIsEqual(pixel, BLACK)) player = newPlayer;

                if (cellX == endCell.x && cellY == endCell.y) DrawText("YOU WIN!", 500, 200, 40, GREEN);
            }

			// END TODO-5
           
            // TODO-6: [1p] Camera 2D system following player movement around the map
            // Update Camera2D parameters as required to follow player and zoom control
			camera2d.target = (Vector2){ player.x + player.width/2, player.y + player.height/2 }; // Camera target follows player
			camera2d.zoom += GetMouseWheelMove() * 0.1f; // Camera zoom controls

            if (camera2d.zoom < 0.3f) camera2d.zoom = 0.3f;
			if (camera2d.zoom > 3.0f) camera2d.zoom = 3.0f;
			// END TODO-6
            
            // TODO-7: [2p] Maze items pickup logic
            /*
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
            */
            Color pixelR = GetImageColor(imMaze, cellX, cellY);
            
            if (ColorIsEqual(pixelR, RED))
            {
                score += 10;
				ImageDrawPixel(&imMaze, cellX, cellY, BLACK); // Remove item from maze

                UnloadTexture(texMaze);
				texMaze = LoadTextureFromImage(imMaze); // Update texture to reflect item removal
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

            bool changed = false;

            if (selectedCell.x >= 0 && selectedCell.x < imMaze.width && selectedCell.y >= 0 && selectedCell.y < imMaze.height)
            {
                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !IsKeyDown(KEY_LEFT_CONTROL))
                {
                    ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, RAYWHITE);
                    changed = true;
                }

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, BLACK);
                    changed = true;
                }

                if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
                {
                    ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, RED);
                    changed = true;
                }

                if (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                {
                    ImageDrawPixel(&imMaze, selectedCell.x, selectedCell.y, GREEN);
                    changed = true;
                }
            }

            if (changed)
            {
                UnloadTexture(texMaze);
                texMaze = LoadTextureFromImage(imMaze);
            }
			// END TODO-8
            
            // TODO-9: [2p] Collectible map items: player score
            // Using same mechanism than maze editor, implement an items editor, registering
            // points in the map where items should be added for player pickup -> TIP: Use mazeItems[]

            if (IsKeyPressed(KEY_I))
            {
                for (int i = 0; i < MAX_MAZE_ITEMS; i++)
                {
                    if (mazeItems[i].x == 0 && mazeItems[i].y == 0) // Find first empty slot
                    {
                        mazeItems[i] = selectedCell; // Place item at selected cell
						mazeItemPicked[i] = false; // Mark item as not picked
                        break;
                    }
				}
            }

			// END TODO-9
        }

        // TODO-10: [1p] Multiple maze biomes supported
        // Implement changing between the different textures to be used as biomes
        // NOTE: For the 3d model, the current selected texture must be applied to the model material  

        if (IsKeyPressed(KEY_ONE)) currentBiome = 0;
        if (IsKeyPressed(KEY_TWO)) currentBiome = 1;
        if (IsKeyPressed(KEY_THREE)) currentBiome = 2;
        if (IsKeyPressed(KEY_FOUR)) currentBiome = 3;
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
                        Vector2 cellPosition = {
                                mazePosition.x + MAZE_SCALE * x,
                                mazePosition.y + MAZE_SCALE * y
                        };

                        if (ColorIsEqual(GetImageColor(imMaze, x, y), RAYWHITE))
                        {
                            // PARED
                            DrawTexturePro(texBiomes[currentBiome],
                                (Rectangle) {
                                    texBiomes[currentBiome].width / 2,
                                    texBiomes[currentBiome].height / 2,
                                    texBiomes[currentBiome].width / 2,
                                    texBiomes[currentBiome].height / 2
                                },
                                (Rectangle) { cellPosition.x, cellPosition.y, MAZE_SCALE, MAZE_SCALE },
                                (Vector2) { 0, 0 }, 0.0f, WHITE );
                        }
                        else
                        {
                            // SUELO
                            DrawTexturePro( texBiomes[currentBiome],
                                (Rectangle) { 0, 0,
                                    texBiomes[currentBiome].width / 2,
                                    texBiomes[currentBiome].height / 2
                                },
                                (Rectangle) {
                                cellPosition.x, cellPosition.y, MAZE_SCALE, MAZE_SCALE },
                                (Vector2) { 0, 0 }, 0.0f, WHITE );
                        }
                    }
				}
				// END TODO-11
                
                // TODO-12: Draw player rectangle or sprite at player position
                DrawRectangleRec(player, BLUE);

				// END TODO-12
                
                // TODO-13: Draw maze items 2d (using sprite texture?)
                for (int y = 0; y < imMaze.height; y++)
                {
                    for (int x = 0; x < imMaze.width; x++)
                    {
                        if (ColorIsEqual(GetImageColor(imMaze, x, y), RED))
                        {
                            DrawCircle(
                                mazePosition.x + x * MAZE_SCALE + MAZE_SCALE / 2,
                                mazePosition.y + y * MAZE_SCALE + MAZE_SCALE / 2,
                                4,
                                YELLOW
                            );
                        }
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
                DrawText("Middle click: item", 20, 160, 20, BLACK);
                DrawText("CTRL + Right click: end point", 20, 190, 20, BLACK);
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
    for (int i = 0; i < 4; i++) UnloadTexture(texBiomes[i]);
	// END TODO-17

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Generate procedural maze image, using grid-based algorithm
// NOTE: Color scheme used: WHITE = Wall, BLACK = Walkable, RED = Item
static Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance)
{
    // Image imMaze = { 0 };
	Image imMaze = GenImageColor(width, height, BLACK);
    
    // TODO-1: [1p] Implement maze image generation algorithm

	Point mazePoints[64] = { 0 };

    for (int i = 0; i < 64; i++)
    {
        mazePoints[i].x = - 1;
        mazePoints[i].y = - 1;
	}

    int mazePointCounter = 0;

    for (int y = 0; y < imMaze.height; y++)
    {
        for (int x = 0; x < imMaze.width; x++)
        {
            if ((x <= 0 || x >= imMaze.width - 1) || (y <= 0 || y >= imMaze.height - 1))
            {
                ImageDrawPixel(&imMaze, x, y, RAYWHITE);
            }
            else if ((x % spacingRows == 0) && (y % spacingCols == 0))
            {
                if (GetRandomValue(0, 100) < pointChance * 100)
                {
                    mazePoints[mazePointCounter] = (Point){ x, y };
                    mazePointCounter++;
                }
            }
        }
    } 

    Point dirIncrement[4] = {
        { 0, -1 },
        { 1, 0 },
        { 0, 1 },
        { -1, 0 }
    };

    int *indices = LoadRandomSequence(mazePointCounter, 0, mazePointCounter - 1);

    for (int i = 0; i < mazePointCounter; i++)
    {
        int index = indices[i];
        int direction = GetRandomValue(0, 3); // 0-N, 1-E, 2-S, 3-W

        Point nextPoint = { 0 };
        nextPoint.x = mazePoints[index].x;
        nextPoint.y = mazePoints[index].y;

        while (ColorIsEqual(GetImageColor(imMaze, nextPoint.x, nextPoint.y), BLACK))
        {
            ImageDrawPixel(&imMaze, nextPoint.x, nextPoint.y, RAYWHITE);
            nextPoint.x += dirIncrement[direction].x;
            nextPoint.y += dirIncrement[direction].y;
        }
    }

    UnloadRandomSequence(indices);

	// END TODO-1
    return imMaze;
}
