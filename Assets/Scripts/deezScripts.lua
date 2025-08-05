
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local deezScripts = {
    data = 
    {
        name = { value = "deezScripts", serialize = true }
    }
}

setmetatable(deezScripts, {
    __index = function(t, k)
        if deezScripts.data[k] then
            return deezScripts.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if deezScripts.data[k] then
            deezScripts.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function deezScripts:Start()
    -- print("deezScripts started")
    -- Initialization code here
end

function deezScripts:Update(dt)
    -- print("deezScripts started")
    -- Update code here
end

function deezScripts:Exit()
    -- print("deezScripts exited")
    -- Cleanup code here
end

return deezScripts
