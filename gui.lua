-- Enhanced Aesthetic Lua GUI Script
-- Modern, clean design with beautiful visual elements

local shape_props = {
    x = 150.0,
    y = 150.0,
    size = 50.0,
    color = { 0.3, 0.7, 1.0, 1.0 } -- Beautiful blue
}

local background_color_props = {
    color = { 0.05, 0.05, 0.08, 1.0 } -- Deep dark blue
}

-- Animation and visual state
local animation_time = 0
local pulse_intensity = 0

-- Color presets for quick selection
local color_presets = {
    { name = "Ocean Blue",    color = { 0.2, 0.6, 1.0, 1.0 } },
    { name = "Sunset Orange", color = { 1.0, 0.5, 0.2, 1.0 } },
    { name = "Forest Green",  color = { 0.2, 0.8, 0.4, 1.0 } },
    { name = "Royal Purple",  color = { 0.6, 0.3, 0.9, 1.0 } },
    { name = "Rose Gold",     color = { 1.0, 0.7, 0.8, 1.0 } },
    { name = "Electric Cyan", color = { 0.0, 1.0, 1.0, 1.0 } }
}

local background_presets = {
    { name = "Midnight",      color = { 0.05, 0.05, 0.08, 1.0 } },
    { name = "Deep Space",    color = { 0.02, 0.02, 0.05, 1.0 } },
    { name = "Warm Charcoal", color = { 0.08, 0.06, 0.05, 1.0 } },
    { name = "Forest Night",  color = { 0.02, 0.08, 0.05, 1.0 } },
    { name = "Wine Dark",     color = { 0.08, 0.02, 0.05, 1.0 } }
}

function OnCppShapePositionUpdated(new_x, new_y)
    if shape_props then
        shape_props.x = new_x
        shape_props.y = new_y
    end
end

function UpdateCppShape()
    if App and App.SetShapePosition then
        App.SetShapePosition(shape_props.x, shape_props.y)
        App.SetShapeSize(shape_props.size)
        App.SetShapeColor(shape_props.color[1], shape_props.color[2], shape_props.color[3], shape_props.color[4])
    end
end

function UpdateCppBackgroundColor()
    if App and App.SetBackgroundColor then
        App.SetBackgroundColor(
            background_color_props.color[1],
            background_color_props.color[2],
            background_color_props.color[3],
            background_color_props.color[4]
        )
    end
end

-- Utility functions for enhanced visuals
function PushStyleColors()
    -- Modern dark theme with accent colors
    ImGui.PushStyleColor(ImGui.Col_WindowBg, 0.08, 0.08, 0.12, 0.95)
    ImGui.PushStyleColor(ImGui.Col_TitleBg, 0.1, 0.1, 0.15, 1.0)
    ImGui.PushStyleColor(ImGui.Col_TitleBgActive, 0.15, 0.15, 0.2, 1.0)
    ImGui.PushStyleColor(ImGui.Col_Button, 0.2, 0.25, 0.35, 1.0)
    ImGui.PushStyleColor(ImGui.Col_ButtonHovered, 0.3, 0.35, 0.45, 1.0)
    ImGui.PushStyleColor(ImGui.Col_ButtonActive, 0.4, 0.45, 0.55, 1.0)
    ImGui.PushStyleColor(ImGui.Col_SliderGrab, 0.4, 0.7, 1.0, 1.0)
    ImGui.PushStyleColor(ImGui.Col_SliderGrabActive, 0.5, 0.8, 1.0, 1.0)
    ImGui.PushStyleColor(ImGui.Col_FrameBg, 0.12, 0.12, 0.18, 1.0)
    ImGui.PushStyleColor(ImGui.Col_FrameBgHovered, 0.18, 0.18, 0.25, 1.0)
    ImGui.PushStyleColor(ImGui.Col_FrameBgActive, 0.25, 0.25, 0.32, 1.0)
    ImGui.PushStyleColor(ImGui.Col_Header, 0.15, 0.15, 0.22, 1.0)
    ImGui.PushStyleColor(ImGui.Col_HeaderHovered, 0.22, 0.22, 0.32, 1.0)
    ImGui.PushStyleColor(ImGui.Col_HeaderActive, 0.3, 0.3, 0.4, 1.0)
end

function PopStyleColors()
    ImGui.PopStyleColor(14)
end

function PushStyleVars()
    ImGui.PushStyleVar(ImGui.StyleVar_WindowRounding, 12.0)
    ImGui.PushStyleVar(ImGui.StyleVar_FrameRounding, 8.0)
    ImGui.PushStyleVar(ImGui.StyleVar_GrabRounding, 6.0)
    ImGui.PushStyleVar(ImGui.StyleVar_ItemSpacing, 8, 12)
    ImGui.PushStyleVar(ImGui.StyleVar_WindowPadding, 20, 20)
    ImGui.PushStyleVar(ImGui.StyleVar_FramePadding, 12, 8)
end

function PopStyleVars()
    ImGui.PopStyleVar(6)
end

-- Removed DrawGradientRect function as it uses unsupported ImGui functions

function CreateIconButton(label, color)
    ImGui.PushStyleColor(ImGui.Col_Button, color[1], color[2], color[3], 0.8)
    ImGui.PushStyleColor(ImGui.Col_ButtonHovered, color[1] * 1.2, color[2] * 1.2, color[3] * 1.2, 1.0)
    ImGui.PushStyleColor(ImGui.Col_ButtonActive, color[1] * 0.8, color[2] * 0.8, color[3] * 0.8, 1.0)

    local clicked = ImGui.Button(label, 60, 30)

    ImGui.PopStyleColor(3)
    return clicked
end

-- Initialize on first run
UpdateCppShape()
UpdateCppBackgroundColor()

function draw_gui()
    -- Update animation time
    animation_time = animation_time + 0.016                    -- Assume ~60fps
    pulse_intensity = (math.sin(animation_time * 2) + 1) * 0.5 -- 0 to 1

    PushStyleColors()
    PushStyleVars()

    -- Main Control Panel
    ImGui.SetNextWindowSize(400, 600, ImGui.Cond_FirstUseEver)
    ImGui.SetNextWindowPos(50, 50, ImGui.Cond_FirstUseEver)

    local window_open = true
    local window_result = ImGui.Begin("Shape Studio", window_open, ImGui.WindowFlags_NoCollapse)

    if window_result then
        -- Header with animated accent
        local header_color = { 0.3 + pulse_intensity * 0.2, 0.7, 1.0, 1.0 }
        ImGui.PushStyleColor(ImGui.Col_Text, header_color[1], header_color[2], header_color[3], header_color[4])
        local text_width = ImGui.CalcTextSize("Modern Shape Control")
        ImGui.SetCursorPosX((ImGui.GetWindowWidth() - text_width) * 0.5)
        ImGui.Text("Modern Shape Control")
        ImGui.PopStyleColor(1)

        ImGui.Spacing()
        ImGui.Separator()
        ImGui.Spacing()

        -- Shape Position Section
        local pos_header = ImGui.CollapsingHeader("Position & Transform", ImGui.TreeNodeFlags_DefaultOpen)
        if pos_header then
            ImGui.Spacing()

            -- Position controls with enhanced styling
            ImGui.Text("Position")
            ImGui.PushStyleColor(ImGui.Col_SliderGrab, 0.9, 0.3, 0.3, 1.0)
            local changed_x, new_x = ImGui.SliderFloat("X Coordinate", shape_props.x, 0.0, 800.0, "%.0f px")
            ImGui.PopStyleColor(1)

            ImGui.PushStyleColor(ImGui.Col_SliderGrab, 0.3, 0.9, 0.3, 1.0)
            local changed_y, new_y = ImGui.SliderFloat("Y Coordinate", shape_props.y, 0.0, 600.0, "%.0f px")
            ImGui.PopStyleColor(1)

            if changed_x then shape_props.x = new_x end
            if changed_y then shape_props.y = new_y end

            if changed_x or changed_y then
                App.SetShapePosition(shape_props.x, shape_props.y)
            end

            ImGui.Spacing()

            -- Size control with custom styling
            ImGui.Text("Dimensions")
            ImGui.PushStyleColor(ImGui.Col_SliderGrab, 0.8, 0.6, 0.2, 1.0)
            local changed_size, new_size = ImGui.SliderFloat("Size", shape_props.size, 10.0, 300.0, "%.0f px")
            ImGui.PopStyleColor(1)

            if changed_size then
                shape_props.size = new_size
                App.SetShapeSize(shape_props.size)
            end

            ImGui.Spacing()
        end

        -- Color Section
        local color_header = ImGui.CollapsingHeader("Color Palette", ImGui.TreeNodeFlags_DefaultOpen)
        if color_header then
            ImGui.Spacing()

            -- Color picker
            ImGui.Text("Primary Color")
            local changed_color, new_color = ImGui.ColorEdit4("##ShapeColor", shape_props.color,
                ImGui.ColorEditFlags_NoLabel)
            if changed_color then
                shape_props.color = new_color
                App.SetShapeColor(shape_props.color[1], shape_props.color[2], shape_props.color[3], shape_props.color[4])
            end

            ImGui.Spacing()

            -- Color presets
            ImGui.Text("Quick Presets")
            local cols = 3
            for i, preset in ipairs(color_presets) do
                if (i - 1) % cols ~= 0 then
                    ImGui.SameLine()
                end

                if CreateIconButton(preset.name:sub(1, 1), preset.color) then
                    shape_props.color = { preset.color[1], preset.color[2], preset.color[3], preset.color[4] }
                    App.SetShapeColor(shape_props.color[1], shape_props.color[2], shape_props.color[3],
                        shape_props.color[4])
                end

                if ImGui.IsItemHovered() then
                    ImGui.SetTooltip(preset.name)
                end
            end

            ImGui.Spacing()
        end

        -- Environment Section
        local env_header = ImGui.CollapsingHeader("Environment", ImGui.TreeNodeFlags_DefaultOpen)
        if env_header then
            ImGui.Spacing()

            -- Background color
            ImGui.Text("Background Atmosphere")
            local changed_bg, new_bg = ImGui.ColorEdit4("##BgColor", background_color_props.color,
                ImGui.ColorEditFlags_NoLabel)
            if changed_bg then
                background_color_props.color = new_bg
                UpdateCppBackgroundColor()
            end

            ImGui.Spacing()

            -- Background presets
            ImGui.Text("Mood Presets")
            for i, preset in ipairs(background_presets) do
                if i > 1 then ImGui.SameLine() end

                if CreateIconButton(preset.name:sub(1, 1), { preset.color[1] * 3, preset.color[2] * 3, preset.color[3] * 3, 1.0 }) then
                    background_color_props.color = { preset.color[1], preset.color[2], preset.color[3], preset.color[4] }
                    UpdateCppBackgroundColor()
                end

                if ImGui.IsItemHovered() then
                    ImGui.SetTooltip(preset.name)
                end
            end

            ImGui.Spacing()
        end

        -- Quick Actions Section
        local actions_header = ImGui.CollapsingHeader("Quick Actions")
        if actions_header then
            ImGui.Spacing()

            -- Center button
            ImGui.PushStyleColor(ImGui.Col_Button, 0.2, 0.5, 0.8, 1.0)
            ImGui.PushStyleColor(ImGui.Col_ButtonHovered, 0.3, 0.6, 0.9, 1.0)
            if ImGui.Button("Center Shape", -1, 40) then
                shape_props.x = 400
                shape_props.y = 300
                App.SetShapePosition(shape_props.x, shape_props.y)
            end
            ImGui.PopStyleColor(2)

            ImGui.Spacing()

            -- Random color button
            ImGui.PushStyleColor(ImGui.Col_Button, 0.7, 0.3, 0.7, 1.0)
            ImGui.PushStyleColor(ImGui.Col_ButtonHovered, 0.8, 0.4, 0.8, 1.0)
            if ImGui.Button("Random Color", -1, 40) then
                shape_props.color = {
                    math.random(),
                    math.random(),
                    math.random(),
                    1.0
                }
                App.SetShapeColor(shape_props.color[1], shape_props.color[2], shape_props.color[3], shape_props.color[4])
            end
            ImGui.PopStyleColor(2)

            ImGui.Spacing()
        end

        -- Info footer
        ImGui.Separator()
        ImGui.Spacing()
        ImGui.PushStyleColor(ImGui.Col_Text, 0.6, 0.6, 0.7, 1.0)
        ImGui.PopStyleColor(1)
    end
    ImGui.End()

    PopStyleVars()
    PopStyleColors()
end
