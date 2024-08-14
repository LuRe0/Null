
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local SomeScript = {
    data = 
    {
        name = { value = "SomeScript", serialize = true }
    }
}

setmetatable(SomeScript, {
    __index = function(t, k)
        if SomeScript.data[k] then
            return SomeScript.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if SomeScript.data[k] then
            SomeScript.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function SomeScript:Start()
    -- print("SomeScript started")
    -- Initialization code here
end

function SomeScript:Update(dt)
    -- print("SomeScript started")
    -- Update code here
end

function SomeScript:Exit()
    -- print("SomeScript exited")
    -- Cleanup code here
end

return SomeScript
