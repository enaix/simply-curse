//
// Created by Flynn on 14.07.2025.
//

#ifndef SIMPLY_CURSE_H
#define SIMPLY_CURSE_H


#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <tuple>
#include <cassert>


namespace curse
{

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define CURSE_IS_POSIX

#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif


// ANSIColor class for handling ANSI color codes
class ANSIColor
{
public:
    enum class FG : int
    {
        Default = 39,
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,
        BrightBlack = 90,
        BrightRed = 91,
        BrightGreen = 92,
        BrightYellow = 93,
        BrightBlue = 94,
        BrightMagenta = 95,
        BrightCyan = 96,
        BrightWhite = 97,
        None = 0
    };

    enum class BG : int
    {
        Default = 49,
        Black = 40,
        Red = 41,
        Green = 42,
        Yellow = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47,
        BrightBlack = 100,
        BrightRed = 101,
        BrightGreen = 102,
        BrightYellow = 103,
        BrightBlue = 104,
        BrightMagenta = 105,
        BrightCyan = 106,
        BrightWhite = 107,
        None = 0
    };

    explicit ANSIColor(FG fg = FG::None, BG bg = BG::None) : _fg(fg), _bg(bg) {}

    ANSIColor(const ANSIColor& other) = default;

    ANSIColor(ANSIColor&& other) noexcept : _fg(other._fg), _bg(other._bg) {}

    ANSIColor& operator=(const ANSIColor& other) = default;

    ANSIColor& operator=(ANSIColor&& other) noexcept
    {
        _fg = other._fg;
        _bg = other._bg;
        return *this;
    }

    [[nodiscard]] std::string code() const
    {
        std::ostringstream oss;
        oss << "\033[" << static_cast<int>(_fg) << ";" << static_cast<int>(_bg) << "m";
        return oss.str();
    }

    static std::string reset() { return "\033[0m"; }

    static ANSIColor None() { return ANSIColor(FG::None, BG::None); }
    [[nodiscard]] bool isna() const { return *this == ANSIColor::None(); }

    bool operator==(const ANSIColor& other) const
    {
        return _fg == other._fg && _bg == other._bg;
    }

    bool operator!=(const ANSIColor& other) const
    {
        return !(*this == other);
    }

    [[nodiscard]] FG fg() const { return _fg; }
    [[nodiscard]] BG bg() const { return _bg; }
    [[nodiscard]] FG& fg() { return _fg; }
    [[nodiscard]] BG& bg() { return _bg; }

    // Mix 2 colors together
    [[nodiscard]] ANSIColor blend(const ANSIColor& with) const
    {
        ANSIColor res = *this;
        if (res.fg() == ANSIColor::FG::None) res.fg() = with.fg();
        if (res.bg() == ANSIColor::BG::None) res.bg() = with.bg();
        return res;
    }

    // Overlay this color with another
    [[nodiscard]] ANSIColor overlay(const ANSIColor& with) const
    {
        ANSIColor res = *this;
        if (with.fg() != ANSIColor::FG::None) res.fg() = with.fg();
        if (with.bg() != ANSIColor::BG::None) res.bg() = with.bg();
        return res;
    }

private:
    FG _fg;
    BG _bg;
};


enum class Colors : int
{
    Primary,   // Main color
    Secondary, // Secondary color
    Accent,    // Color accent (primary)
    Accent2,   // Color accent (secondary)
    Accent3,   // Color accent (tertiary)
    Selected,  // Selected element highlight
    Inactive,  // Not selected
    Disabled,  // Inactive window
    BorderActive,
    BorderInactive,
    BorderDisabled, // Inactive window
    None // last elem
};


// AppStyle: Template for color palettes (e.g., ANSI, TrueColor)
template <class TColor>
class AppStyle
{
protected:
    std::array<TColor, static_cast<size_t>(Colors::None)> _colors;
    int _color_overload = -1;

public:
    template <class... TColors>
    explicit AppStyle(TColors... c) : _colors{c...} {}

    TColor get_color(Colors color) const
    {
        if (_color_overload != -1)
            return _colors[_color_overload];

        for (int i = static_cast<int>(color); i >= 0; i--)
            if (!_colors[i].isna())
                return _colors[i];
        return TColor::None();
    }
};


enum class WidgetLayout
{
    Horizontal, // Use margin and padding to position children
    Vertical, // ditto
    Floating, // Use widgets _xy to position
    Text // Just text (std::string), no children
};

class __attribute__((packed)) BoxStyle
{
public:
    char tl='\0', tr='\0', bl='\0', br='\0', hline='\0', vline='\0';

    constexpr BoxStyle(char tl_i, char tr_i, char bl_i, char br_i, char hline_i, char vline_i) : tl(tl_i), tr(tr_i), bl(bl_i), br(br_i), hline(hline_i), vline(vline_i) {}

    constexpr BoxStyle(int tl_i, int tr_i, int bl_i, int br_i, int hline_i, int vline_i) : tl(tl_i), tr(tr_i), bl(bl_i), br(br_i), hline(hline_i), vline(vline_i) {}

    [[nodiscard]] bool isna() const { return tl == '\0'; }
};

// Symbolic borders
static constexpr BoxStyle NoBoxStyle    {'\0', '\0', '\0', '\0', '\0', '\0'};
static constexpr BoxStyle SingleBoxStyle{'+', '+', '+', '+', '-', '|'};
static constexpr BoxStyle DoubleBoxStyle{'#', '#', '#', '#', '=', 'H'};

// Box-drawing borders
//static constexpr BoxStyle SingleBoxStyleDOS{0xda, 0xbf, 0xc0, 0xd9, 0xc4, 0xb3};
//static constexpr BoxStyle DoubleBoxStyleDOS{'╔', '╗', '╚', '╝', '═', '║'};

// TODO fix shadow rendering
enum class ShadowStyle
{
    None,
    Fill, // No shadow, just background color
    Shadow // Background color and a shadow
};


class Quad : public std::tuple<int, int, int, int> // left, top, right, bottom
{
public:
    using std::tuple<int, int, int, int>::tuple; // using constructors

    // 1d swizzling
    int& l() { return std::get<0>(*this); }
    int& t() { return std::get<1>(*this); }
    int& r() { return std::get<2>(*this); }
    int& b() { return std::get<3>(*this); }

    [[nodiscard]] int l() const { return std::get<0>(*this); }
    [[nodiscard]] int t() const { return std::get<1>(*this); }
    [[nodiscard]] int r() const { return std::get<2>(*this); }
    [[nodiscard]] int b() const { return std::get<3>(*this); }

    // Access quad as a native tuple object. No overhead
    std::tuple<int, int, int, int>& tup() { return static_cast<std::tuple<int, int, int, int>&>(*this); }

    // Operators overload
    Quad& operator+=(const Quad& rhs) { l() += rhs.l(); t() += rhs.t(); r() += rhs.r(); b() += rhs.b(); return *this; }
    Quad& operator-=(const Quad& rhs) { l() -= rhs.l(); t() -= rhs.t(); r() -= rhs.r(); b() -= rhs.b(); return *this; }
    Quad& operator*=(const Quad& rhs) { l() *= rhs.l(); t() *= rhs.t(); r() *= rhs.r(); b() *= rhs.b(); return *this; }
    Quad& operator/=(const Quad& rhs) { l() /= rhs.l(); t() /= rhs.t(); r() /= rhs.r(); b() /= rhs.b(); return *this; }
    Quad& operator+=(int a) { l() += a; t() += a; r() += a; b() += a; return *this; }
    Quad& operator-=(int a) { l() -= a; t() -= a; r() -= a; b() -= a; return *this; }
    Quad& operator*=(int a) { l() *= a; t() *= a; r() *= a; b() *= a; return *this; }
    Quad& operator/=(int a) { l() /= a; t() /= a; r() /= a; b() /= a; return *this; }
};


class Point : public std::pair<int, int>
{
public:
    using std::pair<int, int>::pair; // using constructors

    // 1d swizzling
    int& x() { return this->first; }
    int& y() { return this->second; }
    int& w() { return this->first; } // width
    int& h() { return this->second; } // height

    [[nodiscard]] int x() const { return this->first; }
    [[nodiscard]] int y() const { return this->second; }
    [[nodiscard]] int w() const { return this->first; }
    [[nodiscard]] int h() const { return this->second; }

    // Access point as a native pair object. No overhead
    std::pair<int, int>& pair() { return static_cast<std::pair<int, int>&>(*this); }

    // Operators overload
    friend Point operator+(const Point& lhs, const Point& rhs) { Point res; res.x() += rhs.x(); res.y() += rhs.y(); return res; }
    friend Point operator-(const Point& lhs, const Point& rhs) { Point res; res.x() -= rhs.x(); res.y() -= rhs.y(); return res; }
    friend Point operator*(const Point& lhs, const Point& rhs) { Point res; res.x() *= rhs.x(); res.y() *= rhs.y(); return res; }
    friend Point operator/(const Point& lhs, const Point& rhs) { Point res; res.x() /= rhs.x(); res.y() /= rhs.y(); return res; }

    Point& operator+=(const Point& rhs) { x() += rhs.x(); y() += rhs.y(); return *this; }
    Point& operator-=(const Point& rhs) { x() -= rhs.x(); y() -= rhs.y(); return *this; }
    Point& operator*=(const Point& rhs) { x() *= rhs.x(); y() *= rhs.y(); return *this; }
    Point& operator/=(const Point& rhs) { x() /= rhs.x(); y() /= rhs.y(); return *this; }
    Point& operator+=(int a) { x() += a; y() += a; return *this; }
    Point& operator-=(int a) { x() -= a; y() -= a; return *this; }
    Point& operator*=(int a) { x() *= a; y() *= a; return *this; }
    Point& operator/=(int a) { x() /= a; y() /= a; return *this; }
};


// Event types for user interaction
enum class EventType
{
    None,
    Select,
    Click, // KeyDown
    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,
    OnCreate,
    OnDestroy
};

struct IPEvent
{
    EventType type;
    int key; // For Click, can be ASCII or special key code
    explicit IPEvent(EventType t = EventType::None, int k = 0) : type(t), key(k) {}
};


template<class TChar> class Widget;
template<class TChar> class WindowStack;
// Event handler signature now returns bool for event handling
template<class TChar>
using EventHandler = bool(*)(Widget<TChar>*, WindowStack<TChar>*, const IPEvent&, const std::vector<int>& path);


template<class TChar>
class Widget
{
public:
    Point _xy; // Relative to the parent
    Point _wh; // width, height
    Colors _color;
    Quad _margin; // left, top, right, bottom
    Quad _padding; // left, top, right, bottom
    std::vector<Widget> _children;
    std::basic_string<TChar> _content; // Text content
    ShadowStyle _shadow_style;

    WidgetLayout _layout;

    BoxStyle _box_style;

    bool _selectable = false;
    EventHandler<TChar> on_event = nullptr;


    Widget()
        : _xy{0, 0}, _wh{0, 0}, _color(Colors::Primary),
          _margin{0, 0, 0, 0}, _padding{0, 0, 0, 0}, _shadow_style(ShadowStyle::None)
          , _layout(WidgetLayout::Text), _box_style(NoBoxStyle) {}

    // Text box
    explicit Widget(std::basic_string<TChar> text, Colors color = Colors::Primary, Quad margin = Quad(0, 0, 0, 0),
                      const BoxStyle box = NoBoxStyle, const ShadowStyle shadow = ShadowStyle::None)
        : _content(std::move(text)), _color(color), _margin(std::move(margin)), _box_style(box), _shadow_style(shadow),
          _layout(WidgetLayout::Text) {}

    // Horizontal/Vertical layout
    Widget(WidgetLayout layout, std::vector<Widget> children, Colors color = Colors::Primary,
             Quad margin = Quad(0, 0, 0, 0), Quad padding = Quad(0, 0, 0, 0), BoxStyle box = NoBoxStyle,
             ShadowStyle shadow = ShadowStyle::None, Point xy = {0, 0})
        : _xy(xy), _wh{0, 0}, _color(color), _margin(margin), _padding(padding), _children(std::move(children)),
          _content(), _shadow_style(shadow), _layout(layout), _box_style(box) {}

    // Floating layout
    Widget(const Point& xy, std::vector<Widget> children, Colors color = Colors::Primary,
             Quad margin = Quad(0, 0, 0, 0), Quad padding = Quad(0, 0, 0, 0), BoxStyle box = NoBoxStyle,
             ShadowStyle shadow = ShadowStyle::None)
        : _xy(xy), _wh{0, 0}, _color(color), _margin(margin), _padding(padding), _children(std::move(children)),
          _content(), _shadow_style(shadow), _layout(WidgetLayout::Floating), _box_style(box) {}

    // Full constructor
    Widget(const Point& xy, const Point& wh, Colors color, Quad margin, Quad padding,
             std::vector<Widget> children, std::basic_string<TChar> content, ShadowStyle shadow, WidgetLayout layout,
             BoxStyle box)
        : _xy(xy), _wh(wh), _color(color), _margin(margin), _padding(padding), _children(std::move(children)),
          _content(std::move(content)), _shadow_style(shadow), _layout(layout), _box_style(box) {}

    void set_text(const std::basic_string<TChar>& s) { _content = s; }

    void set_selectable(bool selectable) { _selectable = selectable; }

    Widget<TChar>& add_child(Widget<TChar>& wid) { _children.push_back(std::move(wid)); return _children.front(); }

    Widget<TChar>& add_child(const Widget<TChar>& wid) { _children.push_back(wid); return _children.front(); }

    // Layout/rendering logic
    // ======================

    // Helper to get effective color from palette if set
    template <class TStyle>
    ANSIColor get_effective_color(const TStyle& style, bool active_window, bool selected) const
    {
        if (!active_window)
            return style.get_color(Colors::Disabled);
        if (_selectable)
        {
            if (selected)
                return style.get_color(Colors::Selected);
            return style.get_color(Colors::Inactive);
        }
        return style.get_color(_color);
    }

    // Helper to get effective border color from palette if set
    template <class TStyle>
    ANSIColor get_border_color(const TStyle& style, bool active_window, bool selected) const
    {
        if (!active_window)
            return style.get_color(Colors::BorderDisabled);

        if (_selectable && selected)
            return style.get_color(Colors::BorderActive);

        return style.get_color(Colors::BorderInactive);
    }

    template <class TColor>
    static void draw_box(std::vector<std::basic_string<TChar>>& matrix, std::vector<std::vector<TColor>>& color_matrix, int x, int y,
                         int w, int h, BoxStyle style, const TColor& color)
    {
        if (w < 2 || h < 2) return;
        int x2 = x + w - 1, y2 = y + h - 1;

        if (style.isna()) // is a NoBoxStyle
            return;

        auto set = [&](int row, int col, char ch)
        {
            if (row >= 0 && row < (int)matrix.size() && col >= 0 && col < (int)matrix[0].size())
            {
                matrix[row][col] = ch;
                color_matrix[row][col] = color_matrix[row][col].overlay(color);
            }
        };

        // Corners
        set(y, x, style.tl);
        set(y, x2, style.tr);
        set(y2, x, style.bl);
        set(y2, x2, style.br);
        // Top and bottom edges
        for (int i = x + 1; i < x2; ++i)
        {
            set(y, i, style.hline);
            set(y2, i, style.hline);
        }
        // Left and right edges
        for (int i = y + 1; i < y2; ++i)
        {
            set(i, x, style.vline);
            set(i, x2, style.vline);
        }
    }

    void layout()
    {
        auto [ml, mt, mr, mb] = _margin.tup();
        auto [pl, pt, pr, pb] = _padding.tup();

        switch (_layout)
        {
        case WidgetLayout::Horizontal:
            {
                int x = 0, max_h = 0;

                for (int i = 0; i < _children.size(); i++)
                {
                    _children[i].layout();

                    if (i > 0)
                        x += pl;
                    if (i < _children.size() - 1)
                        x += pr;

                    x += _children[i]._wh.w();
                    max_h = std::max(max_h, _children[i]._wh.h());
                }
                _wh.w() = x + ml + mr;
                _wh.h() = max_h + mt + mb;
                break;
            }
        case WidgetLayout::Vertical:
            {
                int y = 0, max_w = 0;
                for (int i = 0; i < _children.size(); i++)
                {
                    _children[i].layout();

                    if (i > 0)
                        y += pl;
                    if (i < _children.size() - 1)
                        y += pr;

                    y += _children[i]._wh.h();
                    max_w = std::max(max_w, _children[i]._wh.w());
                }
                _wh.w() = max_w + ml + mr;
                _wh.h() = y + mt + mb;
                break;
            }
        case WidgetLayout::Floating:
            {
                int max_x = 0, max_y = 0;
                for (auto& child : _children)
                {
                    child.layout();
                    int child_x = child._xy.x() + child._wh.w();
                    int child_y = child._xy.y() + child._wh.h();
                    max_x = std::max(max_x, child_x);
                    max_y = std::max(max_y, child_y);
                }
                _wh.w() = max_x + ml + mr;
                _wh.h() = max_y + mt + mb;
                break;
            }
        case WidgetLayout::Text:
            {
                _wh.w() = static_cast<int>(_content.size()) + ml + mr;
                _wh.h() = 1 + mt + mb;
                break;
            }
        }

        // If box is present, increment size for box border
        if (!_box_style.isna())
        {
            //_margin += 1;
            _wh += 2;
        }
    }

    template <class TColor, template<class> class TStyle>
    void render(std::vector<std::basic_string<TChar>>& matrix, std::vector<std::vector<TColor>>& color_matrix,
                const TStyle<TColor>& style, bool active_window, int x, int y,
                const TColor& parent_color = TColor::None(), bool top_level = false, std::vector<int> cur_path = {},
                const std::vector<int>* selected_path = nullptr)
    {
        bool selected;
        if (selected_path && cur_path == *selected_path)
            selected = true; // Force lazy eval
        else
            selected = false;

        ANSIColor effective_color = get_effective_color(style, active_window, selected).blend(parent_color);
        auto [ml, mt, mr, mb] = _margin.tup();
        auto [pl, pt, pr, pb] = _padding.tup();
        // Opaque fill for top-level window
        if (top_level)
        {
            for (int row = 0; row < _wh.h(); ++row)
            {
                int abs_y = y + row;
                if (abs_y < 0 || abs_y >= (int)color_matrix.size()) continue;
                for (int col = 0; col < _wh.w(); ++col)
                {
                    int abs_x = x + col;
                    if (abs_x < 0 || abs_x >= (int)color_matrix[0].size()) continue;
                    matrix[abs_y][abs_x] = ' ';
                    color_matrix[abs_y][abs_x] = effective_color;
                }
            }
        }
        // Draw box if needed
        if (!_box_style.isna())
        {
            ANSIColor border_color = get_border_color(style, active_window, selected).blend(parent_color);
            draw_box(matrix, color_matrix, x, y, _wh.w(), _wh.h(), _box_style, border_color);
            x += 1;
            y += 1;
        }

        cur_path.push_back(0); // Add last elem

        switch (_layout)
        {
        case WidgetLayout::Horizontal:
            {
                int cur_x = x + ml;
                for (int i = 0; i < _children.size(); i++)
                {
                    cur_path.front() = i;
                    if (i > 0)
                        cur_x += pl;
                    _children[i].render(matrix, color_matrix, style, active_window, cur_x, y + mt, effective_color,
                                        false, cur_path, selected_path);
                    if (i < _children.size() - 1)
                        cur_x += pr;
                    cur_x += _children[i]._wh.w();
                }
                break;
            }
        case WidgetLayout::Vertical:
            {
                int cur_y = y + mt;
                for (int i = 0; i < _children.size(); i++)
                {
                    cur_path.front() = i;
                    if (i > 0)
                        cur_y += pt;
                    _children[i].render(matrix, color_matrix, style, active_window, x + ml, cur_y, effective_color,
                                        false, cur_path, selected_path);
                    if (i < _children.size() - 1)
                        cur_y += pb;
                    cur_y += _children[i]._wh.h();
                }
                break;
            }
        case WidgetLayout::Floating:
            {
                for (auto& child : _children)
                {
                    cur_path.front()++;
                    child.render(matrix, color_matrix, style, active_window, x + child._xy.x() + ml,
                                 y + child._xy.y() + mt, effective_color, false, cur_path, selected_path);
                }
                break;
            }
        case WidgetLayout::Text:
            {
                int cur_x = x + ml, cur_y = y + mt;
                if (cur_y < static_cast<int>(matrix.size()) && cur_x + static_cast<int>(_content.size()) <= static_cast<
                    int>(matrix[0].size()))
                {
                    for (size_t i = 0; i < _content.size(); ++i)
                    {
                        matrix[cur_y][cur_x + i] = _content[i];
                        color_matrix[cur_y][cur_x + i] = color_matrix[cur_y][cur_x + i].overlay(effective_color);
                    }
                }
                break;
            }
        }

        // Fill the rectangle with color if the flag is set
        int box_offset = (_box_style.isna() ? 0 : 2);
        int shadow_size = (_box_style.isna() ? 1 : 2);

        int shadow_rows = _wh.h() - box_offset + shadow_size - 1;
        int shadow_cols = _wh.w() - box_offset + shadow_size - 1;

        switch (_shadow_style)
        {
        case ShadowStyle::Fill:
            for (int row = 0; row < _wh.h() - box_offset; ++row)
            {
                int abs_y = y + row;
                if (abs_y < 0 || abs_y >= static_cast<int>(color_matrix.size())) continue;
                for (int col = 0; col < _wh.w() - box_offset; ++col)
                {
                    int abs_x = x + col;
                    if (abs_x < 0 || abs_x >= static_cast<int>(color_matrix[0].size())) continue;
                    color_matrix[abs_y][abs_x] = color_matrix[abs_y][abs_x].blend(effective_color);
                }
            }
            break;
        case ShadowStyle::Shadow:
            for (int row = 0; row <= shadow_rows; ++row)
            {
                int abs_y = y + row;
                if (abs_y < 0 || abs_y >= static_cast<int>(color_matrix.size())) continue;
                for (int col = 0; col <= shadow_cols; ++col)
                {
                    int abs_x = x + col;
                    if (abs_x < 0 || abs_x >= static_cast<int>(color_matrix[0].size())) continue;
                    color_matrix[abs_y][abs_x] = color_matrix[abs_y][abs_x].blend(effective_color);
                }
            }
            break;
        default:
            break;
        }
    }
};


// Helper: recursively find nearest _selectable widget in a direction, returning path
template<class TChar>
void find_nearest_selectable_recursive(const std::vector<Widget<TChar>>& widgets, std::vector<int> path,
                                              const Point& cur_xy, EventType dir, std::vector<int>& best_path,
                                              int& best_dist, const std::vector<int>& current_sel_path = {},
                                              Point accumulated_xy = {})
{
    path.push_back(0);
    for (int i = 0; i < (int)widgets.size(); ++i)
    {
        path.front() = i;
        // Skip if this is the currently selected widget
        if (!current_sel_path.empty() && path == current_sel_path) continue;

        Point abs_xy = accumulated_xy + widgets[i]._xy;
        if (widgets[i]._selectable)
        {
            int dx = abs_xy.x() - cur_xy.x();
            int dy = abs_xy.y() - cur_xy.y();
            bool in_quadrant = false;
            switch (dir)
            {
            // TODO fix quadrant selection
            case EventType::ArrowUp: in_quadrant = (std::abs(dy) >= std::abs(dx));
                break;
            case EventType::ArrowDown: in_quadrant = (std::abs(dy) >= std::abs(dx));
                break;
            case EventType::ArrowLeft: in_quadrant = (std::abs(dx) >= std::abs(dy));
                break;
            case EventType::ArrowRight: in_quadrant = (std::abs(dx) >= std::abs(dy));
                break;
            default: break;
            }
            if (in_quadrant)
            {
                int dist = dx * dx + dy * dy;
                if (dist < best_dist)
                {
                    best_path = path;
                    best_dist = dist;
                }
            }
        }
        if (!widgets[i]._children.empty())
        {
            find_nearest_selectable_recursive(widgets[i]._children, path, cur_xy, dir, best_path, best_dist,
                                              current_sel_path, abs_xy);
        }
    }
}


enum class IPWindowFlags : std::size_t
{
    Modal = 0x1,
};


// Window stack and selector logic
template<class TChar>
class WindowStack
{
public:
    std::vector<Widget<TChar>> stack; // Topmost is last
    int selector_idx = -1; // Index of selected widget in stack
    std::vector<std::vector<int>> selection_paths; // Selected widget in each window
    std::vector<Widget<TChar>> overlays; // Overlay windows, not selectable
    std::vector<std::size_t> flags;

    int _dbg_best_dist = INT_MAX; // DEBUG: best distance in selector

    WindowStack() = default;

    void push(Widget<TChar> w, std::size_t win_flags = 0)
    {
        stack.push_back(std::move(w));
        flags.push_back(win_flags);
        selector_idx = static_cast<int>(stack.size()) - 1;
        // Default: select first selectable child in new window
        std::vector<int> path;
        find_first_selectable_path(stack.back()._children, path);
        selection_paths.push_back(path);
        if (stack[selector_idx].on_event)
            stack[selector_idx].on_event(&stack[selector_idx], this, IPEvent(EventType::OnCreate), {0});
        // Widget should be 0
    }

    // Add overlay window (not selectable)
    void push_overlay(Widget<TChar> w)
    {
        overlays.push_back(std::move(w));
    }

    void pop(int index)
    {
        if (!stack.empty() && index >= 0)
        {
            if (stack[index].on_event)
                stack[index].on_event(&stack[index], this, IPEvent(EventType::OnDestroy), {0});
            // We are deleting the whole window

            stack.erase(stack.begin() + index);
            selection_paths.erase(selection_paths.begin() + index);
            flags.erase(flags.begin() + index);

            if (stack.size() - 1 >= index)
                selector_idx = index;
            else if (!stack.empty())
                selector_idx = static_cast<int>(stack.size()) - 1;
            else
                selector_idx = -1;
        }
    }

    Widget<TChar>* top() { return stack.empty() ? nullptr : &stack.back(); }

    [[nodiscard]] bool check_modal_flag() const
    {
        if (selector_idx < 0 || selector_idx >= stack.size())
            return false;

        return flags[selector_idx] & (std::size_t)IPWindowFlags::Modal;
    }

    // Tab cycles through windows
    void move_selector_tab(int dir)
    {
        if (stack.empty()) return;
        if (check_modal_flag()) return;
        const int n = static_cast<int>(stack.size());
        int start = selector_idx;
        for (int i = 1; i <= n; ++i)
        {
            int idx = (start + dir * i + n) % n;
            selector_idx = idx;
            return;
        }
    }

    // Find first selectable widget path (depth-first)
    static bool find_first_selectable_path(const std::vector<Widget<TChar>>& widgets, std::vector<int>& path)
    {
        for (int i = 0; i < (int)widgets.size(); ++i)
        {
            if (widgets[i]._selectable)
            {
                path.push_back(i);
                return true;
            }
            if (!widgets[i]._children.empty())
            {
                path.push_back(i);
                if (find_first_selectable_path(widgets[i]._children, path)) return true;
                path.pop_back();
            }
        }
        return false;
    }

    // Move selection within the top window's widgets in a direction (recursive)
    void move_child_selector_dir(EventType dir)
    {
        if (selector_idx < 0 || selector_idx >= (int)stack.size()) return;
        Widget<TChar>& root = stack[selector_idx];
        std::vector<int>& sel_path = selection_paths[selector_idx];
        Point cur_xy = {0, 0};
        std::vector<Widget<TChar>>* cur_level = &root._children;

        for (int d = 0; d < (int)sel_path.size(); ++d)
        {
            int idx = sel_path[d];
            if (idx < 0 || idx >= (int)cur_level->size()) break;
            cur_xy += (*cur_level)[idx]._xy; // We need to accumulate total pos, since _xy is relative
            cur_level = &(*cur_level)[idx]._children;
        }
        std::vector<int> best_path;
        _dbg_best_dist = INT_MAX;
        find_nearest_selectable_recursive(root._children, {}, cur_xy, dir, best_path, _dbg_best_dist, sel_path);
        if (!best_path.empty()) sel_path = best_path;
    }

    // Route event to selected window and its selected child (recursive, path-based)
    void handle_event(const IPEvent& ev)
    {
        if (selector_idx >= 0 && selector_idx < stack.size())
        {
            Widget<TChar>* root = &stack[selector_idx];
            std::vector<int>& sel_path = selection_paths[selector_idx];

            // Try to handle event recursively from leaf to root
            for (int d = (int)sel_path.size(); d >= 0; --d)
            {
                std::vector<int> subpath(sel_path.begin(), sel_path.begin() + d);
                Widget<TChar>* cur = root;
                std::vector<Widget<TChar>>* cur_level = &root->_children;

                // Inefficient logic
                for (int i = 0; i < (int)subpath.size(); ++i)
                {
                    int idx = subpath[i];
                    if (idx < 0 || idx >= (int)cur_level->size())
                    {
                        cur = nullptr;
                        break;
                    }
                    cur = &(*cur_level)[idx];
                    cur_level = &cur->_children;
                }
                if (cur && cur->on_event && cur->on_event(cur, this, ev, subpath))
                {
                    return;
                }
            }
            // If not handled, process at root widget level
            if (root->on_event && root->on_event(root, this, ev, {}))
                return;

            // THEN we handle it on window level
            window_event_process(ev);
        }
    }

    void window_event_process(const IPEvent& ev)
    {
        if (int(ev.type) >= (int)EventType::ArrowUp && int(ev.type) <= (int)EventType::ArrowRight)
            move_child_selector_dir(ev.type);
    }

    // Render all windows, overlays last. Overlays are not _selectable.
    template <class TColor, template<class> class TStyle>
    void render_all(std::vector<std::basic_string<TChar>>& matrix, std::vector<std::vector<TColor>>& color_matrix,
                    const TStyle<TColor>& style)
    {
        int n = (int)stack.size();
        if (n == 0) return;
        int start = (selector_idx + 1) % n;
        for (int i = 0; i < n; ++i)
        {
            int idx = (start + i) % n;
            bool active = (idx == selector_idx);
            stack[idx].layout();
            stack[idx].render(matrix, color_matrix, style, active, 2 + 2 * idx, 2 + 2 * idx, TColor::None(), true, {},
                              (active ? &selection_paths[idx] : nullptr));
        }
    }

    template <class TColor, template<class> class TStyle>
    void render_overlays(std::vector<std::basic_string<TChar>>& matrix, std::vector<std::vector<TColor>>& color_matrix,
                         const TStyle<TColor>& style)
    {
        // Render overlays last (not _selectable, not active)
        for (auto& overlay : overlays)
        {
            overlay.layout();
            overlay.render(matrix, color_matrix, style, false, overlay._xy.x(), overlay._xy.y(), TColor::None(), true);
        }
    }
};


// POSIX terminal output with a double buffer
template<class TColor, class TChar>
class CurseTerminal
{
public:
    std::ostream& _os;

    // Pointers to the data
    //const std::vector<GSymbol>& _stack; // Symbols stack
    //const std::array<std::size_t, ContextSize>& _context; //

    // Output matrix for rendering: each string is a row
    std::vector<std::basic_string<TChar>> _output_matrix;
    std::vector<std::vector<TColor>> _color_matrix;
    std::vector<std::basic_string<TChar>> _prev_output_matrix;
    std::vector<std::vector<TColor>> _prev_color_matrix;
    std::size_t _rows = 0;
    std::size_t _cols = 0;
    bool _first_frame = true;

    explicit CurseTerminal(std::ostream& os) : _os(os) {}

    /*void set_data(const std::vector<GSymbol>& stack, const std::array<std::size_t, ContextSize>& context) {
        _stack = stack;
        _context = context;
    }*/

    void init_matrix(std::size_t rows, std::size_t cols)
    {
        _rows = rows;
        _cols = cols;
        _output_matrix.assign(rows, std::basic_string<TChar>(cols, ' '));
        _color_matrix.assign(rows, std::vector<TColor>(cols, TColor::None()));
        _prev_output_matrix.assign(rows, std::basic_string<TChar>(cols, ' '));
        _prev_color_matrix.assign(rows, std::vector<TColor>(cols, TColor::None()));
        _first_frame = true;
    }

    void set_cell(std::size_t row, std::size_t col, char value, const TColor& color = TColor())
    {
        if (row < _rows && col < _cols)
        {
            _output_matrix[row][col] = value;
            _color_matrix[row][col] = color;
        }
    }

    void set_text(std::size_t row, std::size_t col, const std::basic_string<TChar>& text, const TColor& color = TColor())
    {
        if (row < _rows && col + text.size() <= _cols)
        {
            for (size_t i = 0; i < text.size(); ++i)
            {
                _output_matrix[row][col + i] = text[i];
                _color_matrix[row][col + i] = color;
            }
        }
    }

    void init_renderer() const
    {
        // Enter alternate screen buffer and hide cursor for smooth rendering
        _os << "\033[?1049h"; // Enter alternate buffer
    }

    void render_matrix()
    {
        if (_first_frame)
        {
            _os << "\033[2J\033[H"; // Clear and home
            _first_frame = false;
        }
        _os << "\033[?25l"; // Hide cursor
        for (std::size_t r = 0; r < _rows; ++r)
        {
            for (std::size_t c = 0; c < _cols; ++c)
            {
                if (_output_matrix[r][c] != _prev_output_matrix[r][c] ||
                    _color_matrix[r][c] != _prev_color_matrix[r][c])
                {
                    // Move cursor (1-based for ANSI)
                    _os << "\033[" << (r + 1) << ";" << (c + 1) << "H";
                    _os << _color_matrix[r][c].code() << _output_matrix[r][c] << TColor::reset();
                }
            }
        }
        _os.flush();
        // Save current frame as previous
        _prev_output_matrix = _output_matrix;
        _prev_color_matrix = _color_matrix;
    }

    // Call this on program exit to restore the screen and cursor
    static void restore_terminal(std::ostream& os = std::cout)
    {
        os << "\033[?25h\033[?1049l" << std::flush; // Show cursor, exit alt buffer
    }

    // Helper for raw input
    static int getch()
    {
#ifdef CURSE_IS_POSIX
        termios oldt{};
        termios newt{};
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        int c = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return c;
#else
        return -1;
#endif
    }

    static int get_terminal_width()
    {
#ifdef CURSE_IS_POSIX
        winsize w{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) return w.ws_col;
#endif
        return 80;
    }

    static int get_terminal_height()
    {
#ifdef CURSE_IS_POSIX
        winsize w{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) return w.ws_row;
#endif
        return 24;
    }

    void print()
    {
        // Render the output
        render_matrix();
    }
};

} // namespace curse

#endif //SIMPLY_CURSE_H
