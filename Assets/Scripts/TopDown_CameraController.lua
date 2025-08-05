
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local TopDown_CameraController = {
    data = 
    {
        lerpSpeed = { value = 0.5 , serialize = true }
    }
}

setmetatable(TopDown_CameraController, {
    __index = function(t, k)
        if TopDown_CameraController.data[k] then
            return TopDown_CameraController.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if TopDown_CameraController.data[k] then
            TopDown_CameraController.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function TopDown_CameraController:Start()
    -- print("TopDown_CameraController started")
    -- Initialization code here
end

function TopDown_CameraController:Update(dt)
    -- print("TopDown_CameraController started")
    -- Update code here
    local transform = pEntity:get_component(Transform)
    if transform then
        local camera = pEntity:get_component(Camera)
        local cameraPos = camera:get_position()
        if camera then
            local pos = vec2(transform.translation.x, transform.translation.y)
            if pos then
            local newPos = mix(cameraPos, pos, (TopDown_CameraController.lerpSpeed*dt))
            camera:set_position(newPos)
            end
        end
    end
end

function TopDown_CameraController:Exit()
    -- print("TopDown_CameraController exited")
    -- Cleanup code here
end

return TopDown_CameraController
