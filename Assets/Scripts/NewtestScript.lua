
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local NewtestScript = {
    data = 
    {
        name = { value = "NewtestScript", serialize = true }
    }
}

setmetatable(NewtestScript, {
    __index = function(t, k)
        if NewtestScript.data[k] then
            return NewtestScript.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if NewtestScript.data[k] then
            NewtestScript.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function NewtestScript:Start()
    -- print("NewtestScript started")
    -- Initialization code here
end

function NewtestScript:Update(dt)
    -- print("NewtestScript started")
    -- Update code here
end

function NewtestScript:Exit()
    -- print("NewtestScript exited")
    -- Cleanup code here
end

return NewtestScript
