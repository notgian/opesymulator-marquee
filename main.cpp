#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <conio.h>
#include <windows.h>
#include "Screen.h"
#include "MarqueeText.h"
#include "Component.h"
#include "ConsoleScreen.h"

// to make our lives easier
using std::cout;
using std::endl;
using std::cin;
using std::string;

void print_welcome(ConsoleScreen &console) {
    console.print("   ____  ____  _____________  ____  _____  ____    ___  __________  ____ ");
    console.print("  / __ \\/ __ \\/ ____/ ___/\\ \\/ /  |/  / / / / /   /   |/_  __/ __ \\/ __ \\");
    console.print(" / / / / /_/ / __/  \\__ \\  \\  / /|_/ / / / / /   / /| | / / / / / / /_/ /");
    console.print("/ /_/ / ____/ /___ ___/ /  / / /  / / /_/ / /___/ ___ |/ / / /_/ / _, _/ ");
    console.print("\\____/_/   /_____//____/  /_/_/  /_/\\____/_____/_/  |_/_/  \\____/_/ |_| ");
    console.print("");
    console.print("Welcome");
    console.print("CSOPESY Emulator");
    console.print("Agsalon - Ercia - Garcia - Ortha");
    console.print("");
}

// Splits "set_speed 200" into {"set_speed", "200"}. args_ is left empty
// when there's no second token.
static void splitCommand(const string& commandLine, string& name, string& args) {
    size_t spacePos = commandLine.find(' ');
    if (spacePos == string::npos) {
        name = commandLine;
        args = "";
    } else {
        name = commandLine.substr(0, spacePos);
        args = commandLine.substr(spacePos + 1);
    }
}

/**
 * Command interpreter: takes one line of raw input, acts on the
 * MarqueeText/ConsoleScreen components as needed, and returns the
 * lines that should be echoed back plus whether "exit" was requested.
 *
 * This is intentionally the only place that knows both what a command
 * means AND which components exist -- ConsoleScreen just displays
 * whatever CommandResult::output it's handed.
 */
CommandResult cmd_dispatch(const string& commandLine, MarqueeText& marquee, ConsoleScreen& console) {
    string name, args;
    splitCommand(commandLine, name, args);

    static const std::unordered_map<std::string, std::function<CommandResult(const string&)>> handlers = {
        {"help", [](const string&) {
            return CommandResult{{
                "Available commands:",
                "  help                 - displays this list",
                "  start_marquee        - starts the marquee animation",
                "  stop_marquee         - stops the marquee animation",
                "  set_text <text>      - sets the marquee text",
                "  set_speed <ms>       - sets the marquee refresh rate in milliseconds",
                "  clear                - clears the console history",
                "  exit                 - terminates the console",
            }};
        }},
        {"start_marquee", [&marquee](const string&) {
            marquee.start();
            return CommandResult{{"Marquee started."}};
        }},
        {"stop_marquee", [&marquee](const string&) {
            marquee.stop();
            return CommandResult{{"Marquee stopped."}};
        }},
        {"set_text", [&marquee](const string& text) {
            if (text.empty()) return CommandResult{{"Usage: set_text <text>"}};
            marquee.setMarqueeText(text);
            return CommandResult{{"Marquee text set to: " + text}};
        }},
        {"set_speed", [&marquee](const string& text) {
            try {
                int speedMs = std::stoi(text);
                marquee.setSpeed(speedMs);
                return CommandResult{{"Marquee speed set to " + std::to_string(speedMs) + "ms."}};
            } catch (...) {
                return CommandResult{{"Usage: set_speed <milliseconds>"}};
            }
        }},
        {"clear", [&console](const string&) {
            console.clearHistory();
            return CommandResult{}; // history is already wiped; nothing left to echo
        }},
        {"exit", [](const string&) {
            CommandResult result;
            result.output = {"bye bye"};
            result.exitRequested = true;
            return result;
        }},
    };

    auto it = handlers.find(name);
    if (it != handlers.end()) {
        return it->second(args);
    }

    return CommandResult{{"Command not found: " + commandLine}};
}

// Helper to full clear built-in terminal screen (not our emulator screen)
void clearScreen() {
    std::cout << "\033[2J\033[H";
}


int main (int argc, char *argv[]) {
    int screenWidth = 100;
    int screenHeight = 25;

    bool running = true;

    Screen MainScreen = Screen(screenWidth, screenHeight);

    MarqueeText MarqueeTextComponent = MarqueeText(0, 0, screenWidth, screenHeight, "Hello World");

    ConsoleScreen Console(0, 0, screenWidth, screenHeight, "Command> ");
    // Console.print("Welcome to CSOPESY!");
    print_welcome(Console);
    Console.print("Type 'help' to see the available commands.");

    // The console only knows how to display text; cmd_dispatch is what
    // actually understands "start_marquee" etc., so it's the one wired
    // in here as the command handler.
    Console.setCommandHandler([&MarqueeTextComponent, &Console](const string& commandText) {
        return cmd_dispatch(commandText, MarqueeTextComponent, Console);
    });

    clearScreen();
    while (running) {
        if (_kbhit()) {
            int intercepted = _getch();

            switch (intercepted) {
                case '\r': // Enter
                    Console.handleEnter();
                    if (Console.consumeExitRequested()) {
                        running = false;
                    }
                    break;
                case '\b': // Backspace
                case 127:  // Some terminals send DEL instead
                    Console.handleBackspace();
                    break;
                default:
                    // Only accept printable characters; ignore arrow keys,
                    // function keys, and other non-text input for now.
                    if (intercepted >= 32 && intercepted < 127) {
                        Console.handleKeyPress(static_cast<char>(intercepted));
                    }
                    break;
            }
        }
        
        // Update components and screen
        MainScreen.update(Console);
        if (MarqueeTextComponent.isRunning()) {
            MarqueeTextComponent.update();
            MainScreen.update(MarqueeTextComponent);
        }

        // Redraw screen
        MainScreen.draw();
        MainScreen.clearBuffer();

        Sleep(1); // yield briefly so the loop doesn't spin the CPU at 100%
    }

    return 0;
}
