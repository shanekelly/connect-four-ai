#include "../include/renderer.h"


// TODO(davidabrahams): calculate this dynamically based on board width
int tileOffsets[7] = {2, 6, 10, 14, 18, 22, 26};

void decrementWithMin(int* num, int min) {
    (*num)--;
    if (*num < min) {
        *num = min;
    }
}

void incrementWithMax(int* num, int max) {
    (*num)++;
    if (*num > max) {
        *num = max;
    }
}

void renderMenu(const char* title,
                const char** items,
                int numItems,
                int selection) {
  // Render the menu title
  mvprintw(Y_OFFSET, X_OFFSET, title);

  for (int i = 0; i < numItems; i++) {
    // Highlight the currently selected menu item
    if (i == selection) {
      attron(A_STANDOUT);
    }

    // Render each menu item
    mvprintw(Y_OFFSET+1+i, X_OFFSET+2, items[i]);
    attroff(A_STANDOUT);
  }
}

int waitUntilOptionSelected(const char* title,
                            const char** items,
                            int numItems) {
  char userInput;
  int selection = 0;

  while (userInput != SELECT_KEY) {
    // Non-blocking user-input
    timeout(10);
    userInput = getch();

    // Navigate through the menu based on user-input
    switch (userInput) {
      case UP_KEY:
        decrementWithMin(&selection, 0);
        break;
      case DOWN_KEY:
        incrementWithMax(&selection, numItems-1);
        break;
    }

    // Render the menu
    renderMenu(title, items, numItems, selection);
  }

  return selection;
}

void displayGrid(State s) {
  mvaddstr(Y_OFFSET, X_OFFSET, "=============================");
  for (int y = Y_OFFSET + 1; y <= Y_OFFSET + State::HEIGHT + 1; y++) {
    mvaddstr(y, X_OFFSET, "|   |   |   |   |   |   |   |");
  }
  for (int row = 0; row < State::HEIGHT; row++) {
    for (int col = 0; col < State::WIDTH; col++) {
      State::TileType tileType = s.getTile(row, col);
      if (tileType == State::Red) {
        attron(COLOR_PAIR(1) | A_BOLD);
        mvaddch(Y_OFFSET + row + 1, X_OFFSET + tileOffsets[col], '@');
        attroff(COLOR_PAIR(1) | A_BOLD);
      }
      if (tileType == State::Black) {
        attron(COLOR_PAIR(2) | A_BOLD);
        mvaddch(Y_OFFSET + row + 1, X_OFFSET + tileOffsets[col], '@');
        attroff(COLOR_PAIR(2) | A_BOLD);
      }
    }
  }
  mvaddstr(Y_OFFSET + State::HEIGHT + 1, X_OFFSET,
      "=============================");
  refresh();
}

State setupScreen(State s) {
  const char* title = "Connect 4 AI";
  const char* items[3] = {"Play against computer",
                          "Play against a friend",
                          "Watch computer vs. computer"};
  const int numItems = 3;
  int selection;

  initscr();                                // Start curses mode
  start_color();
  noecho();
  curs_set(0);                              // Makes cursor invisible
  init_pair(1, COLOR_RED, COLOR_WHITE);     // Creates red tiles
  init_pair(2, COLOR_BLACK, COLOR_WHITE);   // Creates yellow tiles
  init_pair(3, 244, COLOR_WHITE);  // TODO(davidabrahams): which color is this?
  bkgd(COLOR_PAIR(3));
  selection = waitUntilOptionSelected(title, items, numItems);

  switch (selection) {
    case 0:  // Play against the computer (PvC)
      s.gameType = State::PvC;
      break;
    case 1:  // Play against a friend (PvP)
      s.gameType = State::PvP;
      break;
    case 2:  // Watch a computer play itself (CvC)
      s.gameType = State::CvC;
      break;
  }

  mvaddstr(Y_OFFSET, X_OFFSET, "=============================");
  for (int y = Y_OFFSET + 1; y <= Y_OFFSET + State::HEIGHT + 1; y++) {
    mvaddstr(y, X_OFFSET, "|   |   |   |   |   |   |   |");
  }
  mvaddstr(Y_OFFSET + State::HEIGHT + 1, X_OFFSET,
      "=============================");
  mvprintw(Y_OFFSET + State::HEIGHT + 2, X_OFFSET, "Game Mode: %i", selection);
  refresh();
  return s;
}

void shutDownScreen() {
  char userInput;
  while (userInput != QUIT_KEY) {
    userInput = getch();           // Wait for user input
  }
  endwin();                         // End curses mode
}
