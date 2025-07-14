//
// Created by Flynn on 14.07.2025.
//

#include <iostream>
#include <vector>
#include <array>
#include <sstream>

#include "curse.h"

using namespace curse;

/*void test_basic() {
    CurseTerminal<ANSIColor> printer(std::cout);

    int term_w = get_terminal_width();
    int term_h = get_terminal_height();
    printer.init_matrix(term_h, term_w);

    // Root widget (Floating)
    Widget root({0,0}, {}, ANSIColor(ANSIColor::FG::BrightWhite, ANSIColor::BG::Blue), Quad(0,0,0,0), Quad(0,0,0,0), BoxStyle::Double, ShadowStyle::None);

    // Horizontal child
    std::vector<Widget> horiz_children;
    for (int i = 0; i < 3; ++i) {
        horiz_children.emplace_back("H-" + std::to_string(i), ANSIColor(ANSIColor::FG::BrightGreen, ANSIColor::BG::None));
    }
    Widget horiz(WidgetLayout::Horizontal, horiz_children, ANSIColor(ANSIColor::FG::BrightYellow, ANSIColor::BG::Red), Quad(0,0,0,0), Quad(0,0,0,0), BoxStyle::Single, ShadowStyle::Shadow, {2,2});

    // Vertical child
    std::vector<Widget> vert_children;
    for (int i = 0; i < 3; ++i) {
        vert_children.emplace_back("V-" + std::to_string(i), ANSIColor(ANSIColor::FG::BrightRed, ANSIColor::BG::None), Quad(1,1,1,1));
    }
    Widget vert(WidgetLayout::Vertical, vert_children, ANSIColor(ANSIColor::FG::BrightCyan, ANSIColor::BG::Magenta), Quad(0,0,0,0), Quad(0,0,0,0), BoxStyle::None, ShadowStyle::Fill, {20,2});

    // Floating child
    std::vector<Widget> floating_children;
    for (int i = 0; i < 2; ++i) {
        Widget t("F-" + std::to_string(i), ANSIColor(ANSIColor::FG::BrightBlue, ANSIColor::BG::None));
        t._xy = {i * 8, i * 2};
        floating_children.push_back(t);
    }
    Widget floating({2,10}, floating_children, ANSIColor(ANSIColor::FG::BrightMagenta, ANSIColor::BG::Green), Quad(0,0,0,0), Quad(0,0,0,0), BoxStyle::Single, ShadowStyle::Fill);

    // Text child
    Widget text("Just a text widget!", ANSIColor(ANSIColor::FG::BrightWhite, ANSIColor::BG::Red), Quad(0,0,0,0), BoxStyle::Single, ShadowStyle::None);
    text._xy = {40, 10};

    // Add all to root
    root._children.push_back(horiz);
    root._children.push_back(vert);
    root._children.push_back(floating);
    root._children.push_back(text);

    // Layout and render
    root.layout();
    root.render(printer._output_matrix, printer._color_matrix, 0, 0);
    printer.render_matrix();

    std::cin.ignore();
}*/

template<class TChar>
void test_popup_windows()
{
    CurseTerminal<ANSIColor, TChar> terminal(std::cout);
    terminal.init_renderer();
    int term_w = terminal.get_terminal_width();
    int term_h = terminal.get_terminal_height();
    terminal.init_matrix(term_h, term_w);

    // Define a palette
    AppStyle<ANSIColor> style(
        ANSIColor(ANSIColor::FG::Default, ANSIColor::BG::Default), // Primary
        ANSIColor(ANSIColor::FG::White, ANSIColor::BG::Red), // Secondary
        ANSIColor(ANSIColor::FG::BrightBlack, ANSIColor::BG::BrightBlue), // Accent
        ANSIColor(ANSIColor::FG::BrightWhite, ANSIColor::BG::BrightRed), // Selected
        ANSIColor(ANSIColor::FG::White, ANSIColor::BG::Blue), // Inactive
        ANSIColor(ANSIColor::FG::White, ANSIColor::BG::BrightBlue), // Disabled
        ANSIColor(ANSIColor::FG::White, ANSIColor::BG::BrightRed), // BorderActive
        ANSIColor(ANSIColor::FG::White, ANSIColor::BG::Blue), // BorderInactive
        ANSIColor(ANSIColor::FG::White, ANSIColor::BG::BrightBlue) // BorderDisabled
    );

    WindowStack<TChar> winstack;
    for (int i = 0; i < 3; ++i)
    {
        Widget<TChar> close_btn("[X]", Colors::Accent, Quad(0, 0, 0, 0), BoxStyle::Single, ShadowStyle::None);
        close_btn.selectable = true;
        close_btn.on_event = [](Widget<TChar>* self, WindowStack<TChar>* window, const IPEvent& ev,
                                const std::vector<int>& path) -> bool
        {
            if (ev.type == EventType::Select || ev.type == EventType::Click)
            {
                if (window) window->pop(window->selector_idx);
                return true;
            }
            return false;
        };
        Widget<TChar> open_btn("[open]");
        open_btn.selectable = true;
        open_btn.on_event = [](Widget<TChar>* self, WindowStack<TChar>* window, const IPEvent& ev,
                               const std::vector<int>& path) -> bool
        {
            if ((ev.type == EventType::Select || ev.type == EventType::Click) && window)
            {
                // Open a new popup window
                Widget<TChar> close_btn2("[X]", Colors::Accent, Quad(0, 0, 0, 0), BoxStyle::Single,
                                    ShadowStyle::None);
                close_btn2.selectable = true;
                close_btn2.on_event = [](Widget<TChar>* self, WindowStack<TChar>* window, const IPEvent& ev,
                                         const std::vector<int>& path) -> bool
                {
                    if (ev.type == EventType::Select || ev.type == EventType::Click)
                    {
                        if (window) window->pop(window->selector_idx);
                        return true;
                    }
                    return false;
                };
                Widget<TChar> popup2(WidgetLayout::Vertical, {
                                    close_btn2,
                                    Widget<TChar>("New popup!", Colors::Primary, Quad(1, 1, 1, 1), BoxStyle::None,
                                             ShadowStyle::None)
                                }, Colors::Primary, Quad(2, 2, 2, 2), Quad(1, 1, 1, 1), BoxStyle::Double,
                                ShadowStyle::Shadow, {10, 5});
                popup2.on_event = [](Widget<TChar>* self, WindowStack<TChar>* window, const IPEvent& ev,
                                     const std::vector<int>& path) -> bool
                {
                    if (ev.type == EventType::OnCreate)
                    {
                        //if (!self->_children.empty()) self->_children[0].selected = true;
                        return true;
                    }
                    return false;
                };
                window->push(popup2, (std::size_t)IPWindowFlags::Modal);
                return true;
            }
            return false;
        };
        Widget<TChar> popup(WidgetLayout::Vertical, {
                           close_btn,
                           open_btn,
                           Widget("Popup window " + std::to_string(i + 1), Colors::Primary, Quad(1, 1, 1, 1),
                                    BoxStyle::None, ShadowStyle::None)
                       }, Colors::Primary, Quad(2, 2, 2, 2), Quad(1, 1, 1, 1), BoxStyle::Double,
                       ShadowStyle::Shadow, {5 * i, 3 * i});
        popup.on_event = [](Widget<TChar>* self, WindowStack<TChar>* window, const IPEvent& ev, const std::vector<int>& path) -> bool
        {
            if (ev.type == EventType::OnCreate)
            {
                //if (!self->_children.empty()) self->_children[0].selected = true;
                return true;
            }
            return false;
        };
        winstack.push(popup);
    }

    auto get_debug_text = [](WindowStack<TChar>& win)
    {
        std::ostringstream dbg;
        dbg << "selector_idx: " << win.selector_idx << "; ";
        dbg << "selection_paths: ";
        for (size_t i = 0; i < win.selection_paths.size(); ++i)
        {
            dbg << "[";
            for (size_t j = 0; j < win.selection_paths[i].size(); ++j)
            {
                dbg << win.selection_paths[i][j];
                if (j + 1 < win.selection_paths[i].size()) dbg << ",";
            }
            dbg << "] ";
        }

        dbg << "; dist: " << win._dbg_best_dist;

        return dbg.str();
    };

    Widget debug_win(get_debug_text(winstack), Colors::Inactive, Quad(0, 0, 0, 0), BoxStyle::Single,
                       ShadowStyle::Fill);
    debug_win._xy = {0, term_h - 4};
    debug_win._wh = {terminal._cols, 4};
    debug_win.selectable = false;
    winstack.push_overlay(debug_win);

    // Main event loop
    while (!winstack.stack.empty())
    {
        // --- Debug overlay window ---
        winstack.overlays[0].set_text(get_debug_text(winstack));
        // --- End debug overlay ---

        terminal.init_matrix(term_h, term_w);
        winstack.render_all(terminal._output_matrix, terminal._color_matrix, style);
        winstack.render_overlays(terminal._output_matrix, terminal._color_matrix, style);
        terminal.render_matrix();

        int c = terminal.getch();
        if (c == 'q') break;
        if (c == '\t')
        {
            winstack.move_selector_tab(1);
            continue;
        }
        if (c == 'x')
        {
            winstack.pop(winstack.selector_idx);
            continue;
        }
        if (c == 10 || c == ' ')
        {
            // Enter or space
            winstack.handle_event(IPEvent(EventType::Select));
            continue;
        }
        if (c == 27)
        {
            // Escape sequence for arrows
            int c2 = terminal.getch();
            if (c2 == '[')
            {
                int c3 = terminal.getch();
                if (c3 == 'A') winstack.handle_event(IPEvent(EventType::ArrowUp));
                else if (c3 == 'B') winstack.handle_event(IPEvent(EventType::ArrowDown));
                else if (c3 == 'C') winstack.handle_event(IPEvent(EventType::ArrowRight));
                else if (c3 == 'D') winstack.handle_event(IPEvent(EventType::ArrowLeft));
            }
        }
    }
}


int main()
{
    using TChar = char;
    test_popup_windows<TChar>();
    CurseTerminal<ANSIColor, TChar>::restore_terminal();
    return 0;
}
