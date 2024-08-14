
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local CameraFollowScript = {
    data = 
    {
        lerpFactor = { value = 5, serialize = true }
    }
}

setmetatable(CameraFollowScript, {
    __index = function(t, k)
        if CameraFollowScript.data[k] then
            return CameraFollowScript.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if CameraFollowScript.data[k] then
            CameraFollowScript.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function CameraFollowScript:Start()
    -- print("CameraFollowScript started")
    -- Initialization code here
end

function CameraFollowScript:Update(dt)
    -- print("CameraFollowScript started")
    -- Update code here
    local transform = pEntity:get_component(Transform)
    if transform then
        local camera = pEntity:get_component(Camera)
        local cameraPos = camera:get_position()
        if camera then
            local pos = vec2(transform.translation.x, transform.translation.y)
            if pos then
            local newPos = mix(cameraPos, pos, (CameraFollowScript.lerpFactor*dt))
            camera:set_position(newPos)
            end
        end
    end
end

function CameraFollowScript:Exit()
    -- print("CameraFollowScript exited")
    -- Cleanup code here
end

return CameraFollowScript
