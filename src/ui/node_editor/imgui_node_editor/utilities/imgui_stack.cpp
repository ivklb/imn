
#include "imgui_stack.h"
#include <imgui_internal.h>

using namespace ImGui;


static ImGuiLayout* FindLayout(ImGuiID id, ImGuiLayoutType type)
{
    IM_ASSERT(type == ImGuiLayoutType_Horizontal || type == ImGuiLayoutType_Vertical);

    ImGuiWindow* window = GetCurrentWindow();
    ImGuiLayout* layout = (ImGuiLayout*)window->DC.Layouts.GetVoidPtr(id);
    if (!layout)
        return NULL;

    if (layout->Type != type)
    {
        layout->Type = type;
        layout->MinimumSize = ImVec2(0.0f, 0.0f);
        layout->Items.clear();
    }

    return layout;
}

static ImGuiLayout* CreateNewLayout(ImGuiID id, ImGuiLayoutType type, ImVec2 size)
{
    IM_ASSERT(type == ImGuiLayoutType_Horizontal || type == ImGuiLayoutType_Vertical);

    ImGuiWindow* window = GetCurrentWindow();

    ImGuiLayout* layout = IM_NEW(ImGuiLayout)(id, type);
    layout->Size = size;

    window->DC.Layouts.SetVoidPtr(id, layout);

    return layout;
}

static void BeginLayout(ImGuiID id, ImGuiLayoutType type, ImVec2 size, float align)
{
    ImGuiWindow* window = GetCurrentWindow();

    PushID(id);

    // Find or create
    ImGuiLayout* layout = FindLayout(id, type);
    if (!layout)
        layout = CreateNewLayout(id, type, size);

    layout->Live = true;

    PushLayout(layout);

    if (layout->Size.x != size.x || layout->Size.y != size.y)
        layout->Size = size;

    if (align < 0.0f)
        layout->Align = -1.0f;
    else
        layout->Align = ImClamp(align, 0.0f, 1.0f);

    // Start capture
    layout->CurrentItemIndex = 0;

    layout->CurrentSize.x = layout->Size.x > 0.0f ? layout->Size.x : layout->MinimumSize.x;
    layout->CurrentSize.y = layout->Size.y > 0.0f ? layout->Size.y : layout->MinimumSize.y;

    layout->StartPos = window->DC.CursorPos;
    layout->StartCursorMaxPos = window->DC.CursorMaxPos;

    if (type == ImGuiLayoutType_Vertical)
    {
        // Push empty item to recalculate cursor position.
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        Dummy(ImVec2(0.0f, 0.0f));
        PopStyleVar();

        // Indent horizontal position to match edge of the layout.
        layout->Indent = layout->StartPos.x - window->DC.CursorPos.x;
        SignedIndent(layout->Indent);
    }

    BeginLayoutItem(*layout);
}

static void ImGui::EndLayout(ImGuiLayoutType type)
{
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(window->DC.CurrentLayout);
    IM_ASSERT(window->DC.CurrentLayout->Type == type);

    ImGuiLayout* layout = window->DC.CurrentLayout;

    EndLayoutItem(*layout);

    if (layout->CurrentItemIndex < layout->Items.Size)
        layout->Items.resize(layout->CurrentItemIndex);

    if (layout->Type == ImGuiLayoutType_Vertical)
        SignedIndent(-layout->Indent);

    PopLayout(layout);

    const bool auto_width  = layout->Size.x <= 0.0f;
    const bool auto_height = layout->Size.y <= 0.0f;

    ImVec2 new_size = layout->Size;
    if (auto_width)
        new_size.x = layout->CurrentSize.x;
    if (auto_height)
        new_size.y = layout->CurrentSize.y;

    ImVec2 new_minimum_size = CalculateLayoutSize(*layout, true);

    if (new_minimum_size.x != layout->MinimumSize.x || new_minimum_size.y != layout->MinimumSize.y)
    {
        layout->MinimumSize = new_minimum_size;

        // Shrink
        if (auto_width)
            new_size.x = new_minimum_size.x;
        if (auto_height)
            new_size.y = new_minimum_size.y;
    }

    if (!auto_width)
        new_size.x = layout->Size.x;
    if (!auto_height)
        new_size.y = layout->Size.y;

    layout->CurrentSize = new_size;

    ImVec2 measured_size = new_size;
    if ((auto_width || auto_height) && layout->Parent)
    {
        if (layout->Type == ImGuiLayoutType_Horizontal && auto_width && layout->Parent->CurrentSize.x > 0)
            layout->CurrentSize.x = layout->Parent->CurrentSize.x;
        else if (layout->Type == ImGuiLayoutType_Vertical && auto_height && layout->Parent->CurrentSize.y > 0)
            layout->CurrentSize.y = layout->Parent->CurrentSize.y;

        BalanceLayoutSprings(*layout);

        measured_size = layout->CurrentSize;
    }

    layout->CurrentSize = new_size;

    PopID();

    ImVec2 current_layout_item_max = ImVec2(0.0f, 0.0f);
    if (window->DC.CurrentLayoutItem)
        current_layout_item_max = ImMax(window->DC.CurrentLayoutItem->MeasuredBounds.Max, layout->StartPos + new_size);

    window->DC.CursorPos    = layout->StartPos;
    window->DC.CursorMaxPos = layout->StartCursorMaxPos;
    ItemSize(new_size);
    ItemAdd(ImRect(layout->StartPos, layout->StartPos + measured_size), 0);

    if (window->DC.CurrentLayoutItem)
        window->DC.CurrentLayoutItem->MeasuredBounds.Max = current_layout_item_max;

    if (layout->Parent == NULL)
        BalanceChildLayouts(*layout);

    //window->DrawList->AddRect(layout->StartPos, layout->StartPos + measured_size, IM_COL32(0,255,0,255));           // [DEBUG]
    //window->DrawList->AddRect(window->DC.LastItemRect.Min, window->DC.LastItemRect.Max, IM_COL32(255,255,0,255));   // [DEBUG]
}


void ImGui::BeginHorizontal(const char* str_id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
{
    ImGuiWindow* window = GetCurrentWindow();
    BeginLayout(window->GetID(str_id), ImGuiLayoutType_Horizontal, size, align);
}

void ImGui::BeginHorizontal(const void* ptr_id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
{
    ImGuiWindow* window = GetCurrentWindow();
    BeginLayout(window->GetID(ptr_id), ImGuiLayoutType_Horizontal, size, align);
}

void ImGui::BeginHorizontal(int id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
{
    ImGuiWindow* window = GetCurrentWindow();
    BeginLayout(window->GetID((void*)(intptr_t)id), ImGuiLayoutType_Horizontal, size, align);
}

void ImGui::EndHorizontal()
{
    EndLayout(ImGuiLayoutType_Horizontal);
}

void ImGui::BeginVertical(const char* str_id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
{
    ImGuiWindow* window = GetCurrentWindow();
    BeginLayout(window->GetID(str_id), ImGuiLayoutType_Vertical, size, align);
}

void ImGui::BeginVertical(const void* ptr_id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
{
    ImGuiWindow* window = GetCurrentWindow();
    BeginLayout(window->GetID(ptr_id), ImGuiLayoutType_Vertical, size, align);
}

void ImGui::BeginVertical(int id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
{
    ImGuiWindow* window = GetCurrentWindow();
    BeginLayout(window->GetID((void*)(intptr_t)id), ImGuiLayoutType_Vertical, size, align);
}

void ImGui::EndVertical()
{
    EndLayout(ImGuiLayoutType_Vertical);
}

// Inserts spring separator in layout
//      weight <= 0     : spring will always have zero size
//      weight > 0      : power of current spring
//      spacing < 0     : use default spacing if pos_x == 0, no spacing if pos_x != 0
//      spacing >= 0    : enforce spacing amount
void ImGui::Spring(float weight/* = 1.0f*/, float spacing/* = -1.0f*/)
{
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(window->DC.CurrentLayout);

    AddLayoutSpring(*window->DC.CurrentLayout, weight, spacing);
}
