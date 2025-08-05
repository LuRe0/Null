
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local Deez_Behavior = {
    data = 
    {
        name = { value = "Deez_Behavior", serialize = true }
    }
}

setmetatable(Deez_Behavior, {
    __index = function(t, k)
        if Deez_Behavior.data[k] then
            return Deez_Behavior.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if Deez_Behavior.data[k] then
            Deez_Behavior.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function Deez_Behavior:Start()
    -- print("Deez_Behavior started")
    -- Initialization code here
end

function Deez_Behavior:Update(dt)
    -- print("Deez_Behavior started")
    -- Update code here
end

function Deez_Behavior:Exit()
    -- print("Deez_Behavior exited")
    -- Cleanup code here
end

return Deez_Behavior
