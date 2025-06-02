-- Lua GUI Script
local counter = 0
local slider_val = 50.0

function draw_gui()
    if ImGui.Begin("Lua Window") then
        ImGui.Text("Hello from Lua!")
        ImGui.Separator()
        
        if ImGui.Button("Click Me") then
            counter = counter + 1
        end
        
        ImGui.Text("Button clicked: " .. counter .. " times")
        
        local changed, new_val = ImGui.SliderFloat("Slider", slider_val, 0.0, 100.0)
        if changed then
            slider_val = new_val
        end
        
        ImGui.Text("Slider value: " .. string.format("%.1f", slider_val))
    end
    ImGui.End()
end