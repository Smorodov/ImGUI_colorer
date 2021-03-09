#include <iostream>
#include <mutex>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "boost/thread.hpp"
#include "boost/signals2.hpp"
#include "Observer.h"
#include "Observable.h"


#include "LogWindow.h"
#include "TextEditor.h"
#include <string>

#include "boost/assign/list_of.hpp"

//For Custom widget
#include "ImGuiFileBrowser.h"

static LogWindow log_win;
static imgui_addons::ImGuiFileBrowser file_dialog;
static bool show_open_dialog = false;
static bool show_save_dialog = false;
static bool show_select_dialog = false;

// Demonstrate using DockSpace() to create an explicit docking node within an existing window.
// Note that you already dock windows into each others _without_ a DockSpace() by just moving windows
// from their title bar (or by holding SHIFT if io.ConfigDockingWithShift is set).
// DockSpace() is only useful to construct to a central location for your application.
void ShowExampleAppDockSpace(bool* p_open)
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
        // ShowDockingDisabledMessage();
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Docking"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

            if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            ImGui::Separator();
            if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
                *p_open = false;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}


// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// -----------------------------
//
// -----------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
// -----------------------------
//
// -----------------------------
void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// -----------------------------
//
// -----------------------------
static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
// -----------------------------
//
// -----------------------------
struct MainWindowObservers
{
    enum
    {
        OnButtonPressEvent,
        OnRunEvent,
        OnWorkerEvent,
        OnCloseEvent
    };
    using ObserverTable = std::tuple<
        Observer<void(std::string buttonName)>,
        Observer<void(void)>,
        Observer<void(void)>,
        Observer<void(void)>
    >;
};


// -----------------------------
//
// -----------------------------
class MainWindow : public Observable< MainWindowObservers>
{
public:

    std::string LocaleName;
    int width;
    int height;
    int sidePanelWidth;
    // Main window
    GLFWwindow* window;

    bool isRunning;
    std::mutex m;
    boost::thread* thr;
    TextEditor editor;
    
    MainWindow()
    {
        LocaleName = "ru_RU.utf8";
        setlocale(LC_ALL, LocaleName.c_str());
        // Size of window
        width = 1024;
        height = 768;
        // Width of side panel
        sidePanelWidth = 300;
        // Main window
        window = nullptr;

        isRunning = false;
        thr = nullptr;
    }

    ~MainWindow()
    {
        if (thr != nullptr)
        {
            thr->interrupt();
            thr->join();
            delete thr;
            thr = nullptr;
        }
    }

    // -----------------------------
     // Отрисовка редактора кода
     // -----------------------------
    void RenderEditor(void)
    {
        enum StringID
        {
            Preview,
            File,
            Save,
            Load,
            Editor,
            ReadOnly,
            Undo,
            Redo,
            Cut,
            Copy,
            Paste,
            Delete,
            Insert,
            SelectAll,
            View,
            DarkTheme,
            LightTheme,
            OldSchoolTheme,
            Strings
        };

        std::map<StringID, std::string> stringToCaption = boost::assign::map_list_of
        (Preview, u8"Предпросмотр")
            (File, u8"Файл")
            (Save, u8"Сохранить")
            (Load, u8"Считать")
            (Editor, u8"Редактор")
            (ReadOnly, u8"Только чтение")
            (Undo, u8"Откат")
            (Redo, u8"Вощврат")
            (Cut, u8"Вырезать")
            (Copy, u8"Копировать")
            (Paste, u8"Вклеить")
            (Delete, u8"Стереть")
            (Insert, u8"Вставка")
            (SelectAll, u8"Выделить все")
            (View, u8"Вид")
            (DarkTheme, u8"Темная тема")
            (LightTheme, u8"Светлая тема")
            (OldSchoolTheme, u8"OldSchool")
            (Strings, u8"Строк");

        //ImGui::Begin(stringToCaption[Preview].c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar
        //    | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::Begin(stringToCaption[Preview].c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
        auto cpos = editor.GetCursorPosition();
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(stringToCaption[File].c_str()))
            {
                if (ImGui::MenuItem(stringToCaption[Save].c_str()))
                {
                    auto textToSave = editor.GetText();
                    /// save text....
                }

                if (ImGui::MenuItem(stringToCaption[Load].c_str()))
                {

                }

                ImGui::EndMenu();
            }



            if (ImGui::BeginMenu(stringToCaption[Editor].c_str()))
            {
                bool ro = editor.IsReadOnly();
                if (ImGui::MenuItem(stringToCaption[ReadOnly].c_str(), nullptr, &ro))
                    editor.SetReadOnly(ro);
                ImGui::Separator();

                if (ImGui::MenuItem(stringToCaption[Undo].c_str(), "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                    editor.Undo();
                if (ImGui::MenuItem(stringToCaption[Redo].c_str(), "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
                    editor.Redo();

                ImGui::Separator();

                if (ImGui::MenuItem(stringToCaption[Copy].c_str(), "Ctrl-C", nullptr, editor.HasSelection()))
                    editor.Copy();
                if (ImGui::MenuItem(stringToCaption[Cut].c_str(), "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                    editor.Cut();
                if (ImGui::MenuItem(stringToCaption[Delete].c_str(), "Del", nullptr, !ro && editor.HasSelection()))
                    editor.Delete();
                if (ImGui::MenuItem(stringToCaption[Insert].c_str(), "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                    editor.Paste();

                ImGui::Separator();

                if (ImGui::MenuItem(stringToCaption[SelectAll].c_str(), nullptr, nullptr))
                    editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::Text("%6d/%-6d %6d %s | %s | %s | %s ", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
            stringToCaption[Strings].c_str(),
            editor.IsOverwrite() ? u8"Замена " : u8"Вставка",
            editor.CanUndo() ? "*" : " ",
            u8"Текст");

        editor.Render(L"editor");

        //ImGui::End();
    }


    void InitGraphics()
    {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
        {
            return;
        }
        // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
        window = glfwCreateWindow(width, height, "Tracker Generator", NULL, NULL);
        if (window == NULL)
            return;
        glfwSetKeyCallback(window, key_callback);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync
        // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
        bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
        bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
        bool err = gladLoadGL() == 0;
#else
        bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
        if (err)
        {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            return;
        }
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        // Load Fonts        
        io.Fonts->AddFontFromFileTTF("fonts/a_FuturaOrto.TTF", 20, NULL, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->AddFontFromFileTTF("fonts/FiraCode/ttf/FiraCode-Regular.ttf", 30, NULL, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->AddFontFromFileTTF("fonts/FiraCode/ttf/FiraCode-SemiBold.ttf", 30, NULL, io.Fonts->GetGlyphRangesCyrillic());
        editor.regular_font = io.Fonts->Fonts[1];
        editor.bold_font = io.Fonts->Fonts[2];
    }

    // -----------------------------
    // Free graphic resources
    // -----------------------------
    void TerminateGraphics(void)
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    // -----------------------------
    //
    // -----------------------------
    void ShowFontSelector(const char* label)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font_current = ImGui::GetFont();
        if (ImGui::BeginCombo(label, font_current->GetDebugName()))
        {
            for (int n = 0; n < io.Fonts->Fonts.Size; n++)
            {
                ImFont* regular_font = io.Fonts->Fonts[n];
                ImGui::PushID((void*)regular_font);
                if (ImGui::Selectable(regular_font->GetDebugName(), regular_font == font_current))
                    io.FontDefault = regular_font;
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
    }

    // Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
    // Here we use the simplified Combo() api that packs items into a single literal string. Useful for quick combo boxes where the choices are known locally.
    bool ShowStyleSelector(const char* label)
    {
        static int style_idx = -1;
        if (ImGui::Combo(label, &style_idx, "Classic\0Dark\0Light\0"))
        {
            switch (style_idx)
            {
            case 0: ImGui::StyleColorsClassic(); break;
            case 1: ImGui::StyleColorsDark(); break;
            case 2: ImGui::StyleColorsLight(); break;
            }
            return true;
        }
        return false;
    }
    // -----------------------------
    //
    // -----------------------------
    void ShowStyleEditor(ImGuiStyle* ref)
    {
        // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it compares to an internally stored reference)
        ImGuiStyle& style = ImGui::GetStyle();
        static ImGuiStyle ref_saved_style;

        // Default to using internal storage as reference
        static bool init = true;
        if (init && ref == NULL)
            ref_saved_style = style;
        init = false;
        if (ref == NULL)
            ref = &ref_saved_style;

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

        if (ShowStyleSelector("Colors##Selector"))
            ref_saved_style = style;
        ShowFontSelector("Fonts##Selector");

        // Simplified Settings
        if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
            style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding

        { bool window_border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &window_border)) style.WindowBorderSize = window_border ? 1.0f : 0.0f; }
        ImGui::SameLine();
        { bool frame_border = (style.FrameBorderSize > 0.0f); if (ImGui::Checkbox("FrameBorder", &frame_border)) style.FrameBorderSize = frame_border ? 1.0f : 0.0f; }
        ImGui::SameLine();
        { bool popup_border = (style.PopupBorderSize > 0.0f); if (ImGui::Checkbox("PopupBorder", &popup_border)) style.PopupBorderSize = popup_border ? 1.0f : 0.0f; }

        // Save/Revert button
        if (ImGui::Button("Save Ref"))
            *ref = ref_saved_style = style;
        ImGui::SameLine();
        if (ImGui::Button("Revert Ref"))
            style = *ref;
        ImGui::SameLine();
        // HelpMarker("Save/Revert in local non-persistent storage. Default Colors definition are not affected. Use \"Export\" below to save them somewhere.");

        ImGui::Separator();

        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Sizes"))
            {
                ImGui::Text("Main");
                ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
                ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
                ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
                ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
                ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
                ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
                ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
                ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
                ImGui::Text("Borders");
                ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
                ImGui::Text("Rounding");
                ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
                ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
                ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
                ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
                ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
                ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
                ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
                ImGui::Text("Alignment");
                ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
                int window_menu_button_position = style.WindowMenuButtonPosition + 1;
                if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                    style.WindowMenuButtonPosition = window_menu_button_position - 1;
                ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
                ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine();
                // HelpMarker("Alignment applies when a button is larger than its text content.");
                ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine();
                // HelpMarker("Alignment applies when a selectable is larger than its text content.");
                ImGui::Text("Safe Area Padding"); ImGui::SameLine();
                //HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
                ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Colors"))
            {
                static int output_dest = 0;
                static bool output_only_modified = true;
                if (ImGui::Button("Export"))
                {
                    if (output_dest == 0)
                        ImGui::LogToClipboard();
                    else
                        ImGui::LogToTTY();
                    ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
                    for (int i = 0; i < ImGuiCol_COUNT; i++)
                    {
                        const ImVec4& col = style.Colors[i];
                        const char* name = ImGui::GetStyleColorName(i);
                        if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                            ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                    }
                    ImGui::LogFinish();
                }
                ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
                ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

                static ImGuiTextFilter filter;
                filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

                static ImGuiColorEditFlags alpha_flags = 0;
                ImGui::RadioButton("Opaque", &alpha_flags, 0); ImGui::SameLine();
                ImGui::RadioButton("Alpha", &alpha_flags, ImGuiColorEditFlags_AlphaPreview); ImGui::SameLine();
                ImGui::RadioButton("Both", &alpha_flags, ImGuiColorEditFlags_AlphaPreviewHalf); ImGui::SameLine();
                //HelpMarker("In the color list:\nLeft-click on colored square to open color picker,\nRight-click to open edit options menu.");

                ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
                ImGui::PushItemWidth(-160);
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!filter.PassFilter(name))
                        continue;
                    ImGui::PushID(i);
                    ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                    if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                    {
                        // Tips: in a real user application, you may want to merge and use an icon font into the main font, so instead of "Save"/"Revert" you'd use icons.
                        // Read the FAQ and misc/fonts/README.txt about using icon fonts. It's really easy and super convenient!
                        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i];
                        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) style.Colors[i] = ref->Colors[i];
                    }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                    ImGui::TextUnformatted(name);
                    ImGui::PopID();
                }
                ImGui::PopItemWidth();
                ImGui::EndChild();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Fonts"))
            {
                ImGuiIO& io = ImGui::GetIO();
                ImFontAtlas* atlas = io.Fonts;
                // HelpMarker("Read FAQ and misc/fonts/README.txt for details on font loading.");
                ImGui::PushItemWidth(120);
                for (int i = 0; i < atlas->Fonts.Size; i++)
                {
                    ImFont* regular_font = atlas->Fonts[i];
                    ImGui::PushID(regular_font);
                    bool font_details_opened = ImGui::TreeNode(regular_font, "Font %d: \"%s\"\n%.2f px, %d glyphs, %d file(s)", i, regular_font->ConfigData ? regular_font->ConfigData[0].Name : "", regular_font->FontSize, regular_font->Glyphs.Size, regular_font->ConfigDataCount);
                    ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) { io.FontDefault = regular_font; }
                    if (font_details_opened)
                    {
                        ImGui::PushFont(regular_font);
                        ImGui::Text("The quick brown fox jumps over the lazy dog");
                        ImGui::PopFont();
                        ImGui::DragFloat("Font scale", &regular_font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
                        ImGui::SameLine();
                        //HelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
                        ImGui::InputFloat("Font offset", &regular_font->DisplayOffset.y, 1, 1, "%.0f");
                        ImGui::Text("Ascent: %f, Descent: %f, Height: %f", regular_font->Ascent, regular_font->Descent, regular_font->Ascent - regular_font->Descent);
                        ImGui::Text("Fallback character: '%c' (U+%04X)", regular_font->FallbackChar, regular_font->FallbackChar);
                        ImGui::Text("Ellipsis character: '%c' (U+%04X)", regular_font->EllipsisChar, regular_font->EllipsisChar);
                        const float surface_sqrt = sqrtf((float)regular_font->MetricsTotalSurface);
                        ImGui::Text("Texture surface: %d pixels (approx) ~ %dx%d", regular_font->MetricsTotalSurface, (int)surface_sqrt, (int)surface_sqrt);
                        for (int config_i = 0; config_i < regular_font->ConfigDataCount; config_i++)
                            if (const ImFontConfig* cfg = &regular_font->ConfigData[config_i])
                                ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
                        if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", regular_font->Glyphs.Size))
                        {
                            // Display all glyphs of the fonts in separate pages of 256 characters
                            for (int base = 0; base < 0x10000; base += 256)
                            {
                                int count = 0;
                                for (int n = 0; n < 256; n++)
                                    count += regular_font->FindGlyphNoFallback((ImWchar)(base + n)) ? 1 : 0;
                                if (count > 0 && ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                                {
                                    float cell_size = regular_font->FontSize * 1;
                                    float cell_spacing = style.ItemSpacing.y;
                                    ImVec2 base_pos = ImGui::GetCursorScreenPos();
                                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                                    for (int n = 0; n < 256; n++)
                                    {
                                        ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                                        ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                                        const ImFontGlyph* glyph = regular_font->FindGlyphNoFallback((ImWchar)(base + n));
                                        draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                                        if (glyph)
                                            regular_font->RenderChar(draw_list, cell_size, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
                                        if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
                                        {
                                            ImGui::BeginTooltip();
                                            ImGui::Text("Codepoint: U+%04X", base + n);
                                            ImGui::Separator();
                                            ImGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
                                            ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                                            ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                                            ImGui::EndTooltip();
                                        }
                                    }
                                    ImGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
                {
                    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
                    ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
                    ImGui::TreePop();
                }

                // HelpMarker("Those are old settings provided for convenience.\nHowever, the _correct_ way of scaling your UI is currently to reload your font at the designed size, rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.");
                static float window_scale = 1.0f;
                if (ImGui::DragFloat("window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.2f"))   // scale only this window
                    ImGui::SetWindowFontScale(window_scale);
                ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.2f");      // scale everything
                ImGui::PopItemWidth();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Rendering"))
            {
                ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines); ImGui::SameLine();
                //HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
                ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
                ImGui::PushItemWidth(100);
                ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, "%.2f", 2.0f);
                if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
                ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
                ImGui::PopItemWidth();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::PopItemWidth();
    }
    // -----------------------------
    // Main loop
    // -----------------------------
    void worker(void)
    {
        isRunning = true;
        static bool open = true;
        InitGraphics();
        Notify<MainWindowObservers::OnRunEvent>();
        // -------------
        // Main loop
        // -------------
        while (!boost::this_thread::interruption_requested() && !glfwWindowShouldClose(window))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();





            ShowExampleAppDockSpace(&open);

            // static bool p_open = true;
             //ShowExampleAppLog(&open);
            log_win.Draw(u8"Логи", &open);
            RenderEditor();
            ImGui::End();
            ImGui::Begin(u8"Управление", nullptr);
            if (ImGui::Button(u8"Обработать", ImVec2(-1, 0)))
            {
                Notify<MainWindowObservers::OnButtonPressEvent>(u8"Обработать");
            }


            if (ImGui::Button(u8"Открыть", ImVec2(-1, 0)))
            {
                ImGui::OpenPopup("Open File");
                show_open_dialog = false;
            }
            else if (ImGui::Button(u8"Сохранить", ImVec2(-1, 0)))
            {
                ImGui::OpenPopup("Save File");
                show_save_dialog = false;
            }
            else if (ImGui::Button(u8"Выбрать папку", ImVec2(-1, 0)))
            {
                ImGui::OpenPopup("Select Directory##popup");
                show_select_dialog = false;
            }
            //Show an open/save/select file dialog. Last argument provides a list of supported files. Selecting other files will show error. If "*.*" is provided, all files can be opened.
            if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(600, 300), ".h,.cpp,.c"))
            {
                printf("%s\n", file_dialog.selected_fn.c_str());
                printf("%s\n", file_dialog.selected_path.c_str());
            }

            if (file_dialog.showFileDialog("Select Directory##popup", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT, ImVec2(600, 300)))
            {
                printf("%s\n", file_dialog.selected_fn.c_str());
                printf("%s\n", file_dialog.selected_path.c_str());
            }

            if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(600, 300), ".png,.jpg,.bmp"))
            {
                printf("%s\n", file_dialog.selected_fn.c_str());
                printf("%s\n", file_dialog.ext.c_str());
                printf("%s\n", file_dialog.selected_path.c_str());
            }


            /*
            if (ImGui::Button(u8"Сохранить", ImVec2(-1, 0)))
            {
                Notify<MainWindowObservers::OnButtonPressEvent>(u8"Сохранить");
            }
            */
            ImGui::End();

            static bool is_open = true;
            if (ImGui::Begin(u8"Стили", &is_open))
            {
                ImGuiStyle& style = ImGui::GetStyle();
                ShowStyleEditor(&style);
                //  ImGui::End();
            }
            ImGui::End();

            Notify<MainWindowObservers::OnWorkerEvent>();

            glClearColor(0, 0, 0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            // Rendering
            //ImGuiIO& io = ImGui::GetIO();
            //ImFont* font = io.Fonts->Fonts[1];
            //ImGui::PushFont(font);
           // io.FontDefault = font;
            ImGui::Render();
            //ImGui::PopFont();
           // font = io.Fonts->Fonts[0];
           // io.FontDefault = font;

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
        TerminateGraphics();
        isRunning = false;
        Notify<MainWindowObservers::OnCloseEvent>();
    }
    // -----------------------------
    //
    // -----------------------------
    void Run(void)
    {
        if (!isRunning)
        {
            thr = new boost::thread(&MainWindow::worker, this);
            isRunning = true;
        }
    }
    // -----------------------------
    //
    // -----------------------------
    void Stop(void)
    {
        if (isRunning)
        {
            thr->interrupt();
            thr->join();
            delete thr;
            thr = nullptr;
            isRunning = false;
        }
    }

};

// -----------------------------
//
// -----------------------------
// Application: our Observer.
class Application
{
public:
    // -----------------------------
    //
    // -----------------------------
    explicit Application(MainWindow& worker) :
        worker_(worker)
    {
        finished = false;

        worker_.Register < MainWindowObservers::OnCloseEvent >([this](void)
            {
                OnClose();
            });

        worker_.Register < MainWindowObservers::OnRunEvent >([this](void)
            {
                OnRun();
            });
        worker_.Register < MainWindowObservers::OnWorkerEvent >([this](void)
            {
                OnWorker();
            });
        worker_.Register < MainWindowObservers::OnButtonPressEvent >([this](std::string button_name)
            {
                OnButton(button_name);
            });
        std::cout << "Events - registered" << std::endl;

        worker_.Run();

        while (worker_.isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            finished = true;
        }
        worker_.Stop();
    }
    // -----------------------------
    //
    // -----------------------------
    ~Application()
    {

    }

private:
    bool finished;
    boost::thread* thrm;
    // -----------------------------
    //
    // -----------------------------
    void OnRun()
    {
        std::cout << "On run event." << std::endl;
        log_win.AddLog(u8"On run event.\n");
    }
    // -----------------------------
    //
    // -----------------------------
    void OnClose()
    {
        std::cout << "On close event." << std::endl;
    }
    // -----------------------------
    //
    // -----------------------------
    void OnWorker()
    {

    }
    // -----------------------------
    //
    // -----------------------------

    // -----------------------------
    //
    // -----------------------------
    void OnButton(std::string button_name)
    {
        if (button_name == u8"Обработать")
        {
            std::wcout << "mLines contains " << worker_.editor.mLines.size() << " lines" << std::endl;
            Vector<String*>& lines = worker_.editor.textLinesStore.lines;
            std::wcout << "lines contains " << lines.size() << " lines" << std::endl;
            for (int i = 0; i < lines.size(); ++i)
            {
                std::wstring str((wchar_t*)lines.elementAt(i)->getWChars());
                int l=lines.elementAt(i)->length();
                std::wcout << (wchar_t*)lines.elementAt(i)->getWChars() << std::endl;
            }
        }
    }
    MainWindow& worker_;
};
// -----------------------------
//
// -----------------------------
#include<colorer/viewer/TextLinesStore.h>
#include<colorer/editor/BaseEditor.h>

int main()
{
    MainWindow worker;
    CText Source;
    Source.readFile("hello_win.c");
    Source.remove(_T('\r'));  
    std::vector<CText> lines;
    lines.push_back(CText(L"int a=0;"));
    lines.push_back(CText(L"void main(void);"));
   
    worker.editor.SetText(Source); 
    worker.editor.SetTextLines(lines);
    Application application{ worker };
}
