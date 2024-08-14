
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local AnotherScript = {
    data = 
    {
        name = { value = "AnotherScript", serialize = true }
    }
}

setmetatable(AnotherScript, {
    __index = function(t, k)
        if AnotherScript.data[k] then
            return AnotherScript.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if AnotherScript.data[k] then
            AnotherScript.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function AnotherScript:Start()
    -- print("AnotherScript started")
    -- Initialization code here
end

function AnotherScript:Update(dt)
    -- print("AnotherScript started")
    -- Update code here
end

function AnotherScript:Exit()
    -- print("AnotherScript exited")
    -- Cleanup code here
end

return AnotherScript
