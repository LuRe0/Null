
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local DeathBox = {
    data = 
    {
        name = { value = "DeathBox", serialize = true }
    }
}

setmetatable(DeathBox, {
    __index = function(t, k)
        if DeathBox.data[k] then
            return DeathBox.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if DeathBox.data[k] then
            DeathBox.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function DeathBox:Start()
    -- print("DeathBox started")
    -- Initialization code here
end

function DeathBox:Update(dt)
    -- print("DeathBox started")
    -- Update code here
end

function DeathBox:Exit()
    -- print("DeathBox exited")
    -- Cleanup code here
end



function DeathBox:OnCollisionEnter(otherEntity)
    -- Check if otherEntity is valid
    if otherEntity then
        if(otherEntity.name == "ball") then
            Trace.debug("Here")
            if Scene then
                Scene:restart()
            end
        end
    end
end

return DeathBox
