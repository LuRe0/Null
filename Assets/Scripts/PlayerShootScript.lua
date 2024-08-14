
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local PlayerShootScript = {
    data = 
    {
        name = { value = "PlayerShootScript", serialize = true }
    }
}

setmetatable(PlayerShootScript, {
    __index = function(t, k)
        if PlayerShootScript.data[k] then
            return PlayerShootScript.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if PlayerShootScript.data[k] then
            PlayerShootScript.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function PlayerShootScript:Start()
    -- print("PlayerShootScript started")
    -- Initialization code here
end

function PlayerShootScript:Update(dt)
    -- print("PlayerShootScript started")
    -- Update code here
end

function PlayerShootScript:Exit()
    -- print("PlayerShootScript exited")
    -- Cleanup code here
end

return PlayerShootScript
