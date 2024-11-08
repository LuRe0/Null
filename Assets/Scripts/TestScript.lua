
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local TestScript = {
    data = 
    {
        name = { value = "TestScript", serialize = true },
        someValue = { value = "Shows In editor", serialize = false }

    }
}

setmetatable(TestScript, {
    __index = function(t, k)
        if TestScript.data[k] then
            return TestScript.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if TestScript.data[k] then
            TestScript.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function TestScript:Start()
    -- print("TestScript started")
    -- Initialization code here
end

function TestScript:Update(dt)
    -- print("TestScript started")
    -- Update code here
end

function TestScript:Exit()
    -- print("TestScript exited")
    -- Cleanup code here
end

return TestScript
