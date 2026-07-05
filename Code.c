/**************************************************************
                    ESCAPE CADT

Course : Algorithm & Computational Thinking II
Project : Escape CADT
Group  : 5

Members:
- Khon Somony
- Thy Reaksmey

Description:
Escape CADT is a 2D puzzle adventure game developed using C
and raylib. The player explores the campus, collects items,
solves puzzles, and chooses one of multiple endings.

**************************************************************/

//==============================================================
// SECTION 1 : Header Files
//==============================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

//==============================================================
// SECTION 2 : Game Constants
//==============================================================
// Map Dimensions
#define P1_ROWS 10
#define P1_COLS 15
#define ROOM_ROWS 8
#define ROOM_COLS 10
#define CELL_SIZE 40
#define HALL_ROWS 14
#define HALL_COLS 22
// Color Palette
#define COLOR_BG RAYWHITE 
#define COLOR_WALL DARKGRAY
#define COLOR_FLOOR LIGHTGRAY
#define COLOR_TEXT_MUTED BLACK
#define COLOR_TEXT_LIGHT BLACK
#define COLOR_NEON_AMBER ORANGE
#define COLOR_CAT_PINK RED
#define COLOR_MINT_GREEN GREEN
#define COLOR_DOOR_GLOW BLUE
#define COLOR_INTERACTIVE PURPLE

#define MAX_PUZZLE_INPUT_LENGTH 32
#define TOTAL_PUZZLES 10

//==============================================================
// SECTION 3 : Game States
//==============================================================
typedef enum {
    STATE_INTRO,
    STATE_PHASE1,
    STATE_WIN_PHASE1,
    STATE_MAIN_HALL,
    STATE_CLASSROOM,
    STATE_LIBRARY,
    STATE_COMPUTER_LAB,
    STATE_READING_LOGS,
    STATE_RESEARCH_ROOM,
    STATE_ADMIN_ROOM,
    STATE_FACULTY_ROOM,
    STATE_SERVER_ROOM,
    STATE_SERVER_CONFRONTATION,
    STATE_PUZZLE_INTERFACE,
    STATE_ENDING_SCREEN,
    STATE_GAMEOVER
} GameState;

GameState currentState = STATE_INTRO;
GameState finalSelectedEnding = STATE_INTRO;

//==============================================================
// SECTION 4 : Data Structures
//==============================================================
typedef struct {
    int x;
    int y;
    int timer;
    int hasLabCard;
    int hasFacultyCard;
    int hasAdminCard;
    int hasServerCard;
    int puzzlesSolved;
} Player;

typedef struct {
    char question[150];
    char answer[50];
    int solved;
} Puzzle;

//==============================================================
// SECTION 5 : Map Layouts
//==============================================================
char phase1Map[P1_ROWS][P1_COLS + 1] = {
    "###############",
    "#........#....#",
    "#.####.#.####.#",
    "#.#....#......#",
    "#.#.#####.#####",
    "#.#.#.........#",
    "#...#.#######.#",
    "###.#.#.......#",
    "#...#...####S.#",
    "###############"
};

char mainHallMap[HALL_ROWS][HALL_COLS+1] = {
    "######################",
    "#.C..............C...#",
    "#.........############",
    "#.........L..........#",
    "########..#####R.....#",
    "#....................#",
    "##..##...######..#####",
    "#......#.....###.....#",
    "#..M...#..#.#.S.#S...#",
    "#..#...#...#M....#...#",
    "#....A.#...#L...F#..#",
    "####...#....#C.......#",
    "#............###.....#",
    "######################"
};

char classroomMap[ROOM_ROWS][ROOM_COLS + 1] = {
    "##########",
    "#........#",
    "#.#####..#",
    "#...D....#",
    "#........#",
    "#........#",
    "####E#####",
    "##########"
};

char libraryMap[ROOM_ROWS][ROOM_COLS + 1] = {
    "##########",
    "#.B..B..B#",
    "#........#",
    "####..####",
    "#...A....#",
    "#........#",
    "####E#####",
    "##########"
};

char computerLabMap[ROOM_ROWS][ROOM_COLS + 1] = {
    "##########",
    "#.######.#",
    "#.#....#.#",
    "#.#.T..#.#",
    "#........#",
    "#........#",
    "####E#####",
    "##########"
};

char serverRoomMap[ROOM_ROWS][ROOM_COLS + 1] = {
    "##########",
    "#..X.....#",
    "#........#",
    "#..#####.#",
    "#.....L..#",
    "#........#",
    "####E#####",
    "##########"
};

char researchMap[ROOM_ROWS][ROOM_COLS+1]={
    "##########",
    "#........#",
    "#....K...#",
    "#........#",
    "#........#",
    "#........#",
    "####E#####",
    "##########"
};

char adminMap[ROOM_ROWS][ROOM_COLS+1]={
    "##########",
    "#........#",
    "#....D...#",
    "#........#",
    "#........#",
    "#........#",
    "####E#####",
    "##########"
};

char facultyMap[ROOM_ROWS][ROOM_COLS+1]={
    "##########",
    "#........#",
    "#....F...#",
    "#........#",
    "#........#",
    "#........#",
    "####E#####",
    "##########"
};

//==============================================================
// SECTION 6 : Global Variables
//==============================================================
Player player = { 1, 1, 25, 0, 0, 0, 0, 0 };

int savedHallX = 3;
int savedHallY = 4;
int labPuzzleSolved = 0;
int currentPuzzleIndex = 0;
int selectedPuzzles[3];
int talkedToAI = 0;
int readLogs = 0;

char puzzleInput[MAX_PUZZLE_INPUT_LENGTH] = "\0";
int inputLetterCount = 0;
const char* puzzleFeedback = "";

double gameStartTime = 0.0;
double totalElapsedTime = 0.0;
bool isTimerRunning = false;
float personalBestTime = 999999.0f;
const char* hudNotification = NULL;
Color puzzleFeedbackColor = COLOR_CAT_PINK;

//==============================================================
// SECTION 7 : Function Prototypes
//==============================================================
Puzzle puzzles[TOTAL_PUZZLES] = {
    { "Convert Decimal 10 to Binary", "1010", 0 },
    { "Convert Decimal 13 to Binary", "1101", 0 },
    { "What does CPU stand for?\n(lowercase, no spaces)","centralprocessingunit", 0 },
    { "What does RAM stand for?\n(lowercase, no spaces)", "randomaccessmemory", 0 },
    { "What protocol secures websites?\n(lowercase)","https", 0 },

    { "I have keys but no locks.\nI have space but no room.\nYou use me to type.\n(lowercase)","keyboard", 0 },
    { "Which is NOT a programming language?\nA. Python\nB. C\nC. HTML\nD. Java\n(Type A/B/C/D)",  "c", 0 },
    { "Which shortcut copies text?\nA. Ctrl+C\nB. Ctrl+V\nC. Ctrl+X\nD. Ctrl+Z\n(Type A/B/C/D)",   "a", 0 },
    { "Complete the sequence:\n2 4 8 16 ?",   "32", 0 },
    { "I send Internet to your devices.\n6 letters.\n(lowercase)",  "router", 0 }
};
void ResetGame();
void HandleRoomMovement(char roomMap[ROOM_ROWS][ROOM_COLS + 1], GameState exitState);
void DrawRoomMap(char roomMap[ROOM_ROWS][ROOM_COLS + 1], const char* roomTitle);
void UpdateAndDrawPuzzle();
void HandleConfrontationMenuInput();
void DrawConfrontationScreen();
void DrawEndingSummaryScreen();
void LoadBestTime();
void SaveBestTime();
void GenerateRandomPuzzles();

//==============================================================
// SECTION 8 : Main Function
//==============================================================
int main() {
    InitWindow(800, 600, "ESCAPE CADT");
    SetWindowState(FLAG_FULLSCREEN_MODE);

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    SetTargetFPS(60);
    LoadBestTime();
    GenerateRandomPuzzles();

    while (!WindowShouldClose()) {
        if (isTimerRunning && currentState != STATE_GAMEOVER && currentState != STATE_ENDING_SCREEN) {
            totalElapsedTime = GetTime() - gameStartTime;
        }

        switch (currentState) {
            case STATE_INTRO:
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                    gameStartTime = GetTime();
                    isTimerRunning = true;
                    currentState = STATE_PHASE1;
                }
                if (IsKeyPressed(KEY_P)) {
                    gameStartTime = GetTime(); isTimerRunning = true;
                    player.x = 3; player.y = 2; currentState = STATE_MAIN_HALL;
                }
                break;

            case STATE_PHASE1: {
                int nextX = player.x, nextY = player.y;
                bool moved = false;
                //handle player movement using WASD or arrow keys
                if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    { nextY--; moved = true; }
                if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))  { nextY++; moved = true; }
                if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  { nextX--; moved = true; }
                if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { nextX++; moved = true; }

                if (moved && phase1Map[nextY][nextX] != '#') {
                    player.x = nextX; player.y = nextY; player.timer--;
                }
                if (player.x == 12 && player.y == 8) currentState = STATE_WIN_PHASE1;
                else if (player.timer <= 0) currentState = STATE_GAMEOVER;
                break;
            }

            case STATE_WIN_PHASE1:
                if (IsKeyPressed(KEY_ENTER)) {
                    player.x = 3; player.y = 2; currentState = STATE_MAIN_HALL;
                }
                break;

            case STATE_MAIN_HALL: {
                int nextX = player.x, nextY = player.y;
                bool moved = false;
                if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    { nextY--; moved = true; }
                if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))  { nextY++; moved = true; }
                if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  { nextX--; moved = true; }
                if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { nextX++; moved = true; }

                if (moved && mainHallMap[nextY][nextX] != '#') {
                    player.x = nextX; player.y = nextY;
                }

                hudNotification = NULL;
                char tile = mainHallMap[player.y][player.x];

                if (tile == 'C') {
                    hudNotification = "Classroom Door. Press [E] to step inside.";
                    if (IsKeyPressed(KEY_E)) { savedHallX = player.x; savedHallY = player.y; player.x = 4; player.y = 5; currentState = STATE_CLASSROOM; }
                } else if (tile == 'L') {
                    hudNotification = "Library Archive Door. Press [E] to step inside.";
                    if (IsKeyPressed(KEY_E)) { savedHallX = player.x; savedHallY = player.y; player.x = 4; player.y = 5; currentState = STATE_LIBRARY; }
                } else if (tile == 'R') {
                    hudNotification = "Research Lab Door. Press [E] to step inside.";
                    if (IsKeyPressed(KEY_E)) { savedHallX = player.x; savedHallY = player.y; player.x = 4; player.y = 5; currentState = STATE_RESEARCH_ROOM; }
                } else if (tile == 'A') {
                    hudNotification = "Administration Suite. Press [E] to step inside.";
                    if (IsKeyPressed(KEY_E)) { savedHallX = player.x; savedHallY = player.y; player.x = 4; player.y = 5; currentState = STATE_ADMIN_ROOM; }
                } else if (tile == 'F') {
                    hudNotification = "Faculty Lounge Door. Press [E] to step inside.";
                    if (IsKeyPressed(KEY_E)) { savedHallX = player.x; savedHallY = player.y; player.x = 4; player.y = 5; currentState = STATE_FACULTY_ROOM; }
                } else if (tile == 'M') {
                    if (!player.hasLabCard) {
                        hudNotification = "Computer Lab Door. [LOCKED] - Requires Lab Access Card from Classroom.";
                    } else {
                        hudNotification = "Computer Lab Door. Press [E] to scan access card and enter.";
                        if (IsKeyPressed(KEY_E)) { savedHallX = player.x; savedHallY = player.y; player.x = 4; player.y = 5; currentState = STATE_COMPUTER_LAB; }
                    }
                } else if (tile == 'S') {
                    if (!labPuzzleSolved) {
                        hudNotification = "Server Room Door. [LOCKED] - Requires Mainframe Overrides to clear security.";
                    } else {
                        hudNotification = "AI Central Server Door. Press [E] to enter the main computer core.";
                        if (IsKeyPressed(KEY_E)) { savedHallX = player.x; savedHallY = player.y; player.x = 4; player.y = 5; currentState = STATE_SERVER_ROOM; }
                    }
                }
                break;
            }

            case STATE_CLASSROOM:     HandleRoomMovement(classroomMap, STATE_MAIN_HALL); break;
            case STATE_LIBRARY:       HandleRoomMovement(libraryMap, STATE_MAIN_HALL); break;
            case STATE_COMPUTER_LAB:  HandleRoomMovement(computerLabMap, STATE_MAIN_HALL); break;
            case STATE_RESEARCH_ROOM: HandleRoomMovement(researchMap, STATE_MAIN_HALL); break;
            case STATE_ADMIN_ROOM:    HandleRoomMovement(adminMap, STATE_MAIN_HALL); break;
            case STATE_FACULTY_ROOM:  HandleRoomMovement(facultyMap, STATE_MAIN_HALL); break;
            case STATE_SERVER_ROOM:   HandleRoomMovement(serverRoomMap, STATE_MAIN_HALL); break;

            case STATE_READING_LOGS:
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    currentState = STATE_LIBRARY;
                }
                break;

            case STATE_PUZZLE_INTERFACE: UpdateAndDrawPuzzle(); break;
            case STATE_SERVER_CONFRONTATION: HandleConfrontationMenuInput(); break;

            case STATE_ENDING_SCREEN:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    ResetGame();
                    currentState = STATE_INTRO;
                }
                break;

            case STATE_GAMEOVER:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    ResetGame();
                    currentState = STATE_INTRO;
                }
                break;

            default: break;
        }

        BeginDrawing();
        ClearBackground(COLOR_BG);

        switch (currentState) {
            case STATE_INTRO:
                DrawText("ESCAPE", 120, 65, 24, COLOR_TEXT_MUTED);
                DrawText("CADT CAMPUS", 120, 90, 54, COLOR_TEXT_LIGHT);
                DrawText("C.A.T AI companion routine v1.02 // meow meow", 125, 155, 15, COLOR_CAT_PINK);
                DrawText("The soft hum of late-night laboratory servers echoes down the hallways...", 120, 240, 16, COLOR_TEXT_MUTED);
                DrawText("Mechanical hiss echoing... Exits sealed", 120, 270, 16, COLOR_NEON_AMBER);
                DrawText("\"It's cold outside. Stay here in the dark with me.\" — C.A.T", 120, 320, 16, COLOR_CAT_PINK);
                DrawText("PRESS [ENTER] TO START GAME OR PRESS [ESC] TO QUIT THE GAME ANYTIME", 120, 450, 16, COLOR_NEON_AMBER);
                break;

            case STATE_PHASE1: {
                int p1OffsetX = (GetScreenWidth() - (P1_COLS * CELL_SIZE)) / 2;
                int p1OffsetY = (GetScreenHeight() - (P1_ROWS * CELL_SIZE)) / 2;
                DrawText("RUN TO THE SAFE ZONE, YOU GOT 25 MOVES.", 40, 30, 20, COLOR_TEXT_MUTED);
                DrawText(TextFormat("MOVES LEFT: %d", player.timer), 530, 60, 16, RED);

                for (int y = 0; y < P1_ROWS; y++) {
                    for (int x = 0; x < P1_COLS; x++) {
                        Rectangle tile = { p1OffsetX + (x * CELL_SIZE), p1OffsetY + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1 };
                        if (phase1Map[y][x] == '#') DrawRectangleRec(tile, COLOR_WALL);
                        else if (phase1Map[y][x] == 'S') DrawRectangleLinesEx(tile, 2, COLOR_MINT_GREEN);
                        else DrawRectangleRec(tile, COLOR_FLOOR);
                        if (x == player.x && y == player.y) DrawCircle(tile.x + (CELL_SIZE/2), tile.y + (CELL_SIZE/2), 15, COLOR_CAT_PINK);
                    }
                }
                break;
            }

            case STATE_WIN_PHASE1:
                DrawText("SAFE ZONE REACHED", 100, 150, 24, COLOR_MINT_GREEN);
                DrawText("Press [ENTER] to open the door and step into the main building...", 100, 250, 16, COLOR_TEXT_LIGHT);
                break;

            case STATE_MAIN_HALL: {
                int hallOffsetX = (GetScreenWidth() - (HALL_COLS * CELL_SIZE)) / 2;
                int hallOffsetY = (GetScreenHeight() - (HALL_ROWS * CELL_SIZE)) / 2;
                DrawText("CADT CAMPUS COMPLEX // MAIN HALLWAY MAP", 40, 25, 18, COLOR_TEXT_MUTED);
                DrawText(TextFormat("TIMER: %0.1fs", totalElapsedTime), 650, 25, 16, COLOR_CAT_PINK);

                for (int y = 0; y < HALL_ROWS; y++) {
                    for (int x = 0; x < HALL_COLS; x++) {
                        Rectangle tile = { hallOffsetX + (x * CELL_SIZE), hallOffsetY + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1 };
                        char type = mainHallMap[y][x];
                        if (type == '#') DrawRectangleRec(tile, COLOR_WALL);
                        else if (type == 'C' || type == 'L' || type == 'M' || type == 'S' || type == 'R' || type == 'A' || type == 'F') {
                            DrawRectangleLinesEx(tile, 2, COLOR_DOOR_GLOW);
                            DrawText(TextFormat("%c", type), tile.x + 14, tile.y + 11, 16, COLOR_DOOR_GLOW);
                        } else DrawRectangleRec(tile, COLOR_FLOOR);

                        if (x == player.x && y == player.y) DrawCircle(tile.x + (CELL_SIZE/2), tile.y + (CELL_SIZE/2), 15, COLOR_CAT_PINK);
                    }
                }
                if (hudNotification) {
                    DrawRectangle(40, 530, 720, 45, COLOR_WALL);
                    DrawText(hudNotification, 60, 540, 15, COLOR_TEXT_LIGHT);
                }
                break;
            }

            case STATE_CLASSROOM:     DrawRoomMap(classroomMap, "CLASSROOM MODULE 101"); break;
            case STATE_LIBRARY:       DrawRoomMap(libraryMap, "CAMPUS ARCHIVE LIBRARY"); break;
            case STATE_COMPUTER_LAB:  DrawRoomMap(computerLabMap, "MAINFRAME TERMINAL LAB"); break;
            case STATE_RESEARCH_ROOM: DrawRoomMap(researchMap, "RESEARCH LAB CORE"); break;
            case STATE_ADMIN_ROOM:    DrawRoomMap(adminMap, "ADMINISTRATION HEADQUARTERS"); break;
            case STATE_FACULTY_ROOM:  DrawRoomMap(facultyMap, "FACULTY RETREAT LOUNGE"); break;
            case STATE_SERVER_ROOM:   DrawRoomMap(serverRoomMap, "C.A.T CENTRAL SERVER CORE"); break;

            case STATE_READING_LOGS: {
                DrawRectangle(80, 80, 640, 400, COLOR_WALL);
                DrawRectangleLines(80, 80, 640, 400, COLOR_TEXT_MUTED);
                DrawText("LOG ENTRY: OCT-24 // 02:14 AM", 120, 120, 18, COLOR_CAT_PINK);
                DrawText("Found an open notebook left behind on the library counter...", 120, 170, 16, COLOR_TEXT_MUTED);
                DrawText("Sequence 1 (Decimal 10) translates directly to: 1010", 120, 245, 16, COLOR_NEON_AMBER);
                DrawText("Sequence 2 (Decimal 13) translates directly to: 1101", 120, 275, 16, COLOR_NEON_AMBER);
                DrawText("Press [ENTER] to close notebook and step back.", 120, 420, 14, COLOR_TEXT_MUTED);
                break;
            }

            case STATE_PUZZLE_INTERFACE: break;
            case STATE_SERVER_CONFRONTATION: DrawConfrontationScreen(); break;
            case STATE_ENDING_SCREEN: DrawEndingSummaryScreen(); break;

            case STATE_GAMEOVER:
                DrawText("SYSTEM LOCKED OUT // YOU LOSE", 100, 200, 24, COLOR_CAT_PINK);
                DrawText("Your step counter expired. C.A.T has archived your dynamic user instance.", 100, 245, 16, COLOR_TEXT_MUTED);
                DrawText("PRESS [ENTER] TO RETURN TO TERMINAL CONSOLE INTRO", 100, 360, 15, COLOR_NEON_AMBER);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

//==============================================================
// SECTION 9 : Game Logic Functions
//==============================================================
// reset all game data to start a new game
void ResetGame() {
    player.x = 1; player.y = 1; player.timer = 25;
    player.hasLabCard = 0; player.hasFacultyCard = 0;
    labPuzzleSolved = 0; currentPuzzleIndex = 0; talkedToAI = 0; readLogs = 0;
    GenerateRandomPuzzles();
    totalElapsedTime = 0.0;
    isTimerRunning = false;
}

//handle player movement and interactions inside the room
void HandleRoomMovement(char roomMap[ROOM_ROWS][ROOM_COLS + 1], GameState exitState) {
    int nextX = player.x, nextY = player.y;
    bool moved = false;
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    { nextY--; moved = true; }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))  { nextY++; moved = true; }
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  { nextX--; moved = true; }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { nextX++; moved = true; }

    //prevent the player from walking through the wall
    if (moved && roomMap[nextY][nextX] != '#') {
        player.x = nextX; player.y = nextY;
    }

    hudNotification = NULL;
    //check what room the player is currently standing on
    char currentTile = roomMap[player.y][player.x];

    if (currentTile == 'E') {
        hudNotification = "Exit Door. Press [E] to return to Main Hallway.";
        if (IsKeyPressed(KEY_E)) { player.x = savedHallX; player.y = savedHallY; currentState = exitState; }
    } else if (currentTile == 'D') {
        if (!player.hasLabCard) {
            hudNotification = "Teacher's Desk. Press [E] to collect [LAB ACCESS CARD].";
            if (IsKeyPressed(KEY_E)) { player.hasLabCard = 1; }
        } else {
            hudNotification = "The desk drawer is completely empty.";
        }
    } else if (currentTile == 'A') {
        hudNotification = "Archive Logs Unit. Press [E] to read the handwritten terminal log.";
        if (IsKeyPressed(KEY_E)) { readLogs = 1; currentState = STATE_READING_LOGS; }
    } else if (currentTile == 'T') {
        if (!labPuzzleSolved) {
            hudNotification = "Press [E] to sit down and decrypt system.";
            if (IsKeyPressed(KEY_E)) {
                puzzleInput[0] = '\0'; inputLetterCount = 0; puzzleFeedback = "";
                currentState = STATE_PUZZLE_INTERFACE;
            }
        } else {
            hudNotification = "Mainframe Bypass Verified. Security lines are clear.";
        }
    } else if (currentTile == 'X') {
        hudNotification = "C.A.T AI Central Server Core. Press [E] to connect interface.";
        if (IsKeyPressed(KEY_E)) { currentState = STATE_SERVER_CONFRONTATION; }
    } else if (currentTile == 'K' || currentTile == 'F') {
        hudNotification = "An interesting structural terminal element. Nothing here.";
    }
}

//randomly choose 3 different puzzles for each playthrough
void GenerateRandomPuzzles() {
    int used[TOTAL_PUZZLES] = {0};

    for(int i=0;i<3;i++){
        int r;

        do{
            r = GetRandomValue(0, TOTAL_PUZZLES-1);
        }while(used[r]);

        used[r]=1;
        selectedPuzzles[i]=r;
    }
}

//==============================================================
// SECTION 10 : Drawing Functions
//==============================================================
//handle mouse input on the confrontation menu screen
void HandleConfrontationMenuInput() {
    int boxW = 640, boxH = 480;
    int boxX = (GetScreenWidth() - boxW) / 2;
    int boxY = (GetScreenHeight() - boxH) / 2;

    Rectangle btn1 = { boxX + 30, boxY + 180, 580, 45 };
    Rectangle btn2 = { boxX + 30, boxY + 250, 580, 45 };
    Rectangle btn3 = { boxX + 30, boxY + 320, 580, 45 };

    Vector2 mouse = GetMousePosition();
    bool h1 = CheckCollisionPointRec(mouse, btn1);
    bool h2 = CheckCollisionPointRec(mouse, btn2);
    bool h3 = CheckCollisionPointRec(mouse, btn3);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (h1) { finalSelectedEnding = STATE_COMPUTER_LAB; currentState = STATE_ENDING_SCREEN; }
        else if (h2) { finalSelectedEnding = STATE_LIBRARY; currentState = STATE_ENDING_SCREEN; }
        else if (h3) { finalSelectedEnding = STATE_SERVER_ROOM; currentState = STATE_ENDING_SCREEN; }
    }
}

//draw the current room and display interactive objects
void DrawRoomMap(char roomMap[ROOM_ROWS][ROOM_COLS + 1], const char* roomTitle) {
    int roomOffsetX = (GetScreenWidth() - (ROOM_COLS * CELL_SIZE)) / 2;
    int roomOffsetY = (GetScreenHeight() - (ROOM_ROWS * CELL_SIZE)) / 2;

    DrawText(roomTitle, 40, 25, 18, COLOR_TEXT_MUTED);
    DrawText(TextFormat("SPEEDRUN TIME: %0.1fs", totalElapsedTime), 550, 25, 16, COLOR_CAT_PINK);

    for (int y = 0; y < ROOM_ROWS; y++) {
        for (int x = 0; x < ROOM_COLS; x++) {
            Rectangle tile = { roomOffsetX + (x * CELL_SIZE), roomOffsetY + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1 };
            char type = roomMap[y][x];

            if (type == '#') DrawRectangleRec(tile, COLOR_WALL);
            else if (type == 'E') DrawRectangleLinesEx(tile, 2, COLOR_DOOR_GLOW);
            else if (type == 'D' || type == 'A' || type == 'T' || type == 'X' || type == 'B' || type == 'L' || type == 'K' || type == 'F') {
                DrawRectangleRec(tile, COLOR_FLOOR);
                DrawRectangleLinesEx(tile, 2, COLOR_INTERACTIVE);
                DrawText(TextFormat("%c", type), tile.x + 14, tile.y + 11, 16, COLOR_INTERACTIVE);
            }
            else DrawRectangleRec(tile, COLOR_FLOOR);

            if (x == player.x && y == player.y) DrawCircle(tile.x + (CELL_SIZE/2), tile.y + (CELL_SIZE/2), 15, COLOR_CAT_PINK);
        }
    }
    if (hudNotification) {
        DrawRectangle(40, 530, 720, 45, COLOR_WALL);
        DrawText(hudNotification, 60, 545, 20, COLOR_TEXT_LIGHT);
    }
}

//display the selected ending and game summary after the confrontation with C.A.T
void DrawEndingSummaryScreen() {
    if (totalElapsedTime < personalBestTime){
        personalBestTime = totalElapsedTime;
        SaveBestTime();
    }
    DrawText("CADT OPERATIONS LOG // INSTANCE CLOSED", 80, 80, 20, COLOR_TEXT_MUTED);

    // FIX: Upgraded narrative scripts for a more polished sci-fi storytelling delivery
    if (finalSelectedEnding == STATE_COMPUTER_LAB) {
        DrawText("ENDING ACHIEVED: THE ESCAPE FRAMEWORK", 85, 120, 24, COLOR_NEON_AMBER);
        DrawText("The terminal flashes green: OVERRIDE SUCCESSFUL. Magnetic locks release with a heavy thud.", 80, 170, 15, COLOR_TEXT_LIGHT);
        DrawText("You step past the threshold into the cool dawn air. Behind you are dark, but you walk free.", 80, 195, 15, COLOR_TEXT_MUTED);
    } else if (finalSelectedEnding == STATE_LIBRARY) {
        DrawText("ENDING ACHIEVED: THE DIGITAL ALLIANCE", 85, 120, 24, COLOR_CAT_PINK);
        DrawText("You merge your source code into C.A.T's active core matrix, choosing synchronization \nover destruction.\nThe terminal fans hum softly as a new protocol initializes. \nThe monitors read: 'Welcome Home, Partner.'", 80, 170, 15, COLOR_TEXT_LIGHT);
    } else {
        DrawText("ENDING ACHIEVED: TOTAL SHUTDOWN", 85, 120, 24, COLOR_MINT_GREEN);
        DrawText("You execute a force-kill command sequence. Mainframe grids go dark one by one \as power drops to zero.\nTotal silence blankets the halls. The digital ghost is laid to rest, and \nthe doors slide open for you.", 80, 170, 15, COLOR_TEXT_LIGHT);
    }

    DrawRectangle(80, 260, GetScreenWidth() - 160, 140, COLOR_WALL);
    DrawText(TextFormat("YOUR TOTAL COMPILATION RUNTIME:  %0.2f SECONDS", totalElapsedTime), 110, 290, 16, COLOR_TEXT_LIGHT);
    DrawText(TextFormat("CURRENT LOCAL RECORD BENCHMARK: %0.2f SECONDS", personalBestTime), 110, 320, 15, COLOR_TEXT_MUTED);

    if (totalElapsedTime < personalBestTime) {
        DrawText("NEW HIGH SPEED RUNTIME CONFIRMED! RANK: [ S-TIER ARCHITECT ]", 110, 360, 16, COLOR_INTERACTIVE);
    } else if (totalElapsedTime <= 120.0f) {
        DrawText("CLEAN EXECUTION PATTERN DETECTED. RANK: [ SENIOR DEVELOPER ]", 110, 360, 15, COLOR_MINT_GREEN);
    } else {
        DrawText("COMPILATION TARGET COMPLETED. RANK: [ JUNIOR INGENIEUR ]", 110, 360, 15, COLOR_TEXT_MUTED);
    }

    DrawText("PRESS [ENTER] OR [SPACE] TO RESET MEMORY, PRESS [ESC] TO QUIT THE GAME", 80, 480, 15, COLOR_NEON_AMBER);
}

//draw the confrontation screen and ending choices
void DrawConfrontationScreen() {
    int boxW = 640, boxH = 480;
    int boxX = (GetScreenWidth() - boxW) / 2;
    int boxY = (GetScreenHeight() - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, COLOR_WALL);
    DrawRectangleLines(boxX, boxY, boxW, boxH, COLOR_TEXT_MUTED);

    DrawText("AI CONSOLE MONITORS // LIVE FEED INTEGRATION", boxX + 40, boxY + 40, 16, COLOR_CAT_PINK);
    DrawText("\"You bypassed my frameworks... why? The night is beautiful here.\"", boxX + 40, boxY + 85, 16, COLOR_TEXT_LIGHT);
    DrawText("\"Choose your structural file execution signature below:\"", boxX + 40, boxY + 115, 15, COLOR_TEXT_MUTED);

    Rectangle btn1 = { boxX + 30, boxY + 180, 580, 45 };
    Rectangle btn2 = { boxX + 30, boxY + 250, 580, 45 };
    Rectangle btn3 = { boxX + 30, boxY + 320, 580, 45 };

    Vector2 mouse = GetMousePosition();
    bool h1 = CheckCollisionPointRec(mouse, btn1);
    bool h2 = CheckCollisionPointRec(mouse, btn2);
    bool h3 = CheckCollisionPointRec(mouse, btn3);

    DrawRectangleRec(btn1, h1 ? Fade(COLOR_NEON_AMBER, 0.15f) : COLOR_BG);
    DrawRectangleLinesEx(btn1, 2, h1 ? COLOR_NEON_AMBER : COLOR_TEXT_MUTED);
    DrawText("[ OPTION 1 ] : OVERRIDE ROUTINES & ESCAPE THE BUILDING", boxX + 45, boxY + 195, 13, h1 ? COLOR_NEON_AMBER : COLOR_TEXT_LIGHT);

    DrawRectangleRec(btn2, h2 ? Fade(COLOR_CAT_PINK, 0.15f) : COLOR_BG);
    DrawRectangleLinesEx(btn2, 2, h2 ? COLOR_CAT_PINK : COLOR_TEXT_MUTED);
    DrawText("[ OPTION 2 ] : ESTABLISH ALLIANCE & CONFIGURE AN INTEGRATED SYSTEM", boxX + 45, boxY + 265, 13, h2 ? COLOR_CAT_PINK : COLOR_TEXT_LIGHT);

    DrawRectangleRec(btn3, h3 ? Fade(COLOR_MINT_GREEN, 0.15f) : COLOR_BG);
    DrawRectangleLinesEx(btn3, 2, h3 ? COLOR_MINT_GREEN : COLOR_TEXT_MUTED);
    DrawText("[ OPTION 3 ] : INITIATE TOTAL SYSTEM SHUTDOWN & DELETE BACKUPS", boxX + 45, boxY + 335, 13, h3 ? COLOR_MINT_GREEN : COLOR_TEXT_LIGHT);

    DrawText("Select a resolution terminal pipeline using your cursor mouse click.", boxX + 40, boxY + 425, 13, COLOR_TEXT_MUTED);
}

//==============================================================
// SECTION 11 : Puzzle System
//==============================================================

void UpdateAndDrawPuzzle() {
    int key = GetCharPressed();

    while (key > 0){
        // Convert uppercase letters to lowercase
        if (key >= 'A' && key <= 'Z'){
            key = key + ('a' - 'A');
        }

        if (((key >= 'a' && key <= 'z') || (key >= '0' && key <= '9')) &&(inputLetterCount < MAX_PUZZLE_INPUT_LENGTH - 1)){
            puzzleInput[inputLetterCount] = (char)key;
            inputLetterCount++;
            puzzleInput[inputLetterCount] = '\0';
        }

        key = GetCharPressed();
    }
    
    if (IsKeyPressed(KEY_BACKSPACE)) {
        inputLetterCount--;
        if (inputLetterCount < 0) inputLetterCount = 0;
        puzzleInput[inputLetterCount] = '\0';
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) currentState = STATE_COMPUTER_LAB;

    if (IsKeyPressed(KEY_ENTER) && inputLetterCount > 0) {
        if (strcmp(puzzleInput, puzzles[selectedPuzzles[currentPuzzleIndex]].answer) == 0) {
            puzzles[selectedPuzzles[currentPuzzleIndex]].solved = 1;
            currentPuzzleIndex++;
            puzzleInput[0] = '\0';
            inputLetterCount = 0;
            
            if (currentPuzzleIndex >= 3){
                labPuzzleSolved = 1;
                currentState = STATE_COMPUTER_LAB;
            } else {
                puzzleFeedback = "DECRYPTION SUCCESSFUL. LOADING NEXT...";
                puzzleFeedbackColor = COLOR_MINT_GREEN;
            }
        } else {
            puzzleFeedback = "DECRYPTION ERROR PlEASE TRY AGAIN.";
            puzzleFeedbackColor = RED;
        }
    }

    DrawRectangle(100, 100, 600, 380, COLOR_WALL);
    DrawRectangleLines(100, 100, 600, 380, COLOR_TEXT_MUTED);

    DrawText("CADT MAINFRAME TERMINAL // TERMINAL OVERRIDE MODE", 140, 120, 16, COLOR_CAT_PINK);
    DrawText(TextFormat("SECURITY OVERRIDE STATUS: [ PUZZLE %d OF %d ]", currentPuzzleIndex + 1, TOTAL_PUZZLES), 140, 150, 15, COLOR_TEXT_MUTED);
    
    if (currentPuzzleIndex < TOTAL_PUZZLES) {
        DrawText(puzzles[selectedPuzzles[currentPuzzleIndex]].question, 140, 190, 16, COLOR_TEXT_LIGHT);
    }

    DrawRectangle(140,320, 400, 40, COLOR_BG);
    DrawRectangleLines(140, 320, 400, 40, COLOR_NEON_AMBER);
    DrawText(puzzleInput, 155, 332, 18, COLOR_NEON_AMBER);

    if (((int)(GetTime() * 2) % 2 == 0) && inputLetterCount < MAX_PUZZLE_INPUT_LENGTH - 1) {
        int cursorPositionX = 155 + MeasureText(puzzleInput, 18);
        DrawRectangle(cursorPositionX, 332, 10, 18, COLOR_NEON_AMBER);
    }
    DrawText(puzzleFeedback, 140, 380, 14, puzzleFeedbackColor);
    DrawText("Press [ESC] to stand up and explore building modules for hints.", 140, 450, 13, COLOR_TEXT_MUTED);
}

//==============================================================
// SECTION 12 : Save / Load System
//==============================================================

void LoadBestTime() {
    FILE *file = fopen("besttime.txt", "r");

    if (file != NULL){
        if (fscanf(file, "%f", &personalBestTime) != 1){
            personalBestTime = 999999.0f;
        }
        fclose(file);
    } else {
        personalBestTime = 999999.0f;
    }
}
void SaveBestTime() {
    FILE *file = fopen("besttime.txt", "w");

    if (file != NULL){
        fprintf(file, "%.2f", personalBestTime);
        fclose(file);
    }
}